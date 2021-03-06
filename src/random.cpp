#include "random.h"

void Seed(u32 seed_value)
{
	random::default_lcg.seed = seed_value;
}

void
InitLcgSetSeed(LCG *lcg, u32 seed)
{
	//lcg->m = 0xFFFFFFFE;
	lcg->m = 2147483647;
	lcg->a = 1583458089;
	lcg->c = 12345;
	lcg->seed = seed;
	Log("Set seed: %zu", lcg->seed);

	// Do some iterations to get out of the start range
	for(int i=0; i<100; i++) RandomU32(0,1);
}

u64
InitLcgSystemSeed(LCG *lcg)
{
	// TODO: Can m actually be 2^32-1 and still be random enough?
	//lcg->m = 0xFFFFFFFE;
	lcg->m = 2147483647;
	lcg->a = 1583458089;
	lcg->c = 12345;
	lcg->seed = __rdtsc();
	Log("System seed: %zu", lcg->seed);

	// Do some iterations to get out of the start range
	for(int i=0; i<100; i++) RandomU32(0,1);

	return lcg->seed;
}

u32
RandomU32(u32 min, u32 max)
{
	if(min > max) return 0;
	if(min == max) return min;
	if(max == 0xFFFFFFFF)
	{
		max -= 1;
	}

	LCG &lcg = random::default_lcg;
	lcg.seed = (lcg.a * lcg.seed + lcg.c) % lcg.m;
	u32 result = (lcg.seed % (max-min+1)) + min;

	//Log("U32: %u", result);
	return result;
}

float
RandomFloat(float min, float max)
{
    if(min > max or min == max) return min;
	u32 roll = RandomU32(0, 0xFFFFFFFF);

	LCG &lcg = random::default_lcg;
	return min + ((float)roll / (float)(lcg.m-1))*(max-min);
}

bool
Chance(float success_chance)
{
	u32 min = 0;
	u32 max = (u32)-1;

	u32 roll = RandomU32(min, max);
	return(0.01f*success_chance >= (float)roll / (float)max);
}

void
TestDistributionAndLog()
{
	u32 trials = 1000000;
	const u32 range = 100;

	u32 buckets[range] = {};
	//for(int i=0; i<range; i++) buckets[i] = 0; // Zero-init buckets

	for(int i=0; i<trials; i++)
	{
		++buckets[RandomU32(0, range-1)];
	}

	for(int i=0; i<range; i++) Log("%u: %u", i, buckets[i]);
}