#include "main.h"
#include "screens/SetTimeDateScreen.h"

SetTimeDateScreen::SetTimeDateScreen() :
  time(Time::now()) {
	
}

static const char* monthNames[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void SetTimeDateScreen::render() {
	const tm &timeinfo = time.getTimeStruct();

	lcd.noCursor();

	lcd.home();
	lcd.print(" ");
	lcd.print(time.getDayOfWeek().toString());
	lcd.print(" ");
	if(timeinfo.tm_mday < 10) lcd.print("0");
	lcd.print(timeinfo.tm_mday);
	lcd.print(" ");
	lcd.print(monthNames[timeinfo.tm_mon]);
	lcd.print(" ");
	lcd.print(timeinfo.tm_year + 1900);

	lcd.setCursor(0, 1);
	lcd.print(" ");
	if(timeinfo.tm_hour < 10) lcd.print("0");
	lcd.print(timeinfo.tm_hour);
	lcd.print(":");
	if(timeinfo.tm_min < 10) lcd.print("0");
	lcd.print(timeinfo.tm_min);
	lcd.print(":");
	if(timeinfo.tm_sec < 10) lcd.print("0");
	lcd.print(timeinfo.tm_sec);

	uint8_t targetX;
	uint8_t targetY;
	switch(cursorPos) {
		case CURSOR_YEAR:
			targetX = 15;
			targetY = 0;
			break;
		case CURSOR_MONTH:
			targetX = 10;
			targetY = 0;
			break;
		case CURSOR_DAY:
			targetX = 6;
			targetY = 0;
			break;
		case CURSOR_HOUR:
			targetX = 2;
			targetY = 1;
			break;
		case CURSOR_MINUTE:
			targetX = 5;
			targetY = 1;
			break;
		case CURSOR_SECOND:
			targetX = 8;
			targetY = 1;
			break;
	}
	lcd.setCursor(targetX, targetY);
	lcd.cursor();
}

void SetTimeDateScreen::onUp() {
	tm timeinfo = time.getTimeStruct();
	switch(cursorPos) {
		case CURSOR_YEAR:
			timeinfo.tm_year++;
			break;
		case CURSOR_MONTH:
			timeinfo.tm_mon++;
			break;
		case CURSOR_DAY:
			timeinfo.tm_mday++;
			break;
		case CURSOR_HOUR:
			timeinfo.tm_hour++;
			break;
		case CURSOR_MINUTE:
			timeinfo.tm_min++;
			break;
		case CURSOR_SECOND:
			timeinfo.tm_sec++;
			break;
	}
	time = Time::fromEpoch(mktime(&timeinfo));
	this->render();
}

void SetTimeDateScreen::onDown() {
	tm timeinfo = time.getTimeStruct();
	switch(cursorPos) {
		case CURSOR_YEAR:
			timeinfo.tm_year--;
			break;
		case CURSOR_MONTH:
			timeinfo.tm_mon--;
			break;
		case CURSOR_DAY:
			timeinfo.tm_mday--;
			break;
		case CURSOR_HOUR:
			timeinfo.tm_hour--;
			break;
		case CURSOR_MINUTE:
			timeinfo.tm_min--;
			break;
		case CURSOR_SECOND:
			timeinfo.tm_sec--;
			break;
	}
	time = Time::fromEpoch(mktime(&timeinfo));
	this->render();
}

void SetTimeDateScreen::onLeft() {
	if(cursorPos == 0) {
		//exit without saving
		this->exit();
		return;
	}
	cursorPos--;
	this->render();
}

void SetTimeDateScreen::onRight() {
	if(cursorPos >= 5) {
		//save and exit
		Time::set(time);
		this->exit();
		return;
	}
	cursorPos++;
	this->render();
}

void SetTimeDateScreen::onMode() {
	// TODO: switch 12/24 hour mode
}

void SetTimeDateScreen::onModeHold() {
	this->render();
}
