#ifndef UNIT_H
#define UNIT_H

#include "data_table.h"

enum class Team
{
	allies,
	enemies
};

enum class TargetClass
{
	self,
	single_ally,
	single_ally_not_self,
	all_allies,
	all_allies_not_self,
	single_enemy,
	all_enemies,
	single_unit,
	single_unit_not_self,
	all_units
};

const char *TargetClass_strings[] = {
	"self",
	"single_ally",
	"single_ally_not_self",
	"all_allies",
	"all_allies_not_self",
	"single_enemy",
	"all_enemies",
	"single_unit",
	"single_unit_not_self",
	"all_units"
};

struct TraitSet
{
	s32 vigor, focus, armor;
};

struct Ability
{
	char *name;
	TraitSet self_required;
	TraitSet target_required;
	TraitSet change_to_self;
	TraitSet change_to_target;

	TargetClass target_class;
};

struct UnitSchematic
{
	char *name;
	TraitSet max_traits;
	int ability_indices[c::moveset_max_size];
};

struct Unit
{
	bool active;

	Team team;
	int schematic_index;
	TraitSet cur_traits;
	Vec2f last_button_pos;
};

struct TargetSet
{
	int size;
	Unit *units[c::max_target_count];
};

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);

bool LoadAbilityFile(const char *filename, DataTable *table);
bool LoadUnitFile(const char *filename, DataTable *table, DataTable ability_table);

bool CreateUnit(const char *name, Unit *unit);
void DrawUnitHudData(Unit unit);

void AddUnitToTargetSet(Unit *unit, TargetSet *target_set);
TargetSet GenerateValidTargetSet(Unit *source, Ability *ability, TargetSet all_targets);
//bool AbilityIsSelected();
//bool IsSelectedAbility(Ability *ability);
void SetSelectedAbility(Ability *ability);

bool UnitInTargetSet(Unit *unit, TargetSet target_set);
void AddUnitToTargetSet(Unit *unit, TargetSet *target_set);

#endif