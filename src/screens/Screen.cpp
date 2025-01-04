

#include "screens/Screen.h"
#include "ScreenManager.h"

void Screen::exit() {
	// if(prevScreen) {
	// 	prevScreen->show();
	// 	delete this;
	// }
	//error?
	manager->pop();
}