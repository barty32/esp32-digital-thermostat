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
AsyncWebServer server(SERVER_PORT);
CorsMiddleware cors;

//these have to be in the same order as ScreenManager::Button
static const uint8_t inputs[] = {BTN_UP_PIN, BTN_DOWN_PIN, BTN_LEFT_PIN, BTN_RIGHT_PIN, BTN_MODE_PIN};
ArduinoButtonReader<5> buttons(inputs);
Terminal terminal;
ScreenManager screenManager;

ThermostatController thermostat;

void setup() {

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

	/*WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
		log_i("WiFi event: %d, status: %d", event, WiFi.status());
	});*/

	WiFiConfig wifi;
	if(loadWifiConfig(wifi)) {

		lcd.clear();
		lcd.home();
		lcd.print("WiFi connecting ");
		lcd.setCursor(0, 1);

		if(!connectWiFi(wifi, true)){
			log_e("Failed to connect to saved WiFi network.");
			WiFi.disconnect(true);
			lcd.clear();
			lcd.home();
			lcd.print("   Connection   ");
			lcd.setCursor(0, 1);
			lcd.print("   failed.      ");

			vTaskDelay(1000 / portTICK_PERIOD_MS);
			WiFiConfig* params = new WiFiConfig();
			memcpy(params, &wifi, sizeof(wifi));
			xTaskCreate(
				taskReconnectWifi,
				"Reconnect WiFi",
				3000,
				params,
				5,
				nullptr
			);
		}
		else {
			log_i("WiFi connected");
			log_i("IP address: %s", WiFi.localIP().toString().c_str());
			lcd.home();
			lcd.print("   Connection   ");
			lcd.setCursor(0, 1);
			lcd.print("   successful.  ");
		}
	}
	else{

		// Serial.println("WiFi is not setup yet. Creating default AP...");
		// if(!WiFi.softAP(SETUP_WIFI_SSID, SETUP_WIFI_PASS)) {
		// 	log_e("Soft AP creation failed.");
		// 	//while(1);
		// }
		// WiFi.softAPConfig(
		// 	IPAddress(192, 168, 1, 1),
		// 	IPAddress(192, 168, 1, 1),
		// 	IPAddress(255, 255, 255, 0)
		// );
		// IPAddress myIP = WiFi.softAPIP();
		// Serial.print("AP IP address: ");
		// Serial.println(myIP);
		wifi.version = 1;
		wifi.ip = 0;
		strcpy(wifi.ssid, "<ssid>");
		strcpy(wifi.password, "<password>");

		File cfg = LittleFS.open(WIFI_CONFIG_FILE, FILE_WRITE, true);
		if(cfg) {
			cfg.write((byte*)&wifi, sizeof(wifi));
			cfg.close();
		}
	}

	server.addMiddleware(&cors);

	server.onNotFound([](AsyncWebServerRequest* request) {
		request->send(404, "text/plain", "404 Not found");
	});

	//client app
	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		if(!LittleFS.exists("/client/index.html.gz")) {
			request->send(500, "text/plain", "Error - client code has not been uploaded. Please flash the filesystem image.");
			return;
		}
		AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/client/index.html.gz", "text/html");
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});

	setupApiEndpoints();

	server.begin();

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	screenManager.push(new HomeScreen());

	log_i("Initialisation complete.");
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
