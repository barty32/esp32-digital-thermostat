#include "screens/DateAndTimeMenuScreen.h"
#include "screens/ShowTimeDateScreen.h"
#include "screens/SetTimeDateScreen.h"
#include "ScreenManager.h"

void DateAndTimeMenuScreen::onItemSelected(int cursor) {
	Screen* screen;
	switch(cursor) {
		case OPTION_CURRENT:
			manager->push(new ShowTimeDateScreen());
			break;

		case OPTION_ADJUST:
			manager->push(new SetTimeDateScreen());
			break;

		case OPTION_TIMEZONE:
			break;

		case OPTION_SYNC_NETWORK:
			break;

		case OPTION_EXIT:
			this->exit();
			break;

		default:
			break;
	}
}
