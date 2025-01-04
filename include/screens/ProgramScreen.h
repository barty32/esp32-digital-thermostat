#pragma once

#include "ScrollableListScreen.h"

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
		"Back"
	};

	ProgramScreen(Screen* currentScreen) :
	  ScrollableListScreen(currentScreen, options) {
	}

	virtual void showScreen(int cursor);
};
