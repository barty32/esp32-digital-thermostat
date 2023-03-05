#pragma once

//includes
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <deque>


//defines
#define HOLD_TIME 500

#define BTN1_PIN PA4 //mode
#define BTN2_PIN PA5 //down
#define BTN3_PIN PA6 //up
#define BTN4_PIN PA7

#define LED_PIN PC13

#define TEMP_SENSOR_PIN PA0

#define GLOBAL extern

typedef uint32_t Time;
typedef uint8_t Day;

//enums
enum Modes {
	MODE_OFF,
	MODE_DAY,
	MODE_NIGHT,
	MODE_TIMES
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

struct Slot {
	Time startTime;
	Time endTime;
	uint32_t temperature;
};

class Screen;

//functions
void checkButtons();
uint32_t getTemperature();

//global variables
GLOBAL LiquidCrystal_I2C lcd;
GLOBAL HardwareSerial Serial3;
GLOBAL HardwareTimer tim1;

GLOBAL Screen* currentScreen;
GLOBAL Modes mode;

GLOBAL Slot slots[7][8];

GLOBAL uint32_t dayTemperature;
GLOBAL uint32_t nightTemperature;
GLOBAL uint16_t currentTemperature;

GLOBAL uint32_t hysteresis;

#include "screens.h"
