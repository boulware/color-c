#ifndef OSCILLATING_TIMER_H
#define OSCILLATING_TIMER_H

struct OscillatingTimer
{
	float cur;
	float low, high;
	bool decreasing;
};

void Tick(OscillatingTimer *timer);
void ResetLow(OscillatingTimer *timer);
void ResetHigh(OscillatingTimer *timer);

#endif