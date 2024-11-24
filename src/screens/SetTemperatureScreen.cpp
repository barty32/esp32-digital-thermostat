#include "main.h"

void SetTemperatureScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("High:  ");
	lcd.print(highTemp / (double)TEMP_COEF, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");

	lcd.setCursor(0, 1);
	lcd.print("Low:   ");
	lcd.print(lowTemp / (double)TEMP_COEF, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");

	lcd.setCursor(10, cursorPos);
	lcd.cursor();
}

void SetTemperatureScreen::onIncrease() {
	uint32_t* temp = cursorPos ? &lowTemp : &highTemp;
	if(!temp) return;
	(*temp)++;
	if(*temp > MAX_TEMP) {
		*temp = MAX_TEMP;
	}
	this->render();
}

void SetTemperatureScreen::onDecrease() {
	uint32_t* temp = cursorPos ? &lowTemp : &highTemp;
	if(!temp) return;
	(*temp)--;
	if(*temp < MIN_TEMP) {
		*temp = MIN_TEMP;
	}
	this->render();
}

void SetTemperatureScreen::onModePress() {
	if(cursorPos == 0) {
		cursorPos++;
		this->render();
		return;
	}
	//save and exit
	highTemperature = highTemp;
	lowTemperature = lowTemp;
	this->exit();
}
