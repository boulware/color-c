#include "random.h"

void Seed(u32 seed_value)
{
	random::default_lcg.seed = seed_value;
}

void
InitLcgSetSeed(LCG *lcg, u32 seed)
{
	//lcg->m = 0xFFFFFFFF;
	lcg->m = 2147483647;
	lcg->a = 1583458089;
	lcg->c = 12345;
	lcg->seed = seed;

	// Do some iterations to get out of the start range
	for(int i=0; i<100; i++) RandomU32(0,0);
}

void
InitLcgSystemSeed(LCG *lcg)
{
	//lcg->m = 0xFFFFFFFF;
	lcg->m = 2147483647;
	lcg->a = 1583458089;
	lcg->c = 12345;
	lcg->seed = __rdtsc();

	// Do some iterations to get out of the start range
	for(int i=0; i<100; i++) RandomU32(0,0);
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

	//log("U32: %u", result);
	return result;
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

	for(int i=0; i<range; i++) log("%u: %u", i, buckets[i]);
}