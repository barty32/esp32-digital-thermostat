#include "main.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
hw_timer_t *tim1 = NULL;
ESP32Time rtc;
// DS18B20 temp(TEMP_SENSOR_PIN);
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature temp(&oneWire);

static const uint8_t inputs[] = {BTN_MODE_PIN, BTN_DOWN_PIN, BTN_UP_PIN};
ArduinoButtonReader<3> buttons(inputs);

Screen* currentScreen = nullptr;
Modes mode = MODE_OFF;

//temperatures, times 10
int32_t highTemperature = 210;
int32_t lowTemperature = 180;
int32_t currentTemperature = 170;

//Day currentDay = DAY_MONDAY;

Slot slots[7][8]; //8 slots per day

byte customChar1[8] = {
	0b10000,
	0b10100,
	0b10110,
	0b11111,
	0b00111,
	0b00110,
	0b00100,
	0b00000
};

void setup() {

	currentScreen = new HomeScreen(nullptr);
	// currentScreen->autoRender = true;

	Serial.begin(115200);
	Serial.println("============================");
	Serial.println("  ESP32 Digital Thermostat  ");
	Serial.println("============================");

	for(auto &input : inputs) {
		pinMode(input, INPUT_PULLUP);
	}
    pinMode(LED_PIN, OUTPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT_PULLUP);

	//initADC();

	Serial.println("Initialising temperature sensor...");
	temp.begin();
	DeviceAddress addr;
	if(temp.getAddress(addr, 0)) {
		Serial.print("Found sensor with address 0x");
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
	rtc.setTime(0, 12, 10, 17, 10, 2023);
	Serial.println("Current time: " + rtc.getDateTime());

	Serial.println("Initialising button handlers...");
	buttons.setDetectLongPress(BTN_MODE, true);
	buttons.setInputRepeat(BTN_DOWN, true);
	buttons.setInputRepeat(BTN_UP, true);

	buttons.setOnPressListener([](uint8_t index, bool repeat) {
		switch(index) {
			case BTN_MODE:
				currentScreen->onModePress();
				break;
			case BTN_DOWN:
				currentScreen->onDecrease();
				break;
			case BTN_UP:
				currentScreen->onIncrease();
				break;
		}
	});

	buttons.setOnLongPressListener([](uint8_t index) {
		switch(index) {
			case BTN_MODE:
				currentScreen->onModeHold();
				break;
		}
	});

	//run isr at 1000Hz
	tim1 = timerBegin(0, 80, true);
	timerAttachInterrupt(tim1, [](){buttons.readInputs_isr();}, true);
	timerAlarmWrite(tim1, 1000, true);
	timerAlarmEnable(tim1);


	Serial.println("Initialising LCD...");
	lcd.init();
	lcd.backlight();
	lcd.createChar(0, customChar1);
	lcd.setCursor(0, 0);
	lcd.print("ESP32 Thermostat");
	lcd.setCursor(0, 1);
	lcd.print("  ver: 1.0      ");
	delay(1000);

	// for(auto &day : slots) {
	// 	for(auto &slot : day) {
	// 		slot.startTime = -1;
	// 		slot.endTime = -1;
	// 		slot.temperature = 0;
	// 	}
	// }


	Serial.println("Initialisation complete.");
}

String printTime(Time time){
	if(time == TIME_NOT_SET) return "--:--";
	String result = "";
	int hours = time / 3600;
	int minutes = (time % 3600) / 60;
	if(hours < 10) {
		result += ' ';
	}
	if(hours > 23) {
		result += "--";
	}
	else {
		result += hours;
	}
	result += ':';
	if(minutes < 10) {
		result += '0';
	}
	if(minutes > 59) {
		result += "--";
	}
	else {
		result += minutes;
	}
	return result;
}

extern const char* dayNames[];

void loop() {

	buttons.executeHandlers();

	if(Serial.available()) {
		String received = Serial.readString();
		received.trim();
		const int colon = received.indexOf(':');
		String command = colon == -1 ? received : received.substring(0, colon);
		String payload = colon == -1 ? "" : received.substring(colon + 1);
		if(command == "sweep") {
			for(int i = 0; i < 1440; ++i) {
				Serial.println(sweep(i * 60) ? "1," : "0,");
			}
		}
		else if(command == "dump") {
			String line1 = "+------+";
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
			}
		}
		else if(command == "setTemp"){
			const int temp = int(payload.toFloat() * 10.0);
			currentTemperature = temp;
			Serial.println("Set current temperature to: " + String(temp / 10.0));
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

	static uint32_t lastPrint = 0;
	if(millis() - lastPrint > 500) {

		//cteni potenciometru/termistoru
		temp.requestTemperatures();
		//Serial.println( temp.getTempCByIndex(0));//getTemperature()
		// DeviceAddress deviceAddress;
		// if (!temp.getAddress(deviceAddress, 0)) {
		// 	// return DEVICE_DISCONNECTED_C;
		// }
		//currentTemperature = temp.getTemp(sensorAddress) * 0.078125f;//temp / 12.8
		currentTemperature = temp.getTempCByIndex(0) * 10.0f;
		Serial.println("Got temperature: " + String(currentTemperature / 10.0));

		// if(currentScreen->autoRender){
			// currentScreen->render();
		// }

		checkTemperature();

		lastPrint = millis();
	}

	delay(10);
}
