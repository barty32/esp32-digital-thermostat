#pragma once

#include "main.h"

#include "Time.h"
#include "Day.h"
#include "TimeSlot.h"

/*
things to save:

saved version


*/

class ThermostatController {

  public:

	enum Mode : byte {
		MODE_OFF,
		MODE_NORMAL
	};

	static const uint8_t TIME_SLOT_COUNT = 16;
	static const uint8_t TEMP_SLOT_COUNT = 16;

	struct Config {
		struct SavedSlot {
			int32_t startTime;//seconds 0-86400
			int32_t endTime;//seconds 0-86400
			uint8_t daysEnabled;//bitmask, 0x00 = one time, 0x7F = every day
			uint8_t temperatureSlot; //slot index 0-TEMP_SLOT_COUNT
			bool active : 1;
		} __attribute__((packed));
		byte version = 0;//format version
		Mode mode;
		Temperature minThreshold;
		Temperature maxThreshold;
		int32_t maximumOnTime;//seconds, 0 to disable
		int32_t minimumOnTime;
		int32_t minimumOffTime;
		Temperature temperatureSlots[TEMP_SLOT_COUNT];
		SavedSlot timeSlots[TIME_SLOT_COUNT];
	} __attribute__((packed));

  private:

	//setpoint variables
	Mode mode = MODE_NORMAL;

	Temperature minThreshold = 17.0;//if the system is below this temperature, heating will be turned on
	Temperature maxThreshold = 25.0;//if the system is above this temperature, heating will be turned off

	Temperature temperatureSlots[TEMP_SLOT_COUNT];
	TimeSlot timeSlots[TIME_SLOT_COUNT];

	Time maximumOnTime = Time::HOUR * 2;
	Time minimumOnTime = Time::MINUTE * 10;
	Time minimumOffTime = Time::MINUTE * 30;

	//for hysteresis, times 100
	//uint32_t hysteresis = 30;

	//process variables
	Temperature currentTemperature;
	bool boilerOn = false;

	Time lastBoilerOn = Time::ZERO;
	Time lastBoilerOff = Time::ZERO;

	//Time heatResponse = 1800;
	//float lastRiseRate = 0.0007f;

  public:

	Mode getMode() {
		return mode;
	}

	void setMode(Mode mode) {
		this->mode = mode;
	}

	void sendTemperatureUpdate(float newTemperature) {
		currentTemperature = newTemperature;
	}

	Temperature getCurrentTemperature() {
		return currentTemperature;
	}

	TimeSlot* getTimeSlot(int index) {
		return &timeSlots[index];
	}

	Temperature* getTemperatureSlot(int index) {
		return &temperatureSlots[index];
	}

	Temperature getMinThreshold() {
		return minThreshold;
	}

	Temperature getMaxThreshold() {
		return maxThreshold;
	}

	void setMinThreshold(Temperature minThreshold) {
		this->minThreshold = minThreshold;
	}

	void setMaxThreshold(Temperature maxThreshold) {
		this->maxThreshold = maxThreshold;
	}

	void update() {
		// static float p = 0.0;
		// p += 0.1;
		// if(p > 1.0) p = 0.0;
		// this->setOutputPower(p);
		// //log_i("Setting dac to %d", (millis() % 1000) / 1000.0 * 255);
		// //dacWrite(ANALOG_CTRL_PIN, (millis() % 1000) / 1000.0 * 255);
		// return;
		//priorities:
		//
		// 1. max on time
		// 2. min on time
		// 3. min off time
		// 4. max temperature
		// 5. min temperature
		//
		//
		if(mode == MODE_OFF) {
			this->setOutputPower(0.0f);
			return;
		}

		Time current = Time::millis();
		if(boilerOn && (current - lastBoilerOn > maximumOnTime)) {
			log_i("Boiler on-time exceeded");
			this->setOutputPower(0.0f);
			return;
		}

		if(!boilerOn && (current - lastBoilerOff < minimumOffTime)) {
			this->setOutputPower(0.0f);
			return;
		}

		if(currentTemperature > maxThreshold) {
			log_i("Max temperature exceeded");
			this->setOutputPower(0.0f);
			return;
		}

		Temperature targetTemperature = this->calculateRegulationTemperature();
		this->regulate(targetTemperature);

		// if(currentTemperature < minThreshold) {
		// 	Serial.println("Min temperature not met");
		// 	this->regulate(minThreshold);
		// }

		// if(mode == MODE_LOW) {
		// 	Serial.println("Regulating to low temperature");
		// 	this->regulate(minThreshold);
		// }
		// else if(mode == MODE_PROGRAM) {
			// Time time = Time::now();
			// TimeSlot* slot = this->getNearestActiveSlot(time);
			// if(!slot) {
			// 	this->regulate(minThreshold);
			// 	return;
			// }
			
			// Time prediction = this->calculateTimePrediction(time, *slot->temperatureSlot);
			// //limit to 1 hour
			// if(prediction > Time::HOUR) prediction = Time::HOUR;

			// if(slot->isWithin(time + prediction)) {
			// 	Serial.println("Regulating to program");
			// 	this->regulate(*slot->temperatureSlot);
			// }
			// else {
			// 	Serial.println("Regulating to low outside program");
			// 	this->regulate(minThreshold);
			// }
		// }
	}

	void regulate(Temperature temperature) {
		Temperature error = temperature - currentTemperature;
		this->setOutputPower(error.temperature / 3.0);
	}

	//0.0 - 1.0
	void setOutputPower(float value) {
		//clamp to 0.0 - 1.0
		value = max(0.0f, min(1.0f, value));
		//value = random(0, 100) / 100.0;
		dacWrite(ANALOG_CTRL_PIN, value * 255);
		log_i("Setting boiler power: %.1f%%", value * 100);

		if(boilerOn && value < 0.1) {
			boilerOn = false;
			lastBoilerOff = Time::millis();
		}
		else if(!boilerOn && value > 0.1) {
			boilerOn = true;
			lastBoilerOn = Time::millis();
		}
	}

	//time: absolute time
	// TimeSlot* getNearestActiveSlot(Time time) {
	// 	TimeSlot* nearestToday = nullptr;
	// 	TimeSlot* nearestTomorrow = nullptr;
	// 	time = time.getTimeOfDay();
	// 	for(TimeSlot &slot : timeSlots) {
	// 		if(!slot.isSet() || !slot.isActive()) continue;
	// 		if(slot.isWithin(time)) return &slot;

	// 		if(slot.isEnabledOn(Time::dayOfWeek()) &&
	// 		   (slot.startTime > time) &&
	// 		   (!nearestToday || slot.startTime - time < nearestToday->startTime - time)) {
	// 			nearestToday = &slot;
	// 		}
	// 		if(slot.isEnabledOn(++Time::dayOfWeek()) &&
	// 		   (!nearestTomorrow || slot.startTime - time < nearestTomorrow->startTime - time)) {
	// 			nearestTomorrow = &slot;
	// 		}
	// 	}
	// 	return nearestToday ? nearestToday : nearestTomorrow;
	// }

	Temperature calculateRegulationTemperature() {

		Time time = Time::now();
		std::list<TimeSlot*> slotCandidates;
		for(TimeSlot &slot : timeSlots) {
			if(!slot.isSet() || !slot.isActive()) continue;
			Time prediction = this->calculateTimePrediction(*slot.temperatureSlot);
			//limit to 1 hour
			if(prediction > Time::HOUR) prediction = Time::HOUR;
			if(slot.isWithin(time) || slot.isWithin(time + prediction)) {
				slotCandidates.push_back(&slot);
			}
		}
		//select the highest temperature from the candidates
		Temperature max = minThreshold;
		for(TimeSlot* slot : slotCandidates) {
			if(*slot->temperatureSlot > max) {
				max = *slot->temperatureSlot;
			}
		}
		return max;
	}

	//calculates how much time is needed to reach the target temperature
	Time calculateTimePrediction(Temperature targetTemperature) {
		if(currentTemperature >= targetTemperature) {
			return Time::ZERO;
		}
		//Time preheat = heatResponse - (highTemperature - currentTemperature) / 10.0 / lastRiseRate;
		//limit to 1 hour
		//if(preheat > 3600) preheat = 3600;
		return Time::MINUTE * 30;
	}

	bool loadConfig(const Config &config) {
		if(config.version != 1) {
			log_e("Invalid saved config version.");
			return false;
		}
		mode = config.mode;
		minThreshold = config.minThreshold;
		maxThreshold = config.maxThreshold;
		maximumOnTime = Time::fromEpoch(config.maximumOnTime);
		minimumOnTime = Time::fromEpoch(config.minimumOnTime);
		minimumOffTime = Time::fromEpoch(config.minimumOffTime);
		for(int i = 0; i < TEMP_SLOT_COUNT; i++) {
			temperatureSlots[i] = config.temperatureSlots[i];
		}
		for(int i = 0; i < TIME_SLOT_COUNT; i++) {
			timeSlots[i].startTime = Time::fromEpoch(config.timeSlots[i].startTime);
			timeSlots[i].endTime = Time::fromEpoch(config.timeSlots[i].endTime);
			timeSlots[i].daysEnabled = static_cast<Day::Days>(config.timeSlots[i].daysEnabled);
			timeSlots[i].temperatureSlot = config.timeSlots[i].temperatureSlot == 0xFF ? nullptr : &temperatureSlots[config.timeSlots[i].temperatureSlot];
			timeSlots[i].active = config.timeSlots[i].active;
		}
		return true;
	}

	void storeConfig(Config &config) {
		config.version = 1;
		config.mode = mode;
		config.minThreshold = minThreshold;
		config.maxThreshold = maxThreshold;
		config.maximumOnTime = maximumOnTime.toEpoch();
		config.minimumOnTime = minimumOnTime.toEpoch();
		config.minimumOffTime = minimumOffTime.toEpoch();
		for(int i = 0; i < TEMP_SLOT_COUNT; i++) {
			config.temperatureSlots[i] = temperatureSlots[i];
		}
		for(int i = 0; i < TIME_SLOT_COUNT; i++) {
			config.timeSlots[i].startTime = timeSlots[i].startTime.toEpoch();
			config.timeSlots[i].endTime = timeSlots[i].endTime.toEpoch();
			config.timeSlots[i].daysEnabled = timeSlots[i].daysEnabled.toByte();
			config.timeSlots[i].temperatureSlot = timeSlots[i].temperatureSlot ? timeSlots[i].temperatureSlot - temperatureSlots : 0xFF;
			config.timeSlots[i].active = timeSlots[i].active;
		}
	}

	/*Slot* isSlotActive(const Time time) {
		for(Slot &slot : slots[rtc.getDayofWeek()]) {
			if(slot.isSet() &&
			   time >= slot.startTime &&
			   time <= slot.endTime) {

				return &slot;
			}
		}
		return nullptr;
	}*/

	// void regulate(int32_t temperature) {
	// 	if(!heaterOn &&
	// 	   currentTemperature < (temperature - hysteresis) &&
	// 	   (millis() - lastHeaterOff) > minimumOffTime) {
	// 		turnOn();
	// 	}
	// 	if(heaterOn &&
	// 	   currentTemperature > temperature &&
	// 	   (millis() - lastHeaterOn) > minimumOnTime) {
	// 		turnOff();
	// 	}
	// }

	// void turnOn() {
	// 	if(heaterOn) return;
	// 	heaterOn = true;
	// 	Serial.println("Heater turned on");
	// 	digitalWrite(LED_PIN, 1);
	// }

	// void turnOff() {
	// 	if(!heaterOn) return;
	// 	heaterOn = false;
	// 	Serial.println("Heater turned off");
	// 	digitalWrite(LED_PIN, 0);
	// }

	// void checkTemperature() {

	// 	Time time = getTimeOfDay();
	// 	switch(mode) {
	// 		case MODE_OFF:
	// 			turnOff();
	// 			break;

	// 		case MODE_LOW:
	// 			regulate(lowTemperature);
	// 			break;

	// 		case MODE_HIGH:
	// 			regulate(highTemperature);
	// 			break;

	// 		case MODE_PROGRAM:
	// 			Slot* active = isSlotActive();
	// 			Slot* nearest = getNearestSlot();
	// 			Time preheat = heatResponse - (highTemperature - currentTemperature) / 10.0 / lastRiseRate;
	// 			//limit to 1 hour
	// 			if(preheat > 3600) preheat = 3600;

	// 			if(active || nearest && time >= nearest->startTime - preheat) {
	// 				regulate(highTemperature);
	// 				break;
	// 			}
	// 			else {
	// 				regulate(lowTemperature);
	// 			}

	// 			break;
	// 	}
	// }

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
};
