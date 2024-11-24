#pragma once

//includes
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
//#include <STM32RTC.h>
#include <ESP32Time.h>
// #include <DS18B20.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <deque>

#include <ButtonLib.h>


//defines
//#define HOLD_TIME 500
#define MIN_TEMP 50
#define MAX_TEMP 350
#define TEMP_COEF 10
#define TEMP(temp) (int32_t(temp * TEMP_COEF))
#define TIME_NOT_SET -1

#define BTN_MODE_PIN 32//PA4 //mode
#define BTN_DOWN_PIN 33//PA5 //down
#define BTN_UP_PIN   25//PA6 //up
//#define BTN4_PIN PA7

#define LED_PIN 2//PC13

#define TEMP_SENSOR_PIN 26//PA0

#define GLOBAL extern

//typedefs
typedef uint32_t Time;
typedef uint8_t Day;

//class forward declarations
class Screen;

//enums
enum Modes {
	MODE_OFF,
	MODE_LOW,
	MODE_HIGH,
	MODE_PROGRAM
};

enum Days {
	DAY_MONDAY,
	DAY_TUESDAY,
	DAY_WEDNESDAY,
	DAY_THURSDAY,
	DAY_FRIDAY,
	DAY_SATURDAY,
	DAY_SUNDAY,
	DAY_ALL
};

enum BtnInputs {
	BTN_MODE,
	BTN_DOWN,
	BTN_UP
};

struct Slot {
	Time startTime = TIME_NOT_SET;
	Time endTime = TIME_NOT_SET;
	// uint32_t temperature;

	inline bool operator==(const Slot right) const {
		return startTime == right.startTime && endTime == right.endTime/* && right.temperature == temperature*/;
	}

	inline bool operator!=(const Slot right) const {
		return !operator==(right);
	}

	inline bool isSet() const {
		return startTime != TIME_NOT_SET && endTime != TIME_NOT_SET;
	}
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

//function prototypes
uint32_t getTemperature();
void initADC();
void checkTemperature();
void turnOn();
void turnOff();
// void setRegulation(uint32_t reg);
int32_t* getCurrentModeTemp();
Time getTimeOfDay();
Slot* isSlotActive(const Time time = getTimeOfDay());
bool sweep(Time time);

//global variables
GLOBAL LiquidCrystal_I2C lcd;
// GLOBAL HardwareSerial Serial3;
GLOBAL hw_timer_t* tim1;
GLOBAL ESP32Time rtc;
// GLOBAL DS18B20 temp;
GLOBAL OneWire oneWire;
GLOBAL DallasTemperature temp;
GLOBAL ArduinoButtonReader<3> buttons;

GLOBAL Screen* currentScreen;
GLOBAL Modes mode;

GLOBAL Slot slots[7][8];

GLOBAL int32_t lowTemperature;
GLOBAL int32_t highTemperature;
GLOBAL int32_t currentTemperature;

//GLOBAL Day currentDay;

GLOBAL uint32_t hysteresis;

#include "screens.h"


