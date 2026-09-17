#include "main.h"
#include "ScreenManager.h"
#include "screens/HomeScreen.h"
#include "ThermostatController.h"

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);
//I2C_eeprom eeprom(EEPROM_I2C_ADDR, EEPROM_I2C_SIZE);
//extEEPROM eeprom(kbits_4, 1, 16, 0x50);
hw_timer_t* tim1 = NULL;
ESP32Time rtc;
RTC_DS1307 ds1307;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp(&oneWire);
Preferences nvs;

//these have to be in the same order as ScreenManager::Button
static const uint8_t inputs[] = {BTN_UP_PIN, BTN_DOWN_PIN, BTN_LEFT_PIN, BTN_RIGHT_PIN, BTN_MODE_PIN};
ArduinoButtonReader<5> buttons(inputs);
Terminal terminal;
ScreenManager screenManager;

ThermostatController thermostat;

void setup() {

	esp_log_level_set("express", ESP_LOG_INFO);

	Serial.begin(SERIAL_BAUD);
	Serial.println("============================");
	Serial.println("  ESP32 Digital Thermostat  ");
	Serial.println("============================");

	terminal.init();
	terminal.setCommandHandler(handleTerminalCommand);

	for(auto &input : inputs) {
		pinMode(input, INPUT_PULLUP);
	}
	pinMode(LED_BUILTIN_PIN, OUTPUT);
	pinMode(TEMP_SENSOR_PIN, INPUT);
	pinMode(ANALOG_CTRL_PIN, OUTPUT);

	log_i("Initialising temperature sensor...");
	temp.begin();
	DeviceAddress addr;
	if(temp.getAddress(addr, 0)) {
		Serial.print("Found sensor with ID 0x");
		for(int i = 0; i < 8; i++) {
			if(addr[i] < 0x10) {
				Serial.print('0');
			}
			Serial.print(addr[i], HEX);
		}
		Serial.println();
	}
	else {
		log_w("Unable to find address for temperature sensor.");
	}

	temp.setResolution(12);
	temp.setWaitForConversion(false);

	log_i("Initialising RTC...");
	// setenv("TZ", "Europe/Berlin", 1);
	setenv("TZ", "UTC-2", 1);
	tzset();
	if(ds1307.begin()) {
		log_i("RTC initialized successfully");
		if(!ds1307.isrunning()) {
			log_i("RTC is not running. Setting time to 2000-01-01.");
			ds1307.adjust(DateTime());
		}
		DateTime now = ds1307.now();
		log_i("Synchronizing internal RTC with %s", now.timestamp().c_str());
		rtc.setTime(now.unixtime());
		// rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
		//TODO: sync time with NTP
		//rtc.setTime(0, 12, 10, 17, 10, 2023);
		// configTime(3600, 0, "pool.ntp.org", "time.nist.gov");
		log_i("Current internal RTC time: %s", rtc.getDateTime().c_str());
	}
	else {
		log_e("RTC initialization failed. Using internal RTC only.");
	}

	log_i("Initialising button handlers...");
	buttons.setDetectLongPress(ScreenManager::BTN_RIGHT, true);
	buttons.setDetectLongPress(ScreenManager::BTN_LEFT, true);
	buttons.setDetectLongPress(ScreenManager::BTN_MODE, true);
	buttons.setInputRepeat(ScreenManager::BTN_DOWN, true);
	buttons.setInputRepeat(ScreenManager::BTN_UP, true);

	buttons.setOnPressListener([](uint8_t index, bool repeat) {
		screenManager.handleButtonPress(static_cast<ScreenManager::Button>(index));
	});

	buttons.setOnLongPressListener([](uint8_t index) {
		screenManager.handleButtonLongPress(static_cast<ScreenManager::Button>(index));
	});

	//run isr at 1000Hz
	tim1 = timerBegin(1000000);
	timerAttachInterrupt(tim1, []() { buttons.readInputs_isr(); });
	timerAlarm(tim1, 1000, true, 0);
	timerStart(tim1);

	log_i("Initialising LCD...");
	screenManager.init();
	lcd.setCursor(0, 0);
	lcd.print("ESP32 Thermostat");
	lcd.setCursor(0, 1);
	lcd.print("  ver: 1.0      ");

	// log_i("Initialising EEPROM...");
	// eeprom.begin();
	// ThermostatController::PersistentConfig config;
	// eeprom.readBlock(0, (byte*)&config, sizeof(config));
	// if(thermostat.loadConfig(config)) {
	// 	log_i("Successfully loaded configuration from EEPROM.");
	// }

	log_i("Initialising Filesystem...");
	if(!LittleFS.begin()) {
		log_w("Filesystem is not formatted. Formatting...");
		LittleFS.format();
		if(!LittleFS.begin()) {
			log_e("FATAL ERROR: Filesystem initialisation failed. System will not continue initializing...");
			while(1);
		}
	}

	// Load config
	log_i("Loading system config...");
	// nvs.begin("thermostat", false);
	if(!loadThermostatConfig()) {
		//create default config
		log_i("Writing default thermostat config...");
		saveThermostatConfig();
	}

	log_i("Starting tasks...");
	startReadTemperatureTask();

	startLogTemperatureTask();

	startTickThermostatLogicTask();

	delay(1000);

	// WiFi setup
	// startWifiTask();

	// HTTP server setup
	// startHTTPTask();

	//wait until WiFi is configured
	// xSemaphoreTake(wifiSemaphore, 5000 / portTICK_PERIOD_MS);

	delay(500);

	screenManager.push(new HomeScreen());

	log_i("Initialisation complete.");

	delay(1000);
}

void loop() {
	buttons.executeHandlers();
	screenManager.update();
	terminal.update();

	delay(1);
}

BaseType_t startReadTemperatureTask() {
	return xTaskCreate(
		taskReadTemperature,
		"Read temperature",
		1000,
		nullptr,
		20,
		nullptr
	);
}

void taskReadTemperature(void* pvParameters) {
	while(true) {
		temp.requestTemperatures();
		float t = temp.getTempCByIndex(0);
		thermostat.sendTemperatureUpdate(t); //+ (float)random(-10, 10)
		//Serial.println("Got temperature: " + String(t));
		delay(1000);
	}
}

BaseType_t startLogTemperatureTask() {
	return xTaskCreate(
		taskLogTemperature,
		"Log temperature",
		5000,
		nullptr,
		4,
		nullptr
	);
}

void taskLogTemperature(void* pvParameters) {
	while(true) {
		if(Time::now().time > 1736115867000) 
			logCurrentTemperature();
		delay(1000);
	}
}

BaseType_t startTickThermostatLogicTask() {
	return xTaskCreate(
		taskTickThermostatLogic,
		"Tick thermostat logic",
		5000,
		nullptr,
		15,
		nullptr
	);
}

void taskTickThermostatLogic(void* pvParameters) {
	while(true) {
		thermostat.update();
		delay(1000);
	}
}


