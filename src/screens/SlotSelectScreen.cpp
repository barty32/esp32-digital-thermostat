#include "main.h"
#include "screens/SlotSelectScreen.h"

SlotSelectScreen::SlotSelectScreen() {
	for(int i = 0; i < ThermostatController::TIME_SLOT_COUNT; i++) {
		slots[i] = *thermostat.getTimeSlot(i);
	}
}

void SlotSelectScreen::render() {
	lcd.noCursor();
	lcd.home();
	lcd.print("Select slot: ");
	if(slotIndex < 10) {
		lcd.print(" ");
	}
	lcd.print(slotIndex);
	lcd.print((char)1);

	TimeSlot& slot = slots[slotIndex];

	lcd.setCursor(0, 1);
	if(page == 0) {
		if(slot.isActive()) {
			lcd.print("ON  ");
		}
		else {
			lcd.print("OFF ");
		}
		lcd.print(slot.startTime.toString());
		lcd.print("-");
		lcd.print(slot.endTime.toString());
		lcd.print(" ");
	}
	else {
		lcd.print(slot.isEnabledOn(Day::MONDAY   ) ? "M" : "_");
		lcd.print(slot.isEnabledOn(Day::TUESDAY  ) ? "T" : "_");
		lcd.print(slot.isEnabledOn(Day::WEDNESDAY) ? "W" : "_");
		lcd.print(slot.isEnabledOn(Day::THURSDAY ) ? "T" : "_");
		lcd.print(slot.isEnabledOn(Day::FRIDAY   ) ? "F" : "_");
		lcd.print(slot.isEnabledOn(Day::SATURDAY ) ? "S" : "_");
		lcd.print(slot.isEnabledOn(Day::SUNDAY   ) ? "S" : "_");
		lcd.print("   ");

		if(slot.temperatureSlot == nullptr) {
			lcd.print(" N/A ");
		}
		else {
			lcd.print(slot.temperatureSlot->toString(1));
			lcd.print(LCD_SYMBOL_DEGREE);
			lcd.print("C");
		}
	}
	lcd.setCursor(14, 0);
	lcd.cursor();
}

void SlotSelectScreen::onUp() {
	if(slotIndex < ThermostatController::TIME_SLOT_COUNT - 1) {
		slotIndex++;
		this->render();
	}
}

void SlotSelectScreen::onDown() {
	if(slotIndex > 0) {
		slotIndex--;
		this->render();
	}
}

void SlotSelectScreen::onLeft() {
	//exit without saving
	this->exit();
}

void SlotSelectScreen::onRight() {
	//edit slot
}

void SlotSelectScreen::onMode() {
	page = page == 0 ? 1 : 0;
	this->render();
}

void SlotSelectScreen::onModeHold() {
	//show overview
}
