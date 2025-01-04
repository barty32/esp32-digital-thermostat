#include "screens/HomeScreen.h"
#include "screens/MenuScreen.h"
#include "ScreenManager.h"
#include "ThermostatController.h"

void HomeScreen::render() {
	lcd.noCursor();
	lcd.home();
	//lcd.print(dayNames[rtc.getDayofWeek()]); //rtc.getWeekDay() - 1
	lcd.print("Mon");
	lcd.print(" ");
	lcd.print("10");
	//printTime(rtc.getHour(true), rtc.getMinute(), ((millis() / 1000) % 2));
	// lcd.print(Time::now().toString());
	lcd.print(((millis() / 1000) % 2) ? ":" : " ");
	lcd.print("15");
	lcd.print(" ");
	lcd.setCursor(10, 0);
	// if(currentTemperature < TEMP(10)) {
	// 	lcd.print(" ");
	// }
	// lcd.print(currentTemperature / (double)TEMP_COEF, 1);
	lcd.print(thermostat.getCurrentTemperature().toString());
	lcd.print((char)223); //degree
	lcd.print("C ");
	lcd.setCursor(0, 1);
	if(thermostat.getMode() == ThermostatController::MODE_HIGH) {
		lcd.print("High      ");
	}
	else if(thermostat.getMode() == ThermostatController::MODE_LOW) {
		lcd.print("Low       ");
	}
	else if(thermostat.getMode() == ThermostatController::MODE_PROGRAM) {
		lcd.print("P-19:00   ");
	}
	else if(thermostat.getMode() == ThermostatController::MODE_OFF) {
		lcd.print("Off       ");
	}
	lcd.setCursor(10, 1);
	// int32_t* temp = getCurrentModeTemp();
	// if(temp) {
	// 	if(*temp < TEMP(10)) {
	// 		lcd.print(" ");
	// 	}
	// 	lcd.print(*temp / (double)TEMP_COEF, 1);
	// }
	// else {
	// 	lcd.print("--.-");
	// }
	lcd.print("17.0");
	lcd.print((char)223); //degree
	lcd.print("C ");
	lcd.print("Home screen");
}

void HomeScreen::update() {
	static uint32_t lastUpdate = 0;
	if(millis() - lastUpdate > 1000) {
		this->render();
		lastUpdate = millis();
	}
}

void HomeScreen::onUp() {
	/*int32_t* temp = getCurrentModeTemp();
	if(!temp) return;
	(*temp)++;
	if(*temp > MAX_TEMP) {
		*temp = MAX_TEMP;
	}
	this->render();*/
}

void HomeScreen::onDown() {
	/*int32_t* temp = getCurrentModeTemp();
	if(!temp) return;
	(*temp)--;
	if(*temp < MIN_TEMP) {
		*temp = MIN_TEMP;
	}
	this->render();*/
}

void HomeScreen::onRight() {
	// mode = static_cast<Modes>(static_cast<int>(mode) + 1);
	// if(mode > MODE_PROGRAM) {
	// 	mode = MODE_LOW;
	// }
	//Screen* menu = new MenuScreen(this);
	//menu->show();
	manager->push(new MenuScreen());
}

void HomeScreen::onMode() {
	this->onRight();
}

void HomeScreen::onModeHold() {
	// Screen* setup = new SetupScreen();
	// setup->prevScreen = this;
	// setup->setCurrentScreen();
}
