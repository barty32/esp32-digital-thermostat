#include "main.h"
#include "ScreenManager.h"
#include "screens/HomeScreen.h"
#include "ThermostatController.h"

LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);
I2C_eeprom eeprom(EEPROM_I2C_ADDR, EEPROM_I2C_SIZE);
//extEEPROM eeprom(kbits_4, 1, 16, 0x50);
hw_timer_t *tim1 = NULL;
ESP32Time rtc;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp(&oneWire);
AsyncWebServer server(SERVER_PORT);
CorsMiddleware cors;

//these have to be in the same order as ScreenManager::Button
static const uint8_t inputs[] = {BTN_UP_PIN, BTN_DOWN_PIN, BTN_LEFT_PIN, BTN_RIGHT_PIN, BTN_MODE_PIN};
ArduinoButtonReader<5> buttons(inputs);
ScreenManager screenManager;

ThermostatController thermostat;

void setup() {

	Serial.begin(SERIAL_BAUD);
	Serial.println("============================");
	Serial.println("  ESP32 Digital Thermostat  ");
	Serial.println("============================");

	for(auto &input : inputs) {
		pinMode(input, INPUT_PULLUP);
	}
    pinMode(LED_BUILTIN_PIN, OUTPUT);
	pinMode(TEMP_SENSOR_PIN, INPUT_PULLUP);
	pinMode(ANALOG_CTRL_PIN, OUTPUT);

	Serial.println("Initialising temperature sensor...");
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
	else{
		Serial.println("Unable to find address for temperature sensor.");
	}

	temp.setResolution(12);
	temp.setWaitForConversion(false);

	Serial.println("Initialising RTC...");
	// rtc.setClockSource(STM32RTC::LSE_CLOCK);
	// rtc.begin();
	// if(!rtc.isConfigured()) {
	// 	rtc.setDate(1, 1, 10);
	// 	rtc.setTime(0, 0, 0);
	// }
	//TODO: sync time with NTP
	rtc.setTime(0, 12, 10, 17, 10, 2023);
	Serial.println("Current time: " + rtc.getDateTime());

	Serial.println("Initialising button handlers...");
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
	tim1 = timerBegin(0, 80, true);
	timerAttachInterrupt(tim1, [](){buttons.readInputs_isr();}, true);
	timerAlarmWrite(tim1, 1000, true);
	timerAlarmEnable(tim1);


	Serial.println("Initialising LCD...");
	screenManager.init();
	lcd.setCursor(0, 0);
	lcd.print("ESP32 Thermostat");
	lcd.setCursor(0, 1);
	lcd.print("  ver: 1.0      ");

	// byte error, address;
	// int nDevices;

	// Serial.println("Scanning...");

	// nDevices = 0;
	// for(address = 1; address < 127; address++) {
	// 	// The i2c_scanner uses the return value of
	// 	// the Write.endTransmisstion to see if
	// 	// a device did acknowledge to the address.
	// 	Wire.beginTransmission(address);
	// 	error = Wire.endTransmission();

	// 	if(error == 0) {
	// 		Serial.print("I2C device found at address 0x");
	// 		if(address < 16)
	// 			Serial.print("0");
	// 		Serial.print(address, HEX);
	// 		Serial.println("  !");

	// 		nDevices++;
	// 	}
	// 	else if(error == 4) {
	// 		Serial.print("Unknown error at address 0x");
	// 		if(address < 16)
	// 			Serial.print("0");
	// 		Serial.println(address, HEX);
	// 	}
	// }
	// if(nDevices == 0)
	// 	Serial.println("No I2C devices found\n");
	// else
	// 	Serial.println("done\n");

	Serial.println("Initialising EEPROM...");
	eeprom.begin();

	ThermostatController::PersistentConfig config;
	eeprom.readBlock(0, (byte*)&config, sizeof(config));
	if(thermostat.loadConfig(config)) {
		Serial.println("Successfully loaded configuration from EEPROM.");
	}

	// for(int i = 0; i < 512; i += 16) {
	// 	byte zero[] = {
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 		0x54,
	// 	};
	// 	//byte s = eeprom.write(i, zero, 16);
	// 	//eeprom.write(i, i);
	// 	eeprom.writeBlock(i, zero, 16);
	// }

	// for(int i = 0; i < 512; i++) {
	// 	// byte b = eeprom.read(i);
	// 	byte b = eeprom.readByte(i);
	// 	if(b < 0x10) {
	// 		Serial.print('0');
	// 	}
	// 	Serial.print(b, HEX);
	// 	Serial.print(' ');
	// 	if(i % 16 == 15) {
	// 		Serial.println();
	// 	}
	// }

	xTaskCreate(
		taskReadTemperature,
		"Read temperature",
		1000,
		nullptr,
		1,
		nullptr
	);

	xTaskCreate(
		taskTickThermostatLogic,
		"Tick thermostat logic",
		1000,
		nullptr,
		1,
		nullptr
	);

	log_i("Setting up WiFi...");
	WiFi.setHostname(HOSTNAME);
	// if(!wifi.isSetup()) {
	// 	Serial.println("WiFi is not setup yet. Creating default AP...");
	// 	if(!WiFi.softAP(SETUP_WIFI_SSID, SETUP_WIFI_PASS)) {
	// 		log_e("Soft AP creation failed.");
	// 		//while(1);
	// 	}
	// 	WiFi.softAPConfig(
	// 		IPAddress(192, 168, 1, 1),
	// 		IPAddress(192, 168, 1, 1),
	// 		IPAddress(255, 255, 255, 0)
	// 	);
	// 	IPAddress myIP = WiFi.softAPIP();
	// 	Serial.print("AP IP address: ");
	// 	Serial.println(myIP);
	// }
	//WiFi.mode(WIFI_STA);
	
	WiFi.disconnect(true);

	WiFi.begin("WIFI_SSID", "WIFI_PASSWORD");
	while(WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	server.addMiddleware(&cors);

	server.onNotFound([](AsyncWebServerRequest *request) {
		request->send(404, "text/plain", "404 Not found");
	});

	setupWebServer();

	server.begin();

	vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	screenManager.push(new HomeScreen());

	Serial.println("Initialisation complete.");
}

//extern const char* dayNames[];

void loop() {

	buttons.executeHandlers();
	screenManager.update();
	//thermostat.update();

	if(Serial.available()) {
		String received = Serial.readString();
		received.trim();
		const int colon = received.indexOf(':');
		String command = colon == -1 ? received : received.substring(0, colon);
		String payload = colon == -1 ? "" : received.substring(colon + 1);
		if(command == "sweep") {
			/*for(int i = 0; i < 1440; ++i) {
				Serial.println(sweep(i * 60) ? "1," : "0,");
			}*/
		}
		else if(command == "dump") {
			/*String line1 = "+------+";
			String line2 = "| Slot |";
			String line3 = "+------+";
			for(int i = 0; i < 8; i++) {
				line1 += "-------+";
				line2 += "   " + String(i) + "   |";
				line3 += "-------+";
			}
			Serial.println(line1);
			Serial.println(line2);
			Serial.println(line3);

			int i = 0;
			for(auto &day : slots) {
				String line1 = "| " + String(dayNames[i]) + "  |";
				String line2 = "|      |";
				String line3 = "|------+";
				for(auto &slot : day) {
					line1 += " " + printTime(slot.startTime) + " |";
					line2 += " " + printTime(slot.endTime) + " |";
					line3 += "-------+";
				}
				Serial.println(line1);
				Serial.println(line2);
				Serial.println(line3);
				i++;
			}*/
		}
		else if(command == "setTemp"){
			// const int temp = int(payload.toFloat() * 10.0);
			// currentTemperature = temp;
			// Serial.println("Set current temperature to: " + String(temp / 10.0));
		}
		//+------+-------+-------+---
		//| Slot |   1   |   2   |
		//|------+-------+-------+---
		//| Mon  | --:-- |  5:30 |
		//|      | --:-- |  6:30 |
		//|------+-------+-------+---
		//
		//
		//
        // long temp = received.toInt();
        // if (temp == 0) {
        //     //vypise vsechny zmeny
        //     for (auto& change : changes) {
		// 		printSerial(change, false);
		// 	}
		// }
        // else if (temp > 250 || temp < 150) {
		// 	//Serial3.println("mimo rozsah!");
        //     printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
        // } else {
        //     temp -= temp % 5;//zaokrouhleni
		// 	setTemperature = temp;
        //     printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
		// }
	}

	// static uint32_t lastPrint = 0;
	// if(millis() - lastPrint > 1000) {

	// 	temp.requestTemperatures();
	// 	float t = temp.getTempCByIndex(0);
	// 	thermostat.sendTemperatureUpdate(t);
	// 	Serial.println("Got temperature: " + String(t));

	// 	// if(currentScreen->autoRender){
	// 		// currentScreen->render();
	// 	// }

	// 	lastPrint = millis();
	// }

	delay(10);
}

void taskReadTemperature(void* pvParameters) {
	while(true) {
		temp.requestTemperatures();
		float t = temp.getTempCByIndex(0);
		//thermostat.sendTemperatureUpdate(t);
		//Serial.println("Got temperature: " + String(t));
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void taskTickThermostatLogic(void* pvParameters) {
	while(true) {
		thermostat.update();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
