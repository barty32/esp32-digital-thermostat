#pragma once

#include "main.h"


void printTime(uint32_t hours, uint32_t minutes, bool colon = true);

/*
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
	uint32_t hours = 10; //rtc.getHours();
	uint32_t minutes = 15;//rtc.getMinutes();
	Day day = 3;//rtc.getWeekDay() - 1;
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
	Slot allDaySlot;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	virtual void setCurrentScreen(int retval = 0);

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
	uint32_t highTemp = highTemperature;
	uint32_t lowTemp = lowTemperature;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class RegulationScreen : public Screen {

  public:
	uint32_t setRegulation = 0;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};*/