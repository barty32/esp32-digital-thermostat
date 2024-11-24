#include "main.h"

bool heaterOn = false;

//for hysteresis, times 100
uint32_t hysteresis = 30;

Time minimumOnTime = 0;
Time minimumOffTime = 0;

Time maximumOnTime = 7200000;//two hours

Time lastHeaterOn = 0;
Time lastHeaterOff = 0;

Time heatResponse = 1800;
float lastRiseRate = 0.0007f;


Time getTimeOfDay(){
	//return rtc.getEpoch() % (60 * 60 * 24);
	return rtc.getHour() * 3600 + rtc.getMinute() * 60 + rtc.getSecond();
}

Slot* isSlotActive(const Time time){
	for(Slot& slot : slots[rtc.getDayofWeek()]){
		if(	slot.isSet() && 
			time >= slot.startTime && 
			time <= slot.endTime){

			return &slot;
		}
	}
	return nullptr;
}

Slot* getNearestSlot(const Time time = getTimeOfDay()){
	Slot* nearestSlot = isSlotActive(time);
	if(nearestSlot) return nearestSlot;

	for(Slot &slot : slots[rtc.getDayofWeek()]) {
		// if(dayTime >= slot.startTime && dayTime <= slot.endTime){
		// 	//we're in a slot
		// 	return &slot;
		// 	// break;
		// }
		if(slot.isSet() && 
			(!nearestSlot || slot.startTime - time < nearestSlot->startTime - time)){
			nearestSlot = &slot;
		}
	}
	return nearestSlot;
}

void regulate(int32_t temperature){
	if(!heaterOn &&
	currentTemperature < (temperature - hysteresis) && 
	(millis() - lastHeaterOff) > minimumOffTime){
		turnOn();
	}
	if(heaterOn &&
	currentTemperature > temperature && 
	(millis() - lastHeaterOn) > minimumOnTime){
		turnOff();
	}
}

void turnOn(){
	if(heaterOn) return;
	heaterOn = true;
	Serial.println("Heater turned on");
	digitalWrite(LED_PIN, 1);
}

void turnOff(){
	if(!heaterOn) return;
	heaterOn = false;
	Serial.println("Heater turned off");
	digitalWrite(LED_PIN, 0);
}

void checkTemperature(){

	Time time = getTimeOfDay();
	switch(mode) {
		case MODE_OFF:
			turnOff();
			break;

		case MODE_LOW:
			regulate(lowTemperature);
			break;

		case MODE_HIGH:
			regulate(highTemperature);
			break;

		case MODE_PROGRAM:
			Slot* active = isSlotActive();
			Slot* nearest = getNearestSlot();
			Time preheat = heatResponse - (highTemperature - currentTemperature) / 10.0 / lastRiseRate;
			//limit to 1 hour
			if(preheat > 3600) preheat = 3600;

			if(active || nearest && time >= nearest->startTime - preheat) {
				regulate(highTemperature);
				break;
			}
			else{
				regulate(lowTemperature);
			}

			break;
	}
}

bool sweep(Time time) {
	// Slot* active = isSlotActive(time);
	// if(active){
	// 	//use as MODE_HIGH
	// 	return true;
	// }
	// Slot* nearest = getNearestSlot(time);
	// if(!nearest){
	// 	//no slot in 7 days
	// 	//use as MODE_LOW
	// 	return false;
	// }
	// if(time >= nearest->startTime - heatResponse - (highTemperature - currentTemperature) / 10.0 / lastRiseRate &&
	// time < nearest->startTime - heatResponse){
	// 	//use as MODE_HIGH
	// 	return true;
	// }
	// // else if(time >= nearest->startTime - heatResponse){
	// // 	//use as MODE_LOW
	// // 	return false;
	// // }
	// else{
	// 	//use as MODE_LOW
	// 	return false;
	// }
	return false;
}