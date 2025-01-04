#pragma once

#include "screens/ModalScreen.h"
#include <list>

template <typename value_t>
class SelectScreen : public ModalScreen<value_t> {

  public:
	using label_t = const char*;
	using label_val_pair_t = std::pair<label_t, value_t>;
	using option_list_t = std::list<label_val_pair_t>;

  private:
	label_t title;
	const option_list_t items;
	typename option_list_t::const_iterator currentItem = items.begin();

  public:

	//list of <label, value> pairs
	SelectScreen(label_t title, const option_list_t &items, value_t defaultItem) :
	  title(title),
	  items(items) {

		for(auto it = this->items.begin(); it != this->items.end(); ++it) {
			if(it->second == defaultItem) {
				currentItem = it;
				break;
			}
		}
	}

	SelectScreen(label_t title, const option_list_t &items) {
		SelectScreen(title, items, items.begin()->second);
	}

	void onUp() override {
		currentItem++;
		if(currentItem == items.end()) currentItem = items.begin();
		this->render();
	}

	void onDown() override {
		if(currentItem == items.begin()) currentItem = items.end();
		currentItem--;
		this->render();
	}

	void onRight() override {
		this->confirm(currentItem->second);
	}

	void onLeft() override {
		this->dismiss();
	}

	void onMode() override { this->onRight(); }
	void onModeHold() override { this->onLeft(); }

	void render() override {
		//lcd.clear();
		lcd.noCursor();
		lcd.home();
		lcd.print(title);
		lcd.print("               ");

		lcd.setCursor(1, 1);
		lcd.print((char)1);
		lcd.print(currentItem->first);
		lcd.print("              ");

		lcd.setCursor(2, 1);
		lcd.cursor();
	}
};
