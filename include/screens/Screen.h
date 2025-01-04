#pragma once

#include "main.h"


class Screen {

	friend class ScreenManager;

  protected:
	ScreenManager *manager;

  public:
	// bool autoRender = false;
	Screen() {
	  //manager(manager) {
	}

	virtual ~Screen() {}

	//button input handlers
	virtual void onUp() {}
	virtual void onDown() {}

	virtual void onRight() {}
	virtual void onLeft() {}

	virtual void onMode() {}
	virtual void onModeHold() {
		this->exit();
	}

	virtual void update() {}

	virtual void render() = 0;

	// virtual void show() {
	// 	currentScreen = this;
	// 	lcd.clear();
	// 	this->render();
	// }

	virtual void exit();

	// virtual void setCurrentScreen(int retval = 0) {
	// 	currentScreen = this;
	// 	lcd.clear();
	// 	this->render();
	// }
};