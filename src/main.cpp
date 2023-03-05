#include "main.h"



LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial Serial3(PB11, PB10);
//HardwareTimer tim1(TIM1);

uint32_t lastPrint = 0;


Screen* currentScreen = nullptr;
Modes mode = MODE_OFF;

//temperatures, times 10
uint32_t dayTemperature = 210;
uint32_t nightTemperature = 180;
uint16_t currentTemperature = 90;


Slot slots[7][8];//8 slots per day

bool heaterOn = false;

//for hysteresis, times 100
uint32_t hysteresis = 0;

byte customChar1[8] = {
	0b10000,
	0b10100,
	0b10110,
	0b11111,
	0b00111,
	0b00110,
	0b00100,
	0b00000
};

void setup() {
  
	pinMode(BTN1_PIN, INPUT_PULLUP);
	pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
	pinMode(BTN4_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT_ANALOG);

    analogReadResolution(12);

	Serial3.begin(115200);
	lcd.init();
	lcd.backlight();

	lcd.createChar(0, customChar1);

	for(auto &day : slots) {
		for(auto &slot : day) {
			slot.startTime = -1;
			slot.endTime = -1;
			slot.temperature = 0;
		}
	}

	currentScreen = new HomeScreen();
	currentScreen->autoRender = true;
    
    delay(1000);
}

void loop() {
	//cteni potenciometru/termistoru
    //currentTemperature = analogRead(POTENTIOMETER_PIN) * 200 / 4096 + 100;

    checkButtons();

    // if (heaterOn && currentTemperature > (setTemperature - hysteresis)) {
    //     heaterOn = false;
    //     hysteresis = 3;
	// 	printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
    // }
    // if (!heaterOn && currentTemperature < (setTemperature - hysteresis)) {
    //     heaterOn = true;
    //     hysteresis = -3;
    //     printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
    // }

    // if (Serial3.available()) {
	// 	String received = Serial3.readString();
    //     received.trim();
    //     long temp = received.toInt();
    //     if (temp == 0) {
    //         //vypise vsechny zmeny
    //         for (auto& change : changes) {
	// 			printSerial(change, false);
	// 		}
	// 	}
    //     else if (temp > 250 || temp < 150) {
	// 		//Serial3.println("mimo rozsah!");
    //         printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
    //     } else {
    //         temp -= temp % 5;//zaokrouhleni
	// 		setTemperature = temp;
    //         printSerial(ChangeData{ msgNumCounter++, millis(), setTemperature, currentTemperature, heaterOn }, true);
	// 	}
	// }

	if(millis() - lastPrint > 100) {
		if(currentScreen->autoRender){
			currentScreen->render();
		}

        if(heaterOn){
			digitalWrite(LED_PIN, 1);
        }
        else{
			digitalWrite(LED_PIN, 0);
		}

		lastPrint = millis();
		
	}

	delay(1);
}



