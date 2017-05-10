#include <stdbool.h>
#include <time.h>

#ifdef _WIN32
#include "Windows.h"
#endif

#include "myTime.h"

#define MILLISECONDS_PER_SEC 1000
#define MILLISECONDS_PER_NANOSECOND 1000000 /* million */
#define NANOSECONDS_PER_SEC 1000000000L /* billion */

#ifdef __linux__
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
#elif defined _WIN32
double milliseconds(SYSTEMTIME *time)
{
	// Prudent to include minutes in case a frame takes quite long
	return time->wMinute * 60 * MILLISECONDS_PER_SEC + time->wSecond * MILLISECONDS_PER_SEC + time->wMilliseconds;
}
double getTimeSinceLastFrame()
{
	static SYSTEMTIME previousTime;
	static bool timeInitialised = false;

	if (!timeInitialised) {
		GetSystemTime(&previousTime);
		timeInitialised = true;
	}

	SYSTEMTIME currentTime;
	GetSystemTime(&currentTime);

	double ms = milliseconds(&currentTime) - milliseconds(&previousTime);
	previousTime = currentTime;
	return ms / (double) MILLISECONDS_PER_SEC;
}
#endif

