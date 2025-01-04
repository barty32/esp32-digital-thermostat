
#include "main.h"
#include "ScreenManager.h"
#include "ThermostatController.h"
#include "screens/MenuScreen.h"
#include "screens/SelectScreen.h"
//#include "screens/SetModeScreen.h"
#include "screens/SetTemperatureScreen.h"
#include "screens/SlotSelectScreen.h"
#include "screens/SettingsScreen.h"

void MenuScreen::onItemSelected(int cursor) {
	Screen* screen;
	switch(cursor) {
		case MENU_MODE:
			manager->modal<ThermostatController::Mode>(new SelectScreen<ThermostatController::Mode>("Set mode:", {
				{"Off", ThermostatController::MODE_OFF},
				{"Program", ThermostatController::MODE_PROGRAM},
				{"High", ThermostatController::MODE_HIGH},
				{"Low", ThermostatController::MODE_LOW}
			}, thermostat.getMode()), [](ThermostatController::Mode mode) {
				thermostat.setMode(mode);
			});
			break;
			
		case MENU_TEMPERATURE:
			manager->push(new SetTemperatureScreen());
			break;

		case MENU_TIME_SLOTS:
			manager->push(new SlotSelectScreen());
			break;

		case MENU_SCHEDULE:
			break;

		case MENU_SETTINGS:
			//screen = new SettingsScreen(this);
			//screen->show();
			manager->push(new SettingsScreen());
			break;

		case MENU_EXIT:
			this->exit();
			break;

		default:
			break;
	}
}

void MenuScreen::exit() {

	ThermostatController::PersistentConfig config;
	thermostat.saveConfig(config);
	uint16_t bytes = eeprom.updateBlock(0, (byte*)&config, sizeof(config));
	Serial.println("Updated config in EEPROM, written bytes: " + String(bytes));

	ScrollableListScreen::exit();
}