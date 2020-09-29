#ifndef TRAITSET_H
#define TRAITSET_H

#include "text_parsing.h"

Introspect
struct TraitSet
{
	s32 vigor;
	s32 focus;
	s32 armor;

	s32 &operator[](size_t index);
};

TraitSet operator+(TraitSet a, TraitSet b);
TraitSet operator+=(TraitSet &a, TraitSet b);
TraitSet operator-(TraitSet a, TraitSet b);
TraitSet operator-=(TraitSet &a, TraitSet b);
bool operator<(TraitSet a, TraitSet b);
bool operator>(TraitSet a, TraitSet b);
bool operator<=(TraitSet a, TraitSet b);
bool operator>=(TraitSet a, TraitSet b);

s32 *begin(TraitSet &trait_set);
s32 *end(TraitSet &trait_set);

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);

#endif