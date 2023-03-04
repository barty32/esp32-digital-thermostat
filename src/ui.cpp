#include "main.h"

uint32_t* getSetTemperature() {
	if(mode == MODE_DAY) {
		return &dayTemperature;
	}
	else if(mode == MODE_NIGHT) {
		return &nightTemperature;
	}
	else {
		return nullptr;
	}
}


void HomeScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("18:54     ");
	lcd.setCursor(10, 0);
	uint32_t* temp = getSetTemperature();
	if(temp) {
		if(*temp < 100) {
			lcd.print(" ");
		}
		lcd.print(*temp / 10.0, 1);
	}
	else {
		lcd.print("--.-");
	}
	lcd.print((char)223); //degree
	lcd.print("C ");
	lcd.setCursor(0, 1);
	if(mode == MODE_DAY) {
		lcd.print("Day       ");
	}
	else if(mode == MODE_NIGHT) {
		lcd.print("Night     ");
	}
	else if(mode == MODE_TIMES) {
		lcd.print("T-19:00   ");
	}
	lcd.setCursor(10, 1);
	if(currentTemperature < 100) {
		lcd.print(" ");
	}
	lcd.print(currentTemperature / 10.0, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");
}

void HomeScreen::onIncrease() {
	uint32_t* temp = getSetTemperature();
	if(!temp) return;
	(*temp)++;
	if(*temp > 350) {
		*temp = 350;
	}
}
void HomeScreen::onDecrease() {
	uint32_t* temp = getSetTemperature();
	if(!temp) return;
	(*temp)--;
	if(*temp < 50) {
		*temp = 50;
	}
}
void HomeScreen::onModePress() {
	mode = static_cast<Modes>(static_cast<int>(mode) + 1);
	if(mode > MODE_TIMES) {
		mode = MODE_DAY;
	}
}
void HomeScreen::onModeHold() {
	Screen* setup = new SetupScreen();
	setup->prevScreen = this;
	setup->setCurrentScreen();
}


enum Options {
	SET_TEMPERATURE,
	SET_SYSTEM_TIME,
	SET_HYSTERESIS,
	SET_PREDICTION,
	SET_DAY_TIMES,
	SET_EXIT
};

const char* optionNames[] = {
	"Set temperature",
	"Set system time",
	"Set hysteresis",
	"Set prediction",
	"Set day times",
	"Exit"
};

void SetupScreen::render() {
	lcd.clear();
	if(cursorPos % 2) {
		lcd.setCursor(1, 0);
		lcd.print(optionNames[cursorPos - 1]);
		lcd.setCursor(0, 1);
		lcd.print(">");
		lcd.print(optionNames[cursorPos]);
	}
	else {
		lcd.setCursor(0, 0);
		lcd.print(">");
		lcd.print(optionNames[cursorPos]);
		lcd.setCursor(1, 1);
		if(cursorPos < SET_EXIT) {
			lcd.print(optionNames[cursorPos + 1]);
		}
	}
}

void SetupScreen::onIncrease() {
	cursorPos++;
	if(cursorPos > SET_EXIT) {
		cursorPos = 0;
	}
	this->render();
}
void SetupScreen::onDecrease() {
	cursorPos--;
	if(cursorPos < 0) {
		cursorPos = SET_EXIT;
	}
	this->render();
}
void SetupScreen::onModePress() {
	switch(cursorPos) {
		case SET_TEMPERATURE:
			break;
		case SET_SYSTEM_TIME:
			{
			Screen* time = new SystemTimeScreen();
			time->prevScreen = this;
			time->setCurrentScreen();
			}
			break;
		case SET_HYSTERESIS:
			break;
		case SET_PREDICTION:
			break;
		case SET_DAY_TIMES:
			break;
		case SET_EXIT:
			this->onModeHold();
			break;
		default:
			break;
	}
}

void SetupScreen::onModeHold() {
	prevScreen->setCurrentScreen();
	delete this;
}

enum TimeCursorPos {
	TIME_24H_FORMAT,
	TIME_HOURS,
	TIME_MINUTES,
	TIME_AM_PM
};

void SystemTimeScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Set system time:");
	lcd.setCursor(0, 1);
	lcd.print(format24h ? "24h  " : "12h  ");

	lcd.setCursor(5, 1);
	if(hours < 10) {
		lcd.print(' ');
	}
	lcd.print(hours);
	lcd.print(":");
	if(minutes < 10) {
		lcd.print('0');
	}
	lcd.print(minutes);
	if(!format24h) {
		lcd.print(" AM    ");
	}
	else {
		lcd.print("       ");
	}
	
	lcd.cursor();
	switch(cursorPos) {
		case TIME_24H_FORMAT:
			lcd.setCursor(1, 1);
			break;
		case TIME_HOURS:
			lcd.setCursor(6, 1);
			break;
		case TIME_MINUTES:
			lcd.setCursor(9, 1);
			break;
		case TIME_AM_PM:
			lcd.setCursor(12, 1);
			break;
	}
}

void SystemTimeScreen::onIncrease() {
	switch(cursorPos) {
		case TIME_HOURS:
			hours++;
			if(hours > 23 /*|| (format24h && hours > 12)*/) {
				hours = 0;
			}
			break;
		case TIME_MINUTES:
			minutes++;
			if(minutes > 59) {
				minutes = 0;
			}
			break;
		case TIME_24H_FORMAT:
			format24h = !format24h;
			break;
	}
	this->render();
}
void SystemTimeScreen::onDecrease() {
	switch(cursorPos) {
		case TIME_HOURS:
			hours--;
			if(hours > 23/*|| (format24h && hours > 12)*/) {
				hours = 23;
			}
			break;
		case TIME_MINUTES:
			minutes--;
			if(minutes > 59) {
				minutes = 59;
			}
			break;
		case TIME_24H_FORMAT:
			format24h = !format24h;
			break;
	}
	this->render();
}
void SystemTimeScreen::onModePress() {
	cursorPos++;
	if(cursorPos > TIME_AM_PM || (format24h && cursorPos > TIME_MINUTES)) {
		//save time and exit
		this->onModeHold();
		return;
	}
	this->render();
}

void SystemTimeScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}