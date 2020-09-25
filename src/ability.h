#ifndef ABILITY_H
#define ABILITY_H

#include "traitset.h"
#include "target_class.h"

enum class EffectType
{
	Damage,
	Steal,
};

struct Effect
{
	// Examples of effects:
	//		Deal 5 vigor damage
	//		Steal 1 armor
	//		Lifelink (possibly redundant with Steal)

	EffectType type;
	void *parameters;
};

struct AbilityTier
{
	// An ability tier has: requirements, effects, target class
	TraitSet required_traits;
	Effect effects[c::max_effect_count];
};

struct Ability
{
	bool init;

	char name[c::max_ability_name_length+1];
	TraitSet self_required;		// traits required to use this ability
	TraitSet target_required;	// traits required for the TARGET to have, in order for this ability to be used against it
	TraitSet change_to_self;	// the change to the source's traits upon use
	TraitSet change_to_target;	// the change to the target's traits upon use

	TargetClass target_class;	// what kinds of units this ability can target (allies, single unit, teams, self, etc.)
};

// struct Ability
// {
// 	bool init;

// 	char name[c::max_ability_name_length+1];
// 	AbilityTier tiers[c::max_ability_tier_count];
// 	TargetClass target_class;
// };

#endif