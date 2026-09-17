#include "main.h"
#include "ScreenManager.h"
#include "screens/HomeScreen.h"
#include "ThermostatController.h"

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);
//I2C_eeprom eeprom(EEPROM_I2C_ADDR, EEPROM_I2C_SIZE);
//extEEPROM eeprom(kbits_4, 1, 16, 0x50);
hw_timer_t* tim1 = NULL;
ESP32Time rtc;
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
	// rtc.setClockSource(STM32RTC::LSE_CLOCK);
	// rtc.begin();
	// if(!rtc.isConfigured()) {
	// 	rtc.setDate(1, 1, 10);
	// 	rtc.setTime(0, 0, 0);
	// }
	//TODO: sync time with NTP
	//rtc.setTime(0, 12, 10, 17, 10, 2023);
	configTime(3600, 0, "pool.ntp.org", "time.nist.gov");
	Serial.println("Current time: " + rtc.getDateTime());

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
	}

	if(!loadThermostatConfig()) {
		//create default config
		log_i("Writing default thermostat config...");
		saveThermostatConfig();
	}

	log_i("Starting tasks...");
	xTaskCreate(
		taskReadTemperature,
		"Read temperature",
		1000,
		nullptr,
		20,
		nullptr
	);

	xTaskCreate(
		taskLogTemperature,
		"Log temperature",
		5000,
		nullptr,
		4,
		nullptr
	);

	xTaskCreate(
		taskTickThermostatLogic,
		"Tick thermostat logic",
		5000,
		nullptr,
		15,
		nullptr
	);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	log_i("Setting up WiFi...");
	WiFi.setHostname(HOSTNAME);

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

void taskReadTemperature(void* pvParameters) {
	while(true) {
		temp.requestTemperatures();
		float t = temp.getTempCByIndex(0);
		thermostat.sendTemperatureUpdate(t); //+ (float)random(-10, 10)
		//Serial.println("Got temperature: " + String(t));
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void taskLogTemperature(void* pvParameters) {
	while(true) {
		if(Time::now().time > 1736115867000) 
			logCurrentTemperature();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void taskTickThermostatLogic(void* pvParameters) {
	while(true) {
		thermostat.update();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void taskReconnectWifi(void* pvParameters) {
	WiFiConfig* wifi = (WiFiConfig*)pvParameters;
	while(true) {
		log_i("Reconnecting to WiFi...");
		WiFi.disconnect(true);

		if(connectWiFi(*wifi)) {
			log_i("WiFi connected");
			log_i("IP address: %s", WiFi.localIP().toString().c_str());

			break;
		}

		log_e("Connection failed. Status: %d", WiFi.status());

		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	delete wifi;

	vTaskDelete(NULL);
}
