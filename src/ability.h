#ifndef ABILITY_H
#define ABILITY_H

#include "array.h"
#include "traitset.h"
#include "target_class.h"
#include "text_parsing.h"
#include "effect.h"

struct AbilityTier
{
	TraitSet required_traits;
	TargetClass target_class;
	Array<Effect> effects_;
};

// struct Ability
// {
// 	bool init;

// 	char name[c::max_ability_name_length+1];
// 	TraitSet self_required;		// traits required to use this ability
// 	TraitSet target_required;	// traits required for the TARGET to have, in order for this ability to be used against it
// 	TraitSet change_to_self;	// the change to the source's traits upon use
// 	TraitSet change_to_target;	// the change to the target's traits upon use

// 	TargetClass target_class;	// what kinds of units this ability can target (allies, single unit, teams, self, etc.)
// };

struct Ability
{
	bool init;

	String name;
	Array<AbilityTier> tiers;
    Array<float> tier_potentials;
};

bool ValidAbility(const Ability *ability);
Ability *GetAbilityFromId(Id<Ability> id);

void AllocateAndGenerateAbilityTierPotentials(Ability *ability);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);
bool LoadAbilityFile(const char *filename, Table<Ability> *table);
char *GenerateAbilityTierText(AbilityTier tier);

#endif