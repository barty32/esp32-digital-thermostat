#include "screens/SettingsScreen.h"
#include "screens/SelectScreen.h"
#include "ScreenManager.h"

void SettingsScreen::onItemSelected(int cursor) {
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
			manager->modal<bool>(new SelectScreen<bool>("Are you sure?", {
				{"No", false},
				{"Yes", true}
			}, false), [](bool reset) {
				if(reset) {
					//write invalid version to saved data
					eeprom.writeByte(0, 0xFF);
					lcd.clear();
					lcd.noCursor();
					lcd.home();
					lcd.print("Resetting...");
					lcd.setCursor(0, 1);
					for(int i = 0; i < 16; i++) {
						lcd.print((char)0xFF);
						delay(100);
					}
					ESP.restart();
				}
			});
			break;

		case OPTION_EXIT:
			this->exit();
			break;

		default:
			break;
	}
}
