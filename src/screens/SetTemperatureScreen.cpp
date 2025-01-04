#include "main.h"
#include "screens/SetTemperatureScreen.h"

SetTemperatureScreen::SetTemperatureScreen() :
  minThreshold(thermostat.getMinThreshold()),
  maxThreshold(thermostat.getMaxThreshold()) {
	for(int i = 0; i < ThermostatController::TEMP_SLOT_COUNT; i++) {
		slots[i] = *thermostat.getTemperatureSlot(i);
	}
}

void SetTemperatureScreen::render() {
	lcd.noCursor();
	lcd.home();
	if(cursorPos <= 1) {
		lcd.print(" Min:   Max:    ");

		lcd.setCursor(0, 1);
		lcd.print(" ");
		if(minThreshold < 10.0f) {
			lcd.print(" ");
		}
		lcd.print(minThreshold.toString(1));
		lcd.print(LCD_SYMBOL_DEGREE);
		lcd.print("C ");

		lcd.setCursor(8, 1);
		if(maxThreshold < 10.0f) {
			lcd.print(" ");
		}
		lcd.print(maxThreshold.toString(1));
		lcd.print(LCD_SYMBOL_DEGREE);
		lcd.print("C   ");

		lcd.setCursor(15, 1);
		lcd.print(">");

		lcd.setCursor(cursorPos ? 11 : 4, 1);
		lcd.cursor();
	}
	else {
		lcd.print(" Slot:   Temp:  ");

		lcd.setCursor(0, 1);
		lcd.print("< ");
		
		if(slotIndex < 10) {
			lcd.print(" ");
		}
		lcd.print(slotIndex);
		lcd.print((char)1);
		lcd.print("    ");

		lcd.setCursor(9, 1);
		if(slots[slotIndex] < 10.0f) {
			lcd.print(" ");
		}
		lcd.print(slots[slotIndex].toString(1));
		lcd.print(LCD_SYMBOL_DEGREE);
		lcd.print("C   ");

		lcd.setCursor(cursorPos == 3 ? 12 : 3, 1);
		lcd.cursor();
	}
}

void SetTemperatureScreen::onUp() {
	if(cursorPos == 2) {
		if(slotIndex < ThermostatController::TEMP_SLOT_COUNT - 1) {
			slotIndex++;
			this->render();
		}
		return;
	}
	Temperature* temp;
	switch(cursorPos) {
		case 0:
			temp = &minThreshold;
			break;
		case 1:
			temp = &maxThreshold;
			break;
		case 3:
			temp = &slots[slotIndex];
			break;
	}
	if(temp->isSet()){
		//round up to nearest 0.1 - float math is inprecise
		*temp = (int(temp->temperature * 10.0f) + 1) / 10.0;
		if(*temp > MAX_TEMP) {
			*temp = MAX_TEMP;
		}
	}
	else {
		//this is in the middle
		*temp = 20.0f;
	}
	this->render();
}

void SetTemperatureScreen::onDown() {
	if(cursorPos == 2) {
		if(slotIndex > 0) {
			slotIndex--;
			this->render();
		}
		return;
	}
	Temperature* temp;
	switch(cursorPos) {
		case 0:
			temp = &minThreshold;
			break;
		case 1:
			temp = &maxThreshold;
			break;
		case 3:
			temp = &slots[slotIndex];
			break;
	}
	if(temp->isSet()) {
		*temp = (int(temp->temperature * 10.0f) - 1) / 10.0;
		if(*temp < MIN_TEMP) {
			*temp = MIN_TEMP;
		}
	}
	else {
		*temp = 20.0f;
	}
	this->render();
}

void SetTemperatureScreen::onLeft() {
	if(cursorPos == 0) {
		//exit without saving
		this->exit();
		return;
	}
	cursorPos--;
	this->render();
}

void SetTemperatureScreen::onRight() {
	if(cursorPos >= 3) {
		//save and exit
		thermostat.setMinThreshold(minThreshold);
		thermostat.setMaxThreshold(maxThreshold);
		for(int i = 0; i < ThermostatController::TEMP_SLOT_COUNT; i++) {
			thermostat.getTemperatureSlot(i)->temperature = slots[i].temperature;
		}
		this->exit();
		return;
	}
	cursorPos++;
	this->render();
}

void SetTemperatureScreen::onMode() {
	this->onRight();
}

void SetTemperatureScreen::onModeHold() {
	switch(cursorPos) {
		case 0:
			minThreshold = Temperature::NOT_SET;
			break;
		case 1:
			maxThreshold = Temperature::NOT_SET;
			break;
		case 2:
		case 3:
			slots[slotIndex] = Temperature::NOT_SET;
			break;
	}
	this->render();
}
