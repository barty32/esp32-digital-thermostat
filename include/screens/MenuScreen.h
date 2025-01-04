#pragma once

#include "ScrollableListScreen.h"

class MenuScreen : public ScrollableListScreen {
  public:

	enum MenuOptions {
		MENU_MODE,
		MENU_TEMPERATURE,
		MENU_TIME_SLOTS,
		MENU_SCHEDULE,
		MENU_SETTINGS,
		MENU_EXIT
	};

	std::vector<const char*> menuItems = {
		"Mode",
		"Temperature",
		"Time slots",
		"Schedule",
		"Settings",
		"Exit"
	};

	MenuScreen() :
	  ScrollableListScreen(menuItems) {
	}

	void onItemSelected(int cursor) override;

	void exit() override;
};
