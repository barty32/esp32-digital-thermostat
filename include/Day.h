#pragma once

#include "main.h"

class Day {
  public:
	
	enum Days: uint8_t {
		NONE      = 0x00,
		MONDAY    = 0x01,
		TUESDAY   = 0x02,
		WEDNESDAY = 0x04,
		THURSDAY  = 0x08,
		FRIDAY    = 0x10,
		SATURDAY  = 0x20,
		SUNDAY    = 0x40,
		ALL       = 0x7F,
		MASK      = 0x80,
	};

	Days days = NONE;

	Day(Days day = NONE) :
	  days(day) {
	}

	Day operator++() {
		days = Days(days << 1);
		if(days & MASK) {
			//if sunday is shifted out, include monday
			days = Days((days & ~MASK) | MONDAY);
		}
		return *this;
	}

	void operator--() {
		days = Days((days & MONDAY) ? ((days >> 1) | SUNDAY ) : (days >> 1));
	}

	uint8_t toByte() {
		return days;
	}

	//String getString();
};