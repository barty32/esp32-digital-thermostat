#include "main.h"

void HomeScreen::render() {
	lcd.noCursor();
	lcd.setCursor(0, 0);
	lcd.print(dayNames[rtc.getDayofWeek()]); //rtc.getWeekDay() - 1
	lcd.print(" ");
	printTime(rtc.getHour(true), rtc.getMinute(), ((millis() / 1000) % 2)); //rtc.getHours(), rtc.getMinutes()
	lcd.print(" ");
	lcd.setCursor(10, 0);
	if(currentTemperature < TEMP(10)) {
		lcd.print(" ");
	}
	lcd.print(currentTemperature / (double)TEMP_COEF, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");
	lcd.setCursor(0, 1);
	if(mode == MODE_HIGH) {
		lcd.print("High      ");
	}
	else if(mode == MODE_LOW) {
		lcd.print("Low       ");
	}
	else if(mode == MODE_PROGRAM) {
		lcd.print("P-19:00   ");
	}
	else if(mode == MODE_OFF){
		lcd.print("Off       ");
	}
	lcd.setCursor(10, 1);
	int32_t* temp = getCurrentModeTemp();
	if(temp) {
		if(*temp < TEMP(10)) {
			lcd.print(" ");
		}
		lcd.print(*temp / (double)TEMP_COEF, 1);
	}
	else {
		lcd.print("--.-");
	}
	lcd.print((char)223); //degree
	lcd.print("C ");
}

void HomeScreen::onIncrease() {
	int32_t* temp = getCurrentModeTemp();
	if(!temp) return;
	(*temp)++;
	if(*temp > MAX_TEMP) {
		*temp = MAX_TEMP;
	}
	this->render();
}
void HomeScreen::onDecrease() {
	int32_t* temp = getCurrentModeTemp();
	if(!temp) return;
	(*temp)--;
	if(*temp < MIN_TEMP) {
		*temp = MIN_TEMP;
	}
	this->render();
}
void HomeScreen::onModePress() {
	// mode = static_cast<Modes>(static_cast<int>(mode) + 1);
	// if(mode > MODE_PROGRAM) {
	// 	mode = MODE_LOW;
	// }
	Screen* menu = new MenuScreen(this);
	menu->show();
}
void HomeScreen::onModeHold() {
	// Screen* setup = new SetupScreen();
	// setup->prevScreen = this;
	// setup->setCurrentScreen();
}