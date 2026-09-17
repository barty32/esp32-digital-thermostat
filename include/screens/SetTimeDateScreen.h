#pragma once

#include "screens/Screen.h"
#include "Time.h"

class SetTimeDateScreen : public Screen {
  public:

	enum CursorPosition : uint8_t {
		CURSOR_DAY,
		CURSOR_MONTH,
		CURSOR_YEAR,
		CURSOR_HOUR,
		CURSOR_MINUTE,
		CURSOR_SECOND
	};

	uint8_t cursorPos = CURSOR_DAY;
	int slotIndex = 0;

	Time time;

	SetTimeDateScreen();

	void onUp() override;
	void onDown() override;
	void onRight() override;
	void onLeft() override;

	void onMode() override;
	void onModeHold() override;

	void render() override;
};
