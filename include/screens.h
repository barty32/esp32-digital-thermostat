#pragma once
//#include <Arduino.h>
//#include <LiquidCrystal_I2C.h>
#include "main.h"


// class Screen;
// extern Screen* currentScreen;
// extern uint32_t hysteresis;

// extern LiquidCrystal_I2C lcd;

GLOBAL const char* dayNames[];
void printTime(uint32_t hours, uint32_t minutes, bool colon = true);

class Screen {
  private:
	Screen* prevScreen = nullptr;

  public:
	// bool autoRender = false;
	Screen(Screen* currentScreen):
	  prevScreen(currentScreen)
	{
	}

	//button input handlers
	virtual void onIncrease() {};
	virtual void onDecrease() {};
	//virtual void onPlusMinus() {};

	virtual void onModePress() {};
	virtual void onModeHold(){
		this->exit();
	}

	virtual void render() = 0;

	virtual void show(){
		currentScreen = this;
		lcd.clear();
		this->render();
	}

	virtual void exit() {
		if(prevScreen){
			prevScreen->show();
			delete this;
		}
		//error?
	}

	// virtual void setCurrentScreen(int retval = 0) {
	// 	currentScreen = this;
	// 	lcd.clear();
	// 	this->render();
	// }
};

class ScrollableListScreen : public Screen {

	int cursorPos = 0;
	std::vector<const char*>& items;

	public:

	ScrollableListScreen(Screen* currentScreen, std::vector<const char*>& items) :
		Screen(currentScreen),
		items(items)
	{
	}

	virtual void onIncrease() {
		cursorPos--;
		if(cursorPos < 0) {
			cursorPos = items.size() - 1;
		}
		this->render();
	}

	virtual void onDecrease() {
		cursorPos++;
		if(cursorPos > items.size() - 1) {
			cursorPos = 0;
		}
		this->render();
	}

	virtual void onModePress(){
		this->showScreen(cursorPos);
	}

	virtual void onModeHold() {
		this->exit();
	}

	virtual void showScreen(int cursor) = 0;

	virtual void render() {
		lcd.clear();
		lcd.noCursor();
		if(cursorPos % 2) {
			lcd.setCursor(1, 0);
			lcd.print(items[cursorPos - 1]);
			lcd.setCursor(0, 1);
			lcd.print(">");
			lcd.print(items[cursorPos]);
		}
		else {
			lcd.setCursor(0, 0);
			lcd.print(">");
			lcd.print(items[cursorPos]);
			lcd.setCursor(1, 1);
			if(cursorPos < items.size() - 1) {
				lcd.print(items[cursorPos + 1]);
			}
		}
	}
};

class HomeScreen : public Screen {
  public:

	HomeScreen(Screen* currentScreen) :
	  Screen(currentScreen) {
	}

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};

class MenuScreen : public ScrollableListScreen {
  public:

	enum MenuOptions {
		MENU_SET_MODE,
		MENU_SET_TEMPERATURE,
		MENU_SET_PROGRAM,
		MENU_SCHEDULE,
		MENU_SETTINGS,
		MENU_EXIT
	};

	std::vector<const char*> menuItems = {
		"Set mode",
		"Set temperature",
		"Set program",
		"Schedule",
		"Settings",
		"Exit"
	};

	MenuScreen(Screen* currentScreen) :
	  ScrollableListScreen(currentScreen, menuItems) {
	}

	virtual void showScreen(int cursor);
};

class SettingsScreen : public ScrollableListScreen {
  public:

	enum Settings {
		OPTION_DATETIME,
		OPTION_NETWORK,
		OPTION_PROPERTIES,
		OPTION_STATISTICS,
		OPTION_FACTORY_RESET,
		OPTION_TEST,
		OPTION_EXIT
	};

	std::vector<const char*> options = {
		"Date and time",
		"Network",
		"Adjust props",
		"Statistics",
		"Factory reset",
		"test",
		"Back"
	};

	SettingsScreen(Screen* currentScreen) :
	  ScrollableListScreen(currentScreen, options) {
	}

	virtual void showScreen(int cursor);
};

class SetModeScreen : public Screen {
  public:

	int cursorPos = 0;
	const int cursorPositions = 4;

	SetModeScreen(Screen* currentScreen) :
	  Screen(currentScreen) {
	}

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	// virtual void onModeHold();

	virtual void render();

	// virtual void setCurrentScreen();
};

class SetTemperatureScreen : public Screen {
  public:

	int cursorPos = 0;
	uint32_t highTemp = highTemperature;
	uint32_t lowTemp = lowTemperature;

	SetTemperatureScreen(Screen* currentScreen) :
	  Screen(currentScreen) {
	}

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	// virtual void onModeHold();

	virtual void render();

	// virtual void setCurrentScreen();
};

class ProgramScreen : public ScrollableListScreen {
  public:

	enum Settings {
		OPTION_DATETIME,
		OPTION_NETWORK,
		OPTION_PROPERTIES,
		OPTION_STATISTICS,
		OPTION_FACTORY_RESET,
		OPTION_TEST,
		OPTION_EXIT
	};

	std::vector<const char*> options = {
		"Date and time",
		"Network",
		"Adjust props",
		"Statistics",
		"Factory reset",
		"test",
		"Back"};

	ProgramScreen(Screen* currentScreen) :
	  ScrollableListScreen(currentScreen, options) {
	}

	virtual void showScreen(int cursor);
};

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