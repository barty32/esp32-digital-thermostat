#include "main.h"
#include "screens/SetModeScreen.h"

void SetModeScreen::render() {
	lcd.setCursor(0, 0);
	lcd.print(" Off    Program ");
	lcd.setCursor(0, 1);
	lcd.print(" High   Low     ");
	lcd.setCursor(cursorPos % 2 ? 7 : 0, cursorPos >= 2 ? 1 : 0);
	lcd.cursor();
	lcd.print('>');
}

void SetModeScreen::onUp() {
	cursorPos--;
	if(cursorPos < 0) {
		cursorPos = cursorPositions - 1;
	}
	this->render();
}

void SetModeScreen::onDown() {
	//here we want to move cursor right, so increment
	cursorPos++;
	if(cursorPos > cursorPositions - 1) {
		cursorPos = 0;
	}
	this->render();
}

void SetModeScreen::onMode() {
	/*switch(cursorPos){
		case 0:
			mode = MODE_OFF;
			break;
		case 1:
			mode = MODE_PROGRAM;
			break;
		case 2:
			mode = MODE_HIGH;
			break;
		case 3:
			mode = MODE_LOW;
			break;
	}*/
	this->exit();
}
