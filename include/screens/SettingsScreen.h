#pragma once

#include "ScrollableListScreen.h"

class SettingsScreen : public ScrollableListScreen {
  public:

	enum Settings {
		OPTION_DATETIME,
		OPTION_NETWORK,
		OPTION_PROPERTIES,
		OPTION_STATISTICS,
		OPTION_FACTORY_RESET,
		OPTION_EXIT
	};

	const std::vector<const char*> options = {
		"Date and time",
		"Network",
		"Adjust props",
		"Statistics",
		"Factory reset",
		"Back"
	};

	SettingsScreen() :
	  ScrollableListScreen(options) {
	}

	void onItemSelected(int cursor) override;
};
