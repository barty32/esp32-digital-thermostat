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
	SET_FACTORY_RESET,
	SET_EXIT
};

const char* optionNames[] = {
	"Set temperature",
	"Set system time",
	"Set hysteresis",
	"Set prediction",
	"Set day times",
	"Factory reset",
	"Exit"};

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
	cursorPos--;
	if(cursorPos < 0) {
		cursorPos = SET_EXIT;
	}
	this->render();
}
void SetupScreen::onDecrease() {
	cursorPos++;
	if(cursorPos > SET_EXIT) {
		cursorPos = 0;
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
			{
				Screen* hystScr = new HysteresisScreen();
				hystScr->prevScreen = this;
				hystScr->setCurrentScreen();
			}
			break;
		case SET_PREDICTION:
			break;
		case SET_DAY_TIMES:
			{
				Screen* daySelect = new DaySelectScreen();
				daySelect->prevScreen = this;
				daySelect->setCurrentScreen();
			}
			break;
		case SET_FACTORY_RESET:
			{
				Screen* daySelect = new FactoryResetScreen();
				daySelect->prevScreen = this;
				daySelect->setCurrentScreen();
			}
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

void HysteresisScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Set hysteresis:");
	lcd.setCursor(1, 1);
	lcd.print(setHysteresis / 100.0, 2);
	lcd.print((char)223); //degree
	lcd.print("C ");

	lcd.setCursor(4, 1);
	lcd.cursor();
}

void HysteresisScreen::onIncrease() {
	setHysteresis++;
	if(setHysteresis > 500) {
		setHysteresis = 500;
	}
	this->render();
}
void HysteresisScreen::onDecrease() {
	setHysteresis--;
	if(setHysteresis > 500) {
		setHysteresis = 0;
	}
	this->render();
}
void HysteresisScreen::onModePress() {
	//save and exit
	hysteresis = setHysteresis;
	this->onModeHold();
}

void HysteresisScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}


void DaySelectScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Select day:");
	lcd.setCursor(0, 1);
	lcd.print("M T W T F S S  A ");
	lcd.setCursor(cursorPos == 7 ? 15 : cursorPos * 2, 1);
	lcd.cursor();
}

void DaySelectScreen::onIncrease() {
	cursorPos++;
	if(cursorPos > 7) {
		cursorPos = 0;
	}
	this->render();
}

void DaySelectScreen::onDecrease() {
	cursorPos--;
	if(cursorPos < 0) {
		cursorPos = 7;
	}
	this->render();
}
void DaySelectScreen::onModePress() {
	SlotSelectScreen* slotSelect = new SlotSelectScreen();
	slotSelect->prevScreen = this;
	slotSelect->day = cursorPos;
	slotSelect->setCurrentScreen();
}

void DaySelectScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}



void SlotSelectScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Select slot:");
	lcd.setCursor(0, 1);
	lcd.print("1 2 3 4 5 6 7 8 ");
	lcd.setCursor(cursorPos * 2, 1);
	lcd.cursor();
	//lcd.print('>');
}

void SlotSelectScreen::onIncrease() {
	cursorPos++;
	if(cursorPos > 7) {
		cursorPos = 0;
	}
	this->render();
}

void SlotSelectScreen::onDecrease() {
	cursorPos--;
	if(cursorPos < 0) {
		cursorPos = 7;
	}
	this->render();
}
void SlotSelectScreen::onModePress() {
	TimeSelectScreen* timeSelect = new TimeSelectScreen(slots[day][cursorPos]);
	timeSelect->prevScreen = this;
	timeSelect->setCurrentScreen();
}

void SlotSelectScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}



void TimeSelectScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("From: ");
	if(startHours < 10) {
		lcd.print(' ');
	}
	if(startHours > 23) {
		lcd.print("--");
	}
	else {
		lcd.print(startHours);
	}
	lcd.print(":");
	if(startMinutes < 10) {
		lcd.print('0');
	}
	if(startMinutes > 59) {
		lcd.print("--");
	}
	else {
		lcd.print(startMinutes);
	}
	lcd.print("     ");

	
	lcd.setCursor(0, 1);
	lcd.print("To:   ");
	if(endHours < 10) {
		lcd.print(' ');
	}
	if(endHours > 23) {
		lcd.print("--");
	}
	else{
		lcd.print(endHours);
	}
	lcd.print(":");
	if(endMinutes < 10) {
		lcd.print('0');
	}
	if(endMinutes > 59) {
		lcd.print("--");
	}
	else {
		lcd.print(endMinutes);
	}

	lcd.print("     ");
	lcd.cursor();
	switch(cursorPos) {
		case 0:
			lcd.setCursor(7, 0);
			break;
		case 1:
			lcd.setCursor(10, 0);
			break;
		case 2:
			lcd.setCursor(7, 1);
			break;
		case 3:
			lcd.setCursor(10, 1);
			break;
	}
}

void TimeSelectScreen::onIncrease() {
	switch(cursorPos) {
		case 0:
			startHours++;
			if(startHours > 23) {
				startHours = 0;
			}
			break;
		case 1:
			startMinutes++;
			if(startMinutes > 59) {
				startMinutes = 0;
			}
			break;
		case 2:
			endHours++;
			if(endHours > 23) {
				endHours = 0;
			}
			break;
		case 3:
			endMinutes++;
			if(endMinutes > 59) {
				endMinutes = 0;
			}
			break;
	}
	this->render();
}
void TimeSelectScreen::onDecrease() {
	switch(cursorPos) {
		case 0:
			startHours--;
			if(startHours > 23) {
				startHours = 23;
			}
			break;
		case 1:
			startMinutes--;
			if(startMinutes > 59) {
				startMinutes = 59;
			}
			break;
		case 2:
			endHours--;
			if(endHours > 23) {
				endHours = 23;
			}
			break;
		case 3:
			endMinutes--;
			if(endMinutes > 59) {
				endMinutes = 59;
			}
			break;
	}
	this->render();
}

// void TimeSelectScreen::onPlusMinus() {
// 	startHours = startMinutes = endHours = endMinutes = -1;
// 	this->render();
// }

void TimeSelectScreen::onModePress() {
	cursorPos++;
	if(cursorPos > 3) {
		//save time and exit
		slot.startTime = (startHours == -1 || startMinutes == -1) ? -1 : startHours * 3600 + startMinutes * 60;
		slot.endTime = (endHours == -1 || endMinutes == -1) ? -1 : endHours * 3600 + endMinutes * 60;
		//this->onModeHold();
		lcd.noCursor();
		prevScreen->setCurrentScreen();
		delete this;
		return;
	}
	this->render();
}

void TimeSelectScreen::onModeHold() {
	startHours = startMinutes = endHours = endMinutes = -1;
	this->render();
}

void FactoryResetScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Are you sure?");
	lcd.setCursor(0, 1);
	lcd.print("   Yes   No   ");
	lcd.setCursor(cursorPos ? 2 : 8, 1);
	lcd.cursor();
	lcd.print('>');
}

void FactoryResetScreen::onIncrease() {
	cursorPos = !cursorPos;
	this->render();
}

void FactoryResetScreen::onDecrease() {
	this->onIncrease();
}

void FactoryResetScreen::onModePress() {
	if(cursorPos){
		//reset everything
	}
	this->onModeHold();
}

void FactoryResetScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}