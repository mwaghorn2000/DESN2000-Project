#include "plug.h"

static int plug_state = 0;

void plug_set(int on) {
	plug_state = on ? 1 : 0;
}

// Just checking if now minutes falls between coffee_time start and end
int plug_service(int now_minutes, int coffee_time, int enabled) {
	int start;
	int end;
	
	if (!enabled) {
		return 0;
	}
	
	start = coffee_time - PLUG_LEAD;
	if (start < 0) {
		start += 1440;
	}
	
	end = start + PLUG_FOLLOW;
	if (end >= 1440) {
		end -= 1440;
	}
	
	if (start <= end) {
		return (now_minutes >= start && now_minutes < end);
	}
	
	
	// Handle wrapping start and end times
	return (now_minutes >= start || now_minutes < end);
}
