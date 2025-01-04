#pragma once

#include "screens/Screen.h"
#include "ThermostatController.h"

class SlotSelectScreen : public Screen {
  public:

	int slotIndex = 0;
	uint8_t page = 0;

	TimeSlot slots[ThermostatController::TIME_SLOT_COUNT];

	SlotSelectScreen();

	void onUp() override;
	void onDown() override;
	void onRight() override;
	void onLeft() override;

	void onMode() override;
	void onModeHold() override;

	void render() override;
};
