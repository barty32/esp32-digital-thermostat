#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


class Screen;
extern Screen* currentScreen;

extern LiquidCrystal_I2C lcd;

class Screen {
  public:
	bool autoRender = false;
	Screen* prevScreen = nullptr;

	virtual void onIncrease() = 0;
	virtual void onDecrease() = 0;

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
	uint32_t hours = 16;//getHours()
	uint32_t minutes = 35;//getMinutes()
	bool format24h = true;

	virtual void onIncrease();
	virtual void onDecrease();

	virtual void onModePress();
	virtual void onModeHold();

	virtual void render();

	//virtual void setCurrentScreen();
};