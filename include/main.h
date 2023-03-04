#pragma once

//includes
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <deque>

#include "screens.h"


//defines
#define HOLD_TIME 500

#define BTN1_PIN PA4 //mode
#define BTN2_PIN PA5 //down
#define BTN3_PIN PA6 //up
#define BTN4_PIN PA7

#define LED_PIN PC13

#define TEMP_SENSOR_PIN PA0

#define GLOBAL extern




//enums
enum Modes {
	MODE_OFF,
	MODE_DAY,
	MODE_NIGHT,
	MODE_TIMES
};

//functions
void checkButtons();
uint32_t getTemperature();

//global variables
GLOBAL LiquidCrystal_I2C lcd;
GLOBAL HardwareSerial Serial3;
GLOBAL HardwareTimer tim1;

GLOBAL Screen* currentScreen;
GLOBAL Modes mode;

GLOBAL uint32_t dayTemperature;
GLOBAL uint32_t nightTemperature;
GLOBAL uint16_t currentTemperature;
