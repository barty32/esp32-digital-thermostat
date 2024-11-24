
#include "main.h"

void SettingsScreen::showScreen(int cursor) {
	Screen* screen;
	switch(cursor) {
		case OPTION_DATETIME:
			{
				// Screen* screen = new ModeSetupScreen(this);
				// screen->show();
			}
			break;

		case OPTION_NETWORK:
			{
				// Screen* time = new SystemTimeScreen();
				// time->prevScreen = this;
				// time->setCurrentScreen();
			}
			break;

		case OPTION_PROPERTIES:
			{
				// Screen* hystScr = new HysteresisScreen();
				// hystScr->prevScreen = this;
				// hystScr->setCurrentScreen();
			}
			break;

		case OPTION_STATISTICS:
			break;

		case OPTION_FACTORY_RESET:
			// screen = new SettingsScreen(this);
			// screen->show();
			break;

		case OPTION_EXIT:
			this->exit();
			break;

		default:
			break;
	}
}