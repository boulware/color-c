#ifndef TIMER_H
#define TIMER_H

struct Timer
{
	float start;
	float cur;
	float length_s;
	bool finished;
};

bool Tick(Timer *timer);
void Reset(Timer *timer);

#endif