#pragma once

#include <stdint.h>

template <uint8_t num_inputs = 1>
class ButtonReader {
	
	typedef void (*press_listener_t)(uint8_t index, bool repeat);
	typedef void (*long_press_listener_t)(uint8_t index);

  private:

	struct ButtonFlags {
		bool FLAG_PRESS : 1;
		bool FLAG_LONG_PRESS : 1;
		bool FLAG_REPEAT : 1;
		bool FLAG_DETECT_LONG_PRESS : 1;
		bool FLAG_REPEAT_DETECTED : 1;
		bool FLAG_LONG_PRESS_DETECTED : 1;
	};

	const int repeatInterval = 50;
	const int holdTime = 500;

	uint8_t inputs[num_inputs];
	volatile uint16_t states[num_inputs];
	volatile ButtonFlags flags[num_inputs] = {0};
	volatile uint32_t holdStarted[num_inputs] = {0};

	volatile uint32_t repeat = 0;

  protected:

	press_listener_t onPressListener = nullptr;
	long_press_listener_t onLongPressListener = nullptr;

  public:

	ButtonReader(const uint8_t inputPins[num_inputs]) {
		for(int i = 0; i < num_inputs; i++) {
			inputs[i] = inputPins[i];
			states[i] = 0xFFFF;
		}
	}

	virtual ~ButtonReader() {}

	void setOnPressListener(press_listener_t listener) {
		onPressListener = listener;
	}

	void setOnLongPressListener(long_press_listener_t listener) {
		onLongPressListener = listener;
	}

	bool getInputRepeat(uint8_t index) {
		return flags[index].FLAG_REPEAT;
	}

	void setInputRepeat(uint8_t index, bool repeat) {
		flags[index].FLAG_REPEAT = repeat;
	}

	bool getDetectLongPress(uint8_t index) {
		return flags[index].FLAG_DETECT_LONG_PRESS;
	}

	void setDetectLongPress(uint8_t index, bool detect) {
		flags[index].FLAG_DETECT_LONG_PRESS = detect;
	}

	void readInputs_isr() {

		const uint32_t time = this->getSystemTime();

		for(int i = 0; i < num_inputs; i++) {
			states[i] = (states[i] << 1) | this->readPinState(inputs[i]) | 0xE000;

			//executed once on falling edge
			if(states[i] == 0xF000) {
				holdStarted[i] = time;
				flags[i].FLAG_REPEAT_DETECTED = false;
				flags[i].FLAG_LONG_PRESS_DETECTED = false;

				if(flags[i].FLAG_REPEAT || !flags[i].FLAG_DETECT_LONG_PRESS) {
					flags[i].FLAG_PRESS = true;
				}
			}

			//executed repeatedly while the button is held
			if(states[i] == 0xE000 && holdStarted[i] && time - holdStarted[i] > holdTime) {

				if(flags[i].FLAG_REPEAT && time - repeat > repeatInterval) {
					flags[i].FLAG_PRESS = true;
					flags[i].FLAG_REPEAT_DETECTED = true;
				}
				else if(!flags[i].FLAG_REPEAT && flags[i].FLAG_DETECT_LONG_PRESS && !flags[i].FLAG_LONG_PRESS_DETECTED) {
					flags[i].FLAG_LONG_PRESS = true;
					flags[i].FLAG_LONG_PRESS_DETECTED = true;
				}
			}

			//executed once on rising edge
			if(states[i] == 0xE001 && !flags[i].FLAG_REPEAT && !flags[i].FLAG_LONG_PRESS_DETECTED) {
				//this is executed on short press
				flags[i].FLAG_PRESS = true;
			}
		}

		if(time - repeat > repeatInterval) {
			repeat = time;
		}
	}

	void executeHandlers() {
		for(int i = 0; i < num_inputs; i++) {
			if(flags[i].FLAG_PRESS) {
				this->onPress(i, flags[i].FLAG_REPEAT_DETECTED);
				flags[i].FLAG_PRESS = false;
			}
			if(flags[i].FLAG_LONG_PRESS) {
				this->onLongPress(i);
				flags[i].FLAG_LONG_PRESS = false;
			}
		}
	}

  protected:

	virtual uint32_t getSystemTime() = 0;
	virtual bool readPinState(uint8_t pin) = 0;

	virtual void onPress(uint8_t index, bool repeat) {
		if(onPressListener) {
			onPressListener(index, repeat);
		}
	}

	virtual void onLongPress(uint8_t index) {
		if(onLongPressListener) {
			onLongPressListener(index);
		}
	}
	//...on two button press

};
