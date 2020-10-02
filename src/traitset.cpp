#include "traitset.h"

TraitSet operator-(TraitSet a)
{
	return TraitSet{
		.vigor = -a.vigor,
		.focus = -a.focus,
		.armor = -a.armor
	};
}

bool operator==(TraitSet a, TraitSet b)
{
	return(a.vigor == b.vigor and
		   a.focus == b.focus and
		   a.armor == b.armor);
}

bool operator!=(TraitSet a, TraitSet b)
{
	return(!(a==b));
}

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

TraitSet operator-(TraitSet a, TraitSet b)
{
	return {a.vigor-b.vigor, a.focus-b.focus, a.armor-b.armor};
}

TraitSet operator-=(TraitSet &a, TraitSet b)
{
	a = a - b;
	return a;
}

// Returns true if all traits in [a] are lower than all corresponding traits in [b]
bool operator<(TraitSet a, TraitSet b)
{
	return(a[0] < b[0] and a[1] < b[1] and a[2] < b[2]);
}

// Returns true if all traits in [a] are higher than all corresponding traits in [b]
bool operator>(TraitSet a, TraitSet b)
{
	return(a[0] > b[0] and a[1] > b[1] and a[2] > b[2]);
}

// Returns true if all traits in [a] are <= to all corresponding traits in [b]
bool operator<=(TraitSet a, TraitSet b)
{
	return(a[0] <= b[0] and a[1] <= b[1] and a[2] <= b[2]);
}

// Returns true if all traits in [a] are >= to all corresponding traits in [b]
bool operator>=(TraitSet a, TraitSet b)
{
	return(a[0] >= b[0] and a[1] >= b[1] and a[2] >= b[2]);
}

s32 *begin(TraitSet &trait_set)
{
	return (s32*)&trait_set;
}

s32 *end(TraitSet &trait_set)
{
	return (s32*)(&trait_set + 1);
}

bool
ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set)
{
	char *initial = buffer->p;
	TraitSet temp;

	bool valid = true;
	valid = valid && ConfirmNextChar(buffer, '(');
	valid = valid && ParseNextAsS32(buffer, &temp.vigor);
	valid = valid && ConfirmNextChar(buffer, ',');
	valid = valid && ParseNextAsS32(buffer, &temp.focus);
	valid = valid && ConfirmNextChar(buffer, ',');
	valid = valid && ParseNextAsS32(buffer, &temp.armor);
	valid = valid && ConfirmNextChar(buffer, ')');

	if(valid)
	{
		*trait_set = temp;
		return true;
	}
	else
	{
		buffer->p = initial;
		return false;
	}
}