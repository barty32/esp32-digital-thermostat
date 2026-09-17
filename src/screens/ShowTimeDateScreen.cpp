#include "main.h"
#include "Time.h"
#include "screens/ShowTimeDateScreen.h"

ShowTimeDateScreen::ShowTimeDateScreen() {}

static const char* monthNames[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void ShowTimeDateScreen::render() {
	Time time = Time::now();
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
}

void ShowTimeDateScreen::update() {
	static uint32_t lastUpdate = 0;
	if(Time::millis() - lastUpdate > 200) {
		this->render();
		lastUpdate = Time::millis();
	}
}

void ShowTimeDateScreen::onLeft() {
	this->exit();
}

void ShowTimeDateScreen::onRight() {
	this->exit();
}

void ShowTimeDateScreen::onMode() {
	// this->onRight();
}
