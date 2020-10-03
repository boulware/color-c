#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"

struct LCG
{
	// X' = (a*X + c) % m
	u32 m; 	// modulus
	u32 a;	// "multiplier"
	u32 c;	// "increment"
	u32 seed;
};

namespace random
{
	LCG default_lcg;
	bool log_sequences;
};

void InitLcgSetSeed(LCG *lcg, u32 seed);
void InitLcgSystemSeed(LCG *lcg);
void Seed();
u32 RandomU32(u32 min, u32 max);

#endif