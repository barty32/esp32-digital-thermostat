#include "main.h"
/*
//button debounce states
volatile uint16_t btn1state = 0;
volatile uint16_t btn2state = 0;
volatile uint16_t btn3state = 0;
volatile uint16_t btn4state = 0;

volatile uint32_t btn1hold = 0;
volatile uint32_t btn2hold = 0;
volatile uint32_t btn3hold = 0;

volatile uint32_t btn1repeat = 0;
volatile uint32_t btn2repeat = 0;
volatile uint32_t btn3repeat = 0;

//button press state
volatile uint8_t btnFlags = 0;

//isr
void readButtons() {
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
}


void checkButtons() {

	switch(btnFlags) {
		//button 1 - mode
		case 0b0001:
			currentScreen->onModePress();
			// Serial.println("Btn1 press");
			btnFlags = 0;
			break;
		//button 1 - hold
		case 0b00010000:
			currentScreen->onModeHold();
			// Serial.println("Btn1 hold");
			btnFlags = 0;
			break;
		//button 2 - up
		case 0b0010:
			currentScreen->onIncrease();
			// Serial.println("Btn++");
			btnFlags = 0;
			break;
		//button 3 - down
		case 0b0100:
			currentScreen->onDecrease();
			// Serial.println("Btn--");
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
}*/

void initADC() {
	analogReadResolution(12);
	// analogSetCycles(255);

	// ADC_ChannelConfTypeDef sConfig = {0};

	// hadc1.Instance = ADC1;
	// hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	// hadc1.Init.ContinuousConvMode = DISABLE;
	// hadc1.Init.DiscontinuousConvMode = DISABLE;
	// hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	// hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	// hadc1.Init.NbrOfConversion = 1;
	// if(HAL_ADC_Init(&hadc1) != HAL_OK) {
	// 	Error_Handler();
	// }

	// sConfig.Channel = ADC_CHANNEL_0;
	// sConfig.Rank = ADC_REGULAR_RANK_1;
	// sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	// if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
	// 	Error_Handler();
	// }

	// if(HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
	// 	/* ADC Calibration Error */
	// 	Error_Handler();
	// }
}

uint32_t samples = 0;
uint32_t numSamples = 0;

uint32_t getTemperature() {
	uint16_t analogValue = 0;

	// if(HAL_ADC_Start(&hadc1) != HAL_OK) {
	// 	return 0;
	// }

	// if(HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
	// 	return 0;
	// }

	// if((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
	// 	analogValue = HAL_ADC_GetValue(&hadc1);
	// }

	// if(numSamples > 1000) {
	// 	currentTemperature = samples / numSamples * 3300 / 4096;
	// 	Serial3.print("Temperature:");
	// 	Serial3.println(currentTemperature);
	// 	samples = 0;
	// 	numSamples = 0;
	// }
	// numSamples++;
	// samples += analogValue;

	//return analogValue ;
	// return analogRead(TEMP_SENSOR_PIN);
	//return analogRead(TEMP_SENSOR_PIN) * 3300 / 4096;
	return 0;
}
