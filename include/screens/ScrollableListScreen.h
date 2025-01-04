#pragma once

#include "screens/Screen.h"
#include <vector>

class ScrollableListScreen : public Screen {

	int cursorPos = 0;
	const std::vector<const char*> &items;

  public:

	ScrollableListScreen(const std::vector<const char*> &items) :
	  items(items) {
	}

	virtual void onItemSelected(int cursor) = 0;

	void onUp() override {
		// cursorPos--;
		// if(cursorPos < 0) {
		// 	cursorPos = items.size() - 1;
		// }
		if(cursorPos > 0) cursorPos--;
		this->render();
	}

	void onDown() override {
		// cursorPos++;
		// if(cursorPos > items.size() - 1) {
		// 	cursorPos = 0;
		// }
		if(cursorPos < items.size() - 1) cursorPos++;
		this->render();
	}

	void onRight() override {
		this->onItemSelected(cursorPos);
	}

	void onLeft() override {
		this->exit();
	}

	void onMode() override { this->onRight(); }
	void onModeHold() override { this->onLeft(); }

	void render() override {
		//lcd.clear();
		lcd.noCursor();
		if(cursorPos % 2) {
			lcd.setCursor(0, 0);
			lcd.print(" ");
			lcd.print(items[cursorPos - 1]);
			lcd.print("               ");
			lcd.setCursor(0, 1);
			lcd.print(">");
			lcd.print(items[cursorPos]);
			lcd.print("               ");
		}
		else {
			lcd.setCursor(0, 0);
			lcd.print(">");
			lcd.print(items[cursorPos]);
			lcd.print("               ");
			lcd.setCursor(0, 1);
			lcd.print(" ");
			if(cursorPos < items.size() - 1) {
				lcd.print(items[cursorPos + 1]);
			}
			lcd.print("               ");
		}
	}
};
