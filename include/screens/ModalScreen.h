#pragma once

#include "screens/Screen.h"

template<typename T = int>
class ModalScreen : public Screen {

	friend class ScreenManager;

  protected:

	std::function<void(T)> confirmCallback = [](T){};
	std::function<void()> dismissCallback = [](){};

	virtual void confirm(T status) {
		confirmCallback(status);
		this->exit();
	}

	virtual void dismiss() {
		dismissCallback();
		this->exit();
	}

  public:

	ModalScreen() {
	}

	// void onModePress() override {
	// 	this->confirm(0);
	// }

	void onLeft() override {
		this->dismiss();
	}

	void onModeHold() override {
		this->dismiss();
	}

};
