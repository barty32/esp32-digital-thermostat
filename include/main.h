#pragma once

#include "config.h"

//libraries
#include <Arduino.h>
#include <WiFi.h>
//#include <I2C_eeprom.h>
//#include <extEEPROM.h>
#include <LittleFS.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Time.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <Preferences.h>
// #include <AsyncJson.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>

//local
#include <ButtonLib.h>
#include <Terminal.h>
#include <ExpressESP.h>

#include "WifiConfig.h"

#define LCD_SYMBOL_DEGREE (char)223

//class forward declarations
class Day;
class Time;
class TimeSlot;
class Screen;
class ScreenManager;
class ThermostatController;

enum Status {
	STATUS_LOADING,
	STATUS_NOT_SETUP,
	STATUS_OFFLINE,
	STATUS_READY,
};

struct Temperature {

	enum Unit {
		UNIT_CELSIUS,
		UNIT_FAHRENHEIT
	};

	static constexpr float NOT_SET = -128.0f;
	static constexpr float DISCONNECTED = DEVICE_DISCONNECTED_C;

	float temperature = NOT_SET;

	Temperature() {}

	Temperature(float temperature) :
	  temperature(temperature) {
	}
	

	inline bool isSet() const { return temperature != NOT_SET; }

	String toString(int decimalPlaces = 2, Unit unit = UNIT_CELSIUS) const {
		if(temperature == NOT_SET) {
			String t = "--.";
			while(decimalPlaces--) t += '-';
			return t;
		}
		return String(unit == UNIT_FAHRENHEIT ? temperature * 9.0 / 5.0 + 32.0 : temperature, decimalPlaces);
	}

	Temperature operator+(const Temperature &right) const { return Temperature(temperature + right.temperature); }
	Temperature operator-(const Temperature &right) const { return Temperature(temperature - right.temperature); }
	//Temperature operator*(const Temperature &right) const { return Temperature(temperature * right.temperature); }
	//Temperature operator/(const Temperature &right) const { return Temperature(temperature / right.temperature); }

	friend bool operator==(const Temperature &left, const Temperature &right) { return left.temperature == right.temperature; }
	friend bool operator!=(const Temperature &left, const Temperature &right) { return !operator==(left, right); }
	friend bool operator<(const Temperature &left, const Temperature &right) { return left.temperature < right.temperature; }
	friend bool operator>(const Temperature &left, const Temperature &right) { return operator<(right, left); }
	friend bool operator<=(const Temperature &left, const Temperature &right) { return !operator>(left, right); }
	friend bool operator>=(const Temperature &left, const Temperature &right) { return !operator<(left, right); }
};

template <uint8_t num_inputs = 1>
class ArduinoButtonReader : public ButtonReader<num_inputs> {

  public:
	ArduinoButtonReader(const uint8_t inputPins[num_inputs]) :
	  ButtonReader<num_inputs>(inputPins) {
	}

	uint32_t getSystemTime() override {
		return millis();
	}

	bool readPinState(uint8_t pin) override {
		return digitalRead(pin);
	}
};

//tasks
void taskReadTemperature(void* pvParameters);
void taskLogTemperature(void* pvParameters);
void taskTickThermostatLogic(void* pvParameters);
void taskWifi(void* pvParameters);
void taskHTTP(void* pvParameters);

BaseType_t startReadTemperatureTask();
BaseType_t startLogTemperatureTask();
BaseType_t startTickThermostatLogicTask();
BaseType_t startWifiTask();
BaseType_t startHTTPTask();

//utils
bool loadThermostatConfig();
bool saveThermostatConfig();
// bool loadWifiConfig(WiFiConfig &config);
// bool connectWiFi(WiFiConfig &wifi, bool reportToLcd = false);
void setupApiEndpoints();
bool connectWiFi(const WiFiConfig &wifi);
bool createSetupAP();
bool stopAP();

void logCurrentTemperature();
void handleTerminalCommand(Terminal& terminal, const String &command, const String &params);

//global interfaces
extern LiquidCrystal_I2C lcd;
//extern I2C_eeprom eeprom;
//extern extEEPROM eeprom;
extern hw_timer_t* tim1;
extern ESP32Time rtc;
extern OneWire oneWire;
extern DallasTemperature temp;
extern AsyncWebServer server;
extern ArduinoButtonReader<5> buttons;
extern ScreenManager screenManager;

extern ThermostatController thermostat;
