#ifndef MYTIME_H
#define MYTIME_H

/* Returns the time since the previous call in seconds
 * For the first call, the return value will be zero
 * WARNING: May be unsuitable for very high frame rates as inter-frame time may
 * be too small to store
 */
double getTimeSinceLastFrame();

#endif

