#include "traitset.h"

s32 &TraitSet::operator[](size_t index)
{
	return *((s32*)this + index); // 0-vigor, 1-focus, 2-armor
}

TraitSet operator+(TraitSet a, TraitSet b)
{
	return {a.vigor+b.vigor, a.focus+b.focus, a.armor+b.armor};
}

TraitSet operator+=(TraitSet &a, TraitSet b)
{
	a = a + b;
	return a;
}

s32 *begin(TraitSet &trait_set)
{
	return (s32*)&trait_set;
}

s32 *end(TraitSet &trait_set)
{
	return (s32*)(&trait_set + 1);
}