#pragma once

#include "screens/Screen.h"

class SetModeScreen : public Screen {
  public:

	int cursorPos = 0;
	const int cursorPositions = 4;

	SetModeScreen() {
	}

	virtual void onUp();
	virtual void onDown();

	virtual void onMode();
	// virtual void onModeHold();

	virtual void render();

	// virtual void setCurrentScreen();
};
