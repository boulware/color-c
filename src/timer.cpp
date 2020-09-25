#include "timer.h"

bool
Tick(Timer *timer)
{
	if(timer->finished) return true;

	timer->cur += c::frametime_s;
	if(timer->cur >= timer->length_s)
	{
		timer->finished = true;
		timer->cur = 0.f;
	}

	return timer->finished;
}

void Reset(Timer *timer)
{
	timer->cur = 0.f;
	timer->finished = false;
}