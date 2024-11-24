#include "main.h"

int32_t* getCurrentModeTemp() {
	if(mode == MODE_HIGH) {
		return &highTemperature;
	}
	else if(mode == MODE_LOW) {
		return &lowTemperature;
	}
	else {
		return isSlotActive() ? &highTemperature : &lowTemperature;
	}
}

const char* dayNames[] = {
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

void printTime(uint32_t hours, uint32_t minutes, bool colon) {
	if(hours < 10) {
		lcd.print(' ');
	}
	if(hours > 23) {
		lcd.print("--");
	}
	else {
		lcd.print(hours);
	}
	lcd.print(colon ? ':' : ' ');
	if(minutes < 10) {
		lcd.print('0');
	}
	if(minutes > 59) {
		lcd.print("--");
	}
	else {
		lcd.print(minutes);
	}
}



/*


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
			{
				Screen* tempSet = new TemperatureSetScreen();
				tempSet->prevScreen = this;
				tempSet->setCurrentScreen();
			}
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
	TIME_DAY,
	TIME_HOURS,
	TIME_MINUTES
};

void SystemTimeScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Set system time:");
	lcd.setCursor(0, 1);
	//lcd.print(format24h ? "24h  " : "12h  ");
	lcd.print(dayNames[day]);

	lcd.setCursor(5, 1);
	printTime(hours, minutes);
	// if(!format24h) {
	// 	lcd.print(" AM    ");
	// }
	// else {
	 	lcd.print("       ");
	// }
	
	lcd.cursor();
	switch(cursorPos) {
		case TIME_DAY:
			lcd.setCursor(2, 1);
			break;
		case TIME_HOURS:
			lcd.setCursor(6, 1);
			break;
		case TIME_MINUTES:
			lcd.setCursor(9, 1);
			break;
	}
}

void SystemTimeScreen::onIncrease() {
	switch(cursorPos) {
		case TIME_DAY:
			day++;
			if(day > 6) {
				day = 0;
			}
			break;
		case TIME_HOURS:
			hours++;
			if(hours > 23) {
				hours = 0;
			}
			break;
		case TIME_MINUTES:
			minutes++;
			if(minutes > 59) {
				minutes = 0;
			}
			break;
	}
	this->render();
}
void SystemTimeScreen::onDecrease() {
	switch(cursorPos) {
		case TIME_DAY:
			day--;
			if(day > 6) {
				day = 6;
			}
			break;
		case TIME_HOURS:
			hours--;
			if(hours > 23) {
				hours = 23;
			}
			break;
		case TIME_MINUTES:
			minutes--;
			if(minutes > 59) {
				minutes = 59;
			}
			break;
	}
	this->render();
}
void SystemTimeScreen::onModePress() {
	cursorPos++;
	if(cursorPos > TIME_MINUTES) {
		//save time and exit
		// rtc.setHours(hours);
		// rtc.setMinutes(minutes);
		// rtc.setWeekDay(day + 1);
		// rtc.setTime(0, minutes, hours);
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
	hysteresis = setHysteresis;//TODO: round to /10
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
	TimeSelectScreen* timeSelect;
	if(day == DAY_ALL) {
		allDaySlot = slots[DAY_MONDAY][cursorPos];
		for(auto &day : slots) {
			if(day[cursorPos] != allDaySlot) {
				allDaySlot.startTime = -1;
				allDaySlot.endTime = -1;
				// allDaySlot.temperature = 0;
				break;
			}
		}
		timeSelect = new TimeSelectScreen(allDaySlot);
	}
	else {
		timeSelect = new TimeSelectScreen(slots[day][cursorPos]);
	}
	
	timeSelect->prevScreen = this;
	timeSelect->setCurrentScreen();
}

void SlotSelectScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}

void SlotSelectScreen::setCurrentScreen(int retval) {
	currentScreen = this;
	if(retval == 1 && day == DAY_ALL) {
		for(auto &day : slots) {
			day[cursorPos] = allDaySlot;
		}
		//allDaySlot = {static_cast<Time>(-1), static_cast<Time>(-1), 0};
	}
	lcd.clear();
	this->render();
}

void TimeSelectScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("From: ");
	printTime(startHours, startMinutes);
	lcd.print("     ");
	
	lcd.setCursor(0, 1);
	lcd.print("To:   ");
	printTime(endHours, endMinutes);
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
				startHours++;
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
				endHours++;
				endMinutes = 0;
			}
			break;
	}
	if((startHours > 23 || startMinutes > 59) && (cursorPos == 0 || cursorPos == 1)) {
		startHours = 0;
		startMinutes = 0;
	}
	if((endHours > 23 || endMinutes > 59) && (cursorPos == 2 || cursorPos == 3)) {
		endHours = 0;
		endMinutes = 0;
	}
	this->render();
}

void TimeSelectScreen::onDecrease() {
	switch(cursorPos) {
		case 0:
			startHours--;
			if(startHours > 23) {
				startHours = -1;
				startMinutes = -1;
			}
			break;
		case 1:
			startMinutes--;
			if(startMinutes > 59) {
				startHours--;
				startMinutes = 59;
			}
			break;
		case 2:
			endHours--;
			if(endHours > 23) {
				endHours = -1;
				endMinutes = -1;
			}
			break;
		case 3:
			endMinutes--;
			if(endMinutes > 59) {
				endHours--;
				endMinutes = 59;
			}
			break;
	}
	if((startHours > 23 || startMinutes > 59) && (cursorPos == 0 || cursorPos == 1)) {
		startHours = -1;
		startMinutes = -1;
	}
	if((endHours > 23 || endMinutes > 59) && (cursorPos == 2 || cursorPos == 3)) {
		endHours = -1;
		endMinutes = -1;
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
		prevScreen->setCurrentScreen(1);
		delete this;
		return;
	}
	this->render();
}

void TimeSelectScreen::onModeHold() {
	// startHours = startMinutes = endHours = endMinutes = -1;
	// this->render();
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
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

void TemperatureSetScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("High:  ");
	lcd.print(highTemp / 10.0, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");

	lcd.setCursor(0, 1);
	lcd.print("Low:   ");
	lcd.print(lowTemp / 10.0, 1);
	lcd.print((char)223); //degree
	lcd.print("C ");

	lcd.setCursor(10, cursorPos);
	lcd.cursor();
}

void TemperatureSetScreen::onIncrease() {
	uint32_t* temp = cursorPos ? &lowTemp : &highTemp;
	if(!temp) return;
	(*temp)++;
	if(*temp > MAX_TEMP) {
		*temp = MAX_TEMP;
	}
	this->render();
}
void TemperatureSetScreen::onDecrease() {
	uint32_t* temp = cursorPos ? &lowTemp : &highTemp;
	if(!temp) return;
	(*temp)--;
	if(*temp < MIN_TEMP) {
		*temp = MIN_TEMP;
	}
	this->render();
}
void TemperatureSetScreen::onModePress() {
	if(cursorPos == 0) {
		cursorPos++;
		this->render();
		return;
	}
	//save and exit
	highTemperature = highTemp;
	lowTemperature = lowTemp;
	this->onModeHold();
}

void TemperatureSetScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}


void RegulationScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print("Set regulation:");
	lcd.setCursor(1, 1);
	lcd.print(setRegulation);
	lcd.print(" %");

	lcd.setCursor(4, 1);
	lcd.cursor();
}

void RegulationScreen::onIncrease() {
	setRegulation++;
	if(setRegulation > 100) {
		setRegulation = 100;
	}
	this->render();
}
void RegulationScreen::onDecrease() {
	setRegulation--;
	if(setRegulation > 100) {
		setRegulation = 0;
	}
	this->render();
}
void RegulationScreen::onModePress() {
	// ::setRegulation(setRegulation);
	this->onModeHold();
}

void RegulationScreen::onModeHold() {
	lcd.noCursor();
	prevScreen->setCurrentScreen();
	delete this;
}*/

// old system time code
//
// enum TimeCursorPos {
// 	TIME_24H_FORMAT,
// 	TIME_HOURS,
// 	TIME_MINUTES,
// 	TIME_AM_PM
// };
// void SystemTimeScreen::render() {
// 	lcd.setCursor(0, 0);
// 	lcd.print("Set system time:");
// 	lcd.setCursor(0, 1);
// 	lcd.print(format24h ? "24h  " : "12h  ");

// 	lcd.setCursor(5, 1);
// 	if(hours < 10) {
// 		lcd.print(' ');
// 	}
// 	lcd.print(hours);
// 	lcd.print(":");
// 	if(minutes < 10) {
// 		lcd.print('0');
// 	}
// 	lcd.print(minutes);
// 	if(!format24h) {
// 		lcd.print(" AM    ");
// 	}
// 	else {
// 		lcd.print("       ");
// 	}

// 	lcd.cursor();
// 	switch(cursorPos) {
// 		case TIME_24H_FORMAT:
// 			lcd.setCursor(1, 1);
// 			break;
// 		case TIME_HOURS:
// 			lcd.setCursor(6, 1);
// 			break;
// 		case TIME_MINUTES:
// 			lcd.setCursor(9, 1);
// 			break;
// 		case TIME_AM_PM:
// 			lcd.setCursor(12, 1);
// 			break;
// 	}
// }

// void SystemTimeScreen::onIncrease() {
// 	switch(cursorPos) {
// 		case TIME_HOURS:
// 			hours++;
// 			if(hours > 23 /*|| (format24h && hours > 12)*/) {
// 				hours = 0;
// 			}
// 			break;
// 		case TIME_MINUTES:
// 			minutes++;
// 			if(minutes > 59) {
// 				minutes = 0;
// 			}
// 			break;
// 		case TIME_24H_FORMAT:
// 			format24h = !format24h;
// 			break;
// 	}
// 	this->render();
// }
// void SystemTimeScreen::onDecrease() {
// 	switch(cursorPos) {
// 		case TIME_HOURS:
// 			hours--;
// 			if(hours > 23 /*|| (format24h && hours > 12)*/) {
// 				hours = 23;
// 			}
// 			break;
// 		case TIME_MINUTES:
// 			minutes--;
// 			if(minutes > 59) {
// 				minutes = 59;
// 			}
// 			break;
// 		case TIME_24H_FORMAT:
// 			format24h = !format24h;
// 			break;
// 	}
// 	this->render();
// }
// void SystemTimeScreen::onModePress() {
// 	cursorPos++;
// 	if(cursorPos > TIME_AM_PM || (format24h && cursorPos > TIME_MINUTES)) {
// 		//save time and exit
// 		rtc.setHours(hours);
// 		rtc.setMinutes(minutes);
// 		this->onModeHold();
// 		return;
// 	}
// 	this->render();
// }

// void SystemTimeScreen::onModeHold() {
// 	lcd.noCursor();
// 	prevScreen->setCurrentScreen();
// 	delete this;
// }