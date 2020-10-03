#include "timer.h"

bool
Tick(Timer *timer)
{
	if(timer->finished) return true;

	timer->cur += c::frametime_s;
	if(timer->cur >= timer->length_s - timer->start)
	{
		timer->finished = true;
		timer->cur = timer->start + timer->length_s;
	}

	return timer->finished;
}

void Reset(Timer *timer)
{
	timer->cur = timer->start;
	timer->finished = false;
}