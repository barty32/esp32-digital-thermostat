#pragma once
//#include <Arduino.h>
//#include <LiquidCrystal_I2C.h>
#include "main.h"


// class Screen;
// extern Screen* currentScreen;
// extern uint32_t hysteresis;

// extern LiquidCrystal_I2C lcd;

class Screen {
  public:
	bool autoRender = false;
	Screen* prevScreen = nullptr;

	virtual void onIncrease() = 0;
	virtual void onDecrease() = 0;
	//virtual void onPlusMinus() {};

	virtual void onModePress() = 0;
	virtual void onModeHold() = 0;

	virtual void render() = 0;

	void setCurrentScreen() {
		currentScreen = this;
		lcd.clear();
		this->render();
	}
};

class HomeScreen : public Screen {
  public:

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class SetupScreen : public Screen {

  public:
	int cursorPos = 0;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class SystemTimeScreen : public Screen {

  public:
	int cursorPos = 0;
	uint32_t hours = rtc.getHours();
	uint32_t minutes = rtc.getMinutes();
	Day day = rtc.getWeekDay() - 1;
	bool format24h = true;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class HysteresisScreen : public Screen {

  public:
	int cursorPos = 0;
	uint32_t setHysteresis = hysteresis;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class DaySelectScreen : public Screen {

  public:
	int cursorPos = 0;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class SlotSelectScreen : public Screen {

  public:
	int cursorPos = 0;
	Day day = 0;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};


class TimeSelectScreen : public Screen {

  public:
	int cursorPos = 0;
	Slot& slot;
	uint32_t startHours = -1;
	uint32_t startMinutes = -1;
	uint32_t endHours = -1;
	uint32_t endMinutes = -1;

	TimeSelectScreen(Slot& slot): slot(slot) {
		if(slot.startTime != -1) {
			startHours = slot.startTime / 3600;
			startMinutes = (slot.startTime - 3600 * startHours) / 60;
		}
		if(slot.endTime != -1) {
			endHours = slot.endTime / 3600;
			endMinutes = (slot.endTime - 3600 * endHours) / 60;
		}
	}

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	//virtual void onPlusMinus();

	virtual void render();

	//virtual void setCurrentScreen();
};

class FactoryResetScreen : public Screen {

  public:
	bool cursorPos = false;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class TemperatureSetScreen : public Screen {

  public:
	int cursorPos = 0;
	uint32_t dayTemp = dayTemperature;
	uint32_t nightTemp = nightTemperature;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};