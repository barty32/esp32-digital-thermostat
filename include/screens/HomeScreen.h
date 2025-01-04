#pragma once

#include "screens/Screen.h"

class HomeScreen : public Screen {
  public:

	HomeScreen() {
	}

	void onUp() override;
	void onDown() override;

	void onRight() override;
	
	void onMode() override;
	void onModeHold() override;

	void update() override;
	void render() override;

	//virtual void setCurrentScreen();
};
