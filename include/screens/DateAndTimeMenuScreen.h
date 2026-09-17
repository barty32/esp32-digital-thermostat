#pragma once

#include "ScrollableListScreen.h"

class DateAndTimeMenuScreen : public ScrollableListScreen {
  public:

	enum Settings {
		OPTION_CURRENT,
		OPTION_ADJUST,
		OPTION_TIMEZONE,
		OPTION_SYNC_NETWORK,
		OPTION_EXIT
	};

	const std::vector<const char*> options = {
		"View current",
		"Adjust manually",
		"Set timezone",
		"Sync with NTP",
		"Back"
	};

	DateAndTimeMenuScreen() :
	  ScrollableListScreen(options) {
	}

	void onItemSelected(int cursor) override;
};
