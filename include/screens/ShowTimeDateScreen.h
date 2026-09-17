#pragma once

#include "screens/Screen.h"

class ShowTimeDateScreen : public Screen {
  public:

	ShowTimeDateScreen();

	void onRight() override;
	void onLeft() override;

	void onMode() override;

	void update() override;
	void render() override;
};
