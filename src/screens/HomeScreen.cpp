#include "screens/HomeScreen.h"
#include "screens/MenuScreen.h"
#include "ScreenManager.h"
#include "ThermostatController.h"

String dayToString(Day day) {
	// clang-format off
	switch(day.days) {
		case Day::MONDAY:    return "Mon";
		case Day::TUESDAY:   return "Tue";
		case Day::WEDNESDAY: return "Wed";
		case Day::THURSDAY:  return "Thu";
		case Day::FRIDAY:    return "Fri";
		case Day::SATURDAY:  return "Sat";
		case Day::SUNDAY:    return "Sun";
		default:             return "---";
	}
	// clang-format on
}

void HomeScreen::render() {
	Time now = Time::now();
	lcd.noCursor();
	lcd.home();
	lcd.print(dayToString(now.getDayOfWeek()));
	lcd.print(" ");
	lcd.print(now.toString(((Time::millis() / 1000) % 2) ? ":" : " "));
	lcd.print(" ");
	lcd.setCursor(10, 0);
	// if(currentTemperature < TEMP(10)) {
	// 	lcd.print(" ");
	// }
	// lcd.print(currentTemperature / (double)TEMP_COEF, 1);
	lcd.print(thermostat.getCurrentTemperature().toString(1));
	lcd.print((char)223); //degree
	lcd.print("C ");
	lcd.setCursor(0, 1);
	if(thermostat.getMode() == ThermostatController::MODE_OFF) {
		lcd.print("Off       ");
	}
	else {
		lcd.print("Normal    ");
	}
	// if(thermostat.getMode() == ThermostatController::MODE_HIGH) {
	// 	lcd.print("High      ");
	// }
	// else if(thermostat.getMode() == ThermostatController::MODE_LOW) {
	// 	lcd.print("Low       ");
	// }
	// else if(thermostat.getMode() == ThermostatController::MODE_PROGRAM) {
	// 	lcd.print("P-19:00   ");
	// }
	// else if(thermostat.getMode() == ThermostatController::MODE_OFF) {
	// 	lcd.print("Off       ");
	// }
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
	lcd.print(thermostat.getMinThreshold().toString(1));
	lcd.print(LCD_SYMBOL_DEGREE);
	lcd.print("C ");
	// lcd.print("Home screen");
}

void HomeScreen::update() {
	static uint32_t lastUpdate = 0;
	if(Time::millis() - lastUpdate > 500) {
		this->render();
		lastUpdate = Time::millis();
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
