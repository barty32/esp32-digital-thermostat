
#include "main.h"


void MenuScreen::showScreen(int cursor) {
	Screen* screen;
	switch(cursor) {
		case MENU_SET_MODE:
			screen = new SetModeScreen(this);
			screen->show();
			break;
			
		case MENU_SET_TEMPERATURE:
			screen = new SetTemperatureScreen(this);
			screen->show();
			break;

		case MENU_SET_PROGRAM:
			{
				// Screen* hystScr = new HysteresisScreen();
				// hystScr->prevScreen = this;
				// hystScr->setCurrentScreen();
			}
			break;

		case MENU_SCHEDULE:
			break;

		case MENU_SETTINGS:
			screen = new SettingsScreen(this);
			screen->show();
			break;

		case MENU_EXIT:
			this->exit();
			break;

		default:
			break;
	}
}