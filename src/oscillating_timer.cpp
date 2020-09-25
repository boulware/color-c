#include "oscillating_timer.h"

void Tick(OscillatingTimer *timer)
{
	if(timer->decreasing)
	{
		timer->cur -= timer->speed;
		if(timer->cur < timer->min)
		{
			timer->decreasing = false;
			float excess_time = timer->min - timer->cur;
			timer->cur = timer->min + excess_time;
		}
	}
	else
	{
		timer->cur += timer->speed;
		if(timer->cur > timer->max)
		{
			timer->decreasing = true;
			float excess_time = timer->cur - timer->max;
			timer->cur = timer->max - excess_time;
		}
	}
}

void ResetLow(OscillatingTimer *timer)
{
	timer->cur = timer->min;
	timer->decreasing = false;
}

void ResetHigh(OscillatingTimer *timer)
{
	timer->cur = timer->max;
	timer->decreasing = true;
}