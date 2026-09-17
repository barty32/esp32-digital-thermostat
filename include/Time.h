#pragma once

#include "main.h"
#include "Day.h"

class Time {
	public:

	using time_t = int64_t;//this is in milliseconds since epoch, UTC

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

	static Time fromEpoch(time_t seconds) {
		return Time(seconds * SECOND);
	}

	time_t toEpoch() const {
		return time / SECOND;
	}

	static Time now() {
		// return Time::fromSeconds(rtc.getEpoch());
		// This gets the time from ESP internal RTC, which is synced with external RTC at startup
		return Time::fromEpoch(::time(nullptr));
		//return Time::millis();
	}

	static void set(Time time) {
		time_t epoch = time.toEpoch();
		// Set both internal and external RTC
		rtc.setTime(epoch);
		ds1307.adjust(DateTime(epoch));
		log_i("RTC time set to: %s", ctime(&epoch));
	}

	// Returns the number of milliseconds since the program started
	static time_t millis() {
		return esp_timer_get_time() / 1000;
	}

	// Returns the number of microseconds since the program started
	static time_t micros() {
		return esp_timer_get_time();
	}

	// Returns the current day of the week
	// static Day dayOfWeek() {
	// 	int day = rtc.getDayofWeek();
	// 	switch(day) {
	// 		case 0: return Day::SUNDAY;
	// 		case 1: return Day::MONDAY;
	// 		case 2: return Day::TUESDAY;
	// 		case 3: return Day::WEDNESDAY;
	// 		case 4: return Day::THURSDAY;
	// 		case 5: return Day::FRIDAY;
	// 		case 6: return Day::SATURDAY;
	// 		default: return Day::NONE;
	// 	}
	// }

	// int getSeconds() const { return time % MINUTE / SECOND; }
	// int getMinutes() const { return time % HOUR / MINUTE; }
	// int getHours() const { return time % DAY / HOUR; }

	int getSeconds() const {
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		return timeinfo->tm_sec;
	}

	int getMinutes() const {
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		return timeinfo->tm_min;
	}

	int getHours() const {
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		return timeinfo->tm_hour;
	}

	Time getTimeSinceMidnight() const {
		// 	//return rtc.getEpoch() % (60 * 60 * 24);
		// 	return rtc.getHour() * 3600 + rtc.getMinute() * 60 + rtc.getSecond();
		// return time % DAY;
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		return Time((timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec) * SECOND);
	}

	// int getDay();
	Day getDayOfWeek() const {
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		switch(timeinfo->tm_wday) {
			case 0: return Day::SUNDAY;
			case 1: return Day::MONDAY;
			case 2: return Day::TUESDAY;
			case 3: return Day::WEDNESDAY;
			case 4: return Day::THURSDAY;
			case 5: return Day::FRIDAY;
			case 6: return Day::SATURDAY;
			default: return Day::NONE;
		}
	}

	tm& getTimeStruct() const {
		time_t epoch = this->toEpoch();
		tm* timeinfo = localtime((time_t*)&epoch);
		return *timeinfo;
	}

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

	Time operator+(const Time& right) const { return Time(time + right.time); }
	Time operator-(const Time& right) const { return Time(time - right.time); }

	friend bool operator==(const Time &left, const Time &right) { return left.time == right.time; }
	friend bool operator!=(const Time &left, const Time &right) { return !operator==(left, right); }
	friend bool operator<(const Time &left, const Time &right) { return left.time < right.time; }
	friend bool operator>(const Time &left, const Time &right) { return operator<(right, left); }
	friend bool operator<=(const Time &left, const Time &right) { return !operator>(left, right); }
	friend bool operator>=(const Time &left, const Time &right) { return !operator<(left, right); }
};

