#include "oscillating_timer.h"

void Tick(OscillatingTimer *timer)
{
	if(timer->decreasing)
	{
		timer->cur -= c::frametime_s;
		if(timer->cur <= timer->low)
		{
			timer->decreasing = false;
			float excess_time = timer->low - timer->cur;
			timer->cur = timer->low + excess_time;
		}
	}
	else
	{
		timer->cur += c::frametime_s;
		if(timer->cur >= timer->high)
		{
			timer->decreasing = true;
			float excess_time = timer->cur - timer->high;
			timer->cur = timer->high - excess_time;
		}
	}
}

void ResetLow(OscillatingTimer *timer)
{
	timer->cur = timer->low;
	timer->decreasing = false;
}

void ResetHigh(OscillatingTimer *timer)
{
	timer->cur = timer->high;
	timer->decreasing = true;
}