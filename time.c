#include <stdbool.h>
#include <time.h>

#include "time.h"

#define MILLISECONDS_PER_SEC 1000
#define MILLISECONDS_PER_NANOSECOND 1000000 /* million */
#define NANOSECONDS_PER_SEC 1000000000L /* billion */

double milliseconds(struct timespec *ts)
{
	double s_to_ms = ts->tv_sec * MILLISECONDS_PER_SEC;
	double ns_to_ms = ts->tv_nsec / (double) MILLISECONDS_PER_NANOSECOND;

	return s_to_ms + ns_to_ms;
}

double getTimeSinceLastFrame()
{
	static struct timespec previousTime;
	static bool timeInitialised = false;

	if (!timeInitialised) {
		timespec_get(&previousTime, TIME_UTC);
		timeInitialised = true;
	}

	struct timespec currentTime;
	timespec_get(&currentTime, TIME_UTC);

	double ms = milliseconds(&currentTime) - milliseconds(&previousTime);

	previousTime = currentTime;
	return ms / (double) MILLISECONDS_PER_SEC;
}

