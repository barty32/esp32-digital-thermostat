#include "main.h"

//button debounce states
uint16_t btn1state = 0;
uint16_t btn2state = 0;
uint16_t btn3state = 0;
uint16_t btn4state = 0;

uint32_t btn1hold = 0;
uint32_t btn2hold = 0;
uint32_t btn3hold = 0;

uint32_t btn1repeat = 0;
uint32_t btn2repeat = 0;
uint32_t btn3repeat = 0;

//button press state
uint8_t btnFlags = 0;


void checkButtons() {
	// button debounce
	btn1state = (btn1state << 1) | digitalRead(BTN1_PIN) | 0xE000;
	btn2state = (btn2state << 1) | digitalRead(BTN2_PIN) | 0xE000;
	btn3state = (btn3state << 1) | digitalRead(BTN3_PIN) | 0xE000;
	//btn4state = (btn4state << 1) | digitalRead(BTN4_PIN) | 0xE000;

	if(btn1state == 0xF000) {
		btn1hold = millis();
		btn1repeat = 0;
	}
	if(btn2state == 0xF000) {
		btn2hold = millis();
	}
	if(btn3state == 0xF000) {
		btn3hold = millis();
	}
	if(btn1hold && millis() - btn1hold > HOLD_TIME && btn1state == 0xE000 && !btn1repeat) {
		btnFlags |= 0b00010000;
		btn1repeat = 1;
	}
	if(btn2hold && millis() - btn2hold > HOLD_TIME && btn2state == 0xE000 && millis() - btn2repeat > 50) {
		btnFlags |= 0b0010;
		btn2repeat = millis();
	}
	if(btn3hold && millis() - btn3hold > HOLD_TIME && btn3state == 0xE000 && millis() - btn3repeat > 50) {
		btnFlags |= 0b0100;
		btn3repeat = millis();
	}
	if(btn1state == 0xE001 && !btn1repeat) {
		btnFlags |= 0b0001;
	}
	if(btn2state == 0xF000) {
		btnFlags |= 0b0010;
	}
	if(btn3state == 0xF000) {
		btnFlags |= 0b0100;
	}
	//if(btn4state == 0xF000) {
	// 	btnFlags |= 0b1000;
	// }

	switch(btnFlags) {
		//button 1 - mode
		case 0b0001:
			currentScreen->onModePress();
			//Serial3.println("Btn1 press");
			btnFlags = 0;
			break;
		//button 1 - hold
		case 0b00010000:
			currentScreen->onModeHold();
			//Serial3.println("Btn1 hold");
			btnFlags = 0;
			break;
		//button 2 - up
		case 0b0010:
			currentScreen->onIncrease();
			//Serial3.println("Btn++");
			btnFlags = 0;
			break;
		//button 3 - down
		case 0b0100:
			currentScreen->onDecrease();
			//Serial3.println("Btn--");
			btnFlags = 0;
			break;
		// case 0b0110:
		// 	currentScreen->onPlusMinus();
		// 	Serial3.println("Btn Plus and Minus");
		// 	btnFlags = 0;
		// 	break;
		default:
			btnFlags = 0;
	}
}



uint32_t getTemperature() {
	return analogRead(TEMP_SENSOR_PIN) * 200 / 4096 + 100;
}