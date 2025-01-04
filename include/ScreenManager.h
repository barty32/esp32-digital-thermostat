#pragma once

#include "main.h"
#include "screens/ModalScreen.h"
#include "screens/Screen.h"
#include <stack>
#include <list>

class ScreenManager {

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

	byte customChar2[8] = {
		0b00100,
		0b01010,
		0b10001,
		0b00000,
		0b00000,
		0b10001,
		0b01010,
		0b00100
	};

	std::stack<Screen*, std::list<Screen*>> stack;

  public:

	enum Button {
		BTN_UP,
		BTN_DOWN,
		BTN_LEFT,
		BTN_RIGHT,
		BTN_MODE
	};

	void init() {
		lcd.init();
		lcd.backlight();
		lcd.createChar(0, customChar1);
		lcd.createChar(1, customChar2);
	}

	void push(Screen* scr) {
		scr->manager = this;
		stack.push(scr);
		lcd.clear();
		scr->render();
	}

	void replace(Screen* scr) {
		scr->manager = this;
		Screen* top = stack.top();
		stack.pop();
		delete top;
		stack.push(scr);
		lcd.clear();
		scr->render();
	}

	template<typename T>
	void modal(ModalScreen<T>* scr, std::function<void(T)> onConfirm = [](){}, std::function<void()> onDismiss = [](){}) {
		scr->manager = this;
		scr->confirmCallback = onConfirm;
		scr->dismissCallback = onDismiss;

		//currentScreen->show();
		stack.push(scr);
		lcd.clear();
		scr->render();
	}

	void pop() {
		Screen* old = stack.top();
		stack.pop();
		delete old;
		Screen* scr = stack.top();
		lcd.clear();
		scr->render();
	}

	void render() {
		// if(currentScreen) {
		// 	currentScreen->render();
		// }
		stack.top()->render();
	}

	void update() {
		stack.top()->update();
	}

	void handleButtonPress(Button btn) {
		// if(currentScreen)
		Screen* top = stack.top();
		switch(btn) {
			case BTN_UP:
				top->onUp();
				break;
			case BTN_DOWN:
				top->onDown();
				break;
			case BTN_LEFT:
				top->onLeft();
				break;
			case BTN_RIGHT:
				top->onRight();
				break;
			case BTN_MODE:
				top->onMode();
				break;
		}
	}

	void handleButtonLongPress(Button btn) {
		Screen* top = stack.top();
		switch(btn) {
			case BTN_MODE:
				top->onModeHold();
				break;

			default:
				break;
		}
	}
};





