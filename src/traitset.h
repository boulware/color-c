#ifndef TRAITSET_H
#define TRAITSET_H

struct TraitSet
{
	s32 vigor, focus, armor;

	s32 &operator[](size_t index);
};

TraitSet operator+(TraitSet a, TraitSet b);
TraitSet operator+=(TraitSet &a, TraitSet b);
s32 *begin(TraitSet &trait_set);
s32 *end(TraitSet &trait_set);


#endif