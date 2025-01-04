#pragma once

#include "screens/Screen.h"
#include "ThermostatController.h"

class SetTemperatureScreen : public Screen {
  public:

	int cursorPos = 0;
	int slotIndex = 0;

	Temperature minThreshold;
	Temperature maxThreshold;
	Temperature slots[ThermostatController::TEMP_SLOT_COUNT];

	SetTemperatureScreen();

	void onUp() override;
	void onDown() override;
	void onRight() override;
	void onLeft() override;

	void onMode() override;
	void onModeHold() override;

	void render() override;
};
