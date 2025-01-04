#pragma once

#include "main.h"
#include "Time.h"
#include "Day.h"

struct TimeSlot {
	Time startTime = Time::NOT_SET;
	Time endTime = Time::NOT_SET;
	Temperature* temperatureSlot = nullptr;
	Day daysEnabled = Day::NONE;
	bool active = false;

	// inline bool operator==(const Slot right) const {
	// 	return startTime == right.startTime && endTime == right.endTime/* && right.temperature == temperature*/;
	// }

	// inline bool operator!=(const Slot right) const {
	// 	return !operator==(right);
	// }

	inline bool isSet() const {
		return startTime != Time::NOT_SET && endTime != Time::NOT_SET && temperatureSlot != nullptr;
	}

	inline bool isActive() const { return active; }

	bool isWithin(Time time) const {
		if(time == Time::NOT_SET || startTime == Time::NOT_SET || endTime == Time::NOT_SET) {
			return false;
		}
		return time >= startTime && time <= endTime;
	}

	bool isEnabledOn(Day day) const {
		return daysEnabled.days & day.days;
	}
};