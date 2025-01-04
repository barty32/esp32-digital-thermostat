#pragma once

#include "main.h"
#include "Day.h"

class Time {
	public:

	using time_t = int64_t;

	//contants
	static const time_t NOT_SET = -1;
	static const time_t ZERO = 0;
	static const time_t SECOND = 1000;
	static const time_t MINUTE = 60 * SECOND;
	static const time_t HOUR = 60 * MINUTE;
	static const time_t DAY = 24 * HOUR;

	time_t time = ZERO;

	Time(time_t time = ZERO) :
	  time(time) {
	}

	static Time fromSeconds(time_t seconds) {
		return Time(seconds * SECOND);
	}

	static Time now() {
		//TODO: get time from RTC
		return Time::fromSeconds(rtc.getEpoch());
		//return Time::millis();
	}

	static time_t millis() {
		return esp_timer_get_time() / 1000;
	}

	static time_t micros() {
		return esp_timer_get_time();
	}

	static Day dayOfWeek() {
		//get from rtc
		return Day::MONDAY;
	}

	int getSeconds() const { return time % MINUTE / SECOND; }
	int getMinutes() const { return time % HOUR / MINUTE; }
	int getHours() const { return time % DAY / HOUR; }
	// int getDay();
	// int getDayofWeek();
	// int getDayofYear();
	// int getMonth();
	// int getYear();

	String toString(bool colon = true) const {
		if(time == NOT_SET) return "--:--";
		String result = "";

		int hours = this->getHours();
		int minutes = this->getMinutes();
		if(hours < 10) {
			result += ' ';
		}
		if(hours > 23) {
			result += "--";
		}
		else {
			result += hours;
		}
		result += colon ? ':' : ' ';
		if(minutes < 10) {
			result += '0';
		}
		if(minutes > 59) {
			result += "--";
		}
		else {
			result += minutes;
		}
		return result;
	}

	time_t toEpoch() const {
		return time;
	}

	time_t toSeconds() const {
		return time / SECOND;
	}

	Time getTimeOfDay() const {
		// 	//return rtc.getEpoch() % (60 * 60 * 24);
		// 	return rtc.getHour() * 3600 + rtc.getMinute() * 60 + rtc.getSecond();
		return time % DAY;
	}

	Time operator+(const Time& right) const { return Time(time + right.time); }
	Time operator-(const Time& right) const { return Time(time - right.time); }

	friend bool operator==(const Time &left, const Time &right) { return left.time == right.time; }
	friend bool operator!=(const Time &left, const Time &right) { return !operator==(left, right); }
	friend bool operator<(const Time &left, const Time &right) { return left.time < right.time; }
	friend bool operator>(const Time &left, const Time &right) { return operator<(right, left); }
	friend bool operator<=(const Time &left, const Time &right) { return !operator>(left, right); }
	friend bool operator>=(const Time &left, const Time &right) { return !operator<(left, right); }
};

