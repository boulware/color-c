#ifndef UNIT_H
#define UNIT_H

#include "data_table.h"
#include "const.h"
#include "text_parsing.h"

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

const char *TargetClass_filestrings[] = {
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

const char *TargetClass_userstrings[] = {
	"Self",
	"Single Ally",
	"Single Ally (Not Self)",
	"All Allies",
	"All Allies (Not Self)",
	"Single Enemy",
	"All Enemies",
	"Single Unit",
	"Single Unit (Not Self)",
	"All"
};

struct TraitSet
{
	s32 vigor, focus, armor;

	s32 &operator[](size_t index);
};

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

struct UnitSchematic
{
	bool init;

	char name[c::max_unit_name_length+1];
	TraitSet max_traits;
	int ability_table_indices[c::moveset_max_size];
};

struct Unit
{
	bool init;

	char name[c::max_unit_name_length+1];
	Team team;
	TraitSet cur_traits;
	TraitSet max_traits;
	int cur_action_points;
	int max_action_points;
	Ability abilities[c::moveset_max_size];
};

struct UnitSlot
{
	Vec2f pos; // physical location on the screen
};

struct TargetSet
{
	int size;
	Unit *units[c::max_target_count];
};

Unit **begin(TargetSet &target_set);
Unit **end(TargetSet &target_set);

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);

bool LoadAbilityFile(const char *filename, DataTable *table);
bool LoadUnitSchematicFile(const char *filename, DataTable *table, DataTable ability_table);

Unit *CreateUnit(int schematic_index, Team team);
void DrawUnitHudData(Unit unit);
void DrawTraitSet(Vec2f pos, TraitSet cur_traits, TraitSet max_traits);
Vec2f DrawTraitBarWithPreview(Vec2f pos, int current, int max, int preview, Color color, float flash_timer);
void DrawTraitSetWithPreview(Vec2f pos, TraitSet cur_traits, TraitSet max_traits, TraitSet preview_traits, float flash_timer);
void DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align);

void AddUnitToTargetSet(Unit *unit, TargetSet *target_set);
TargetSet GenerateValidTargetSet(Unit *source, Ability *ability, TargetSet all_targets);
//bool AbilityIsSelected();
//bool IsSelectedAbility(Ability *ability);
void SetSelectedAbility(Ability *ability);

bool UnitInTargetSet(Unit *unit, TargetSet target_set);
void AddUnitToTargetSet(Unit *unit, TargetSet *target_set);

void ApplyAbilityToTargetSet(Unit *source, Ability ability, TargetSet target_set);

UnitSchematic *GetUnitSchematic(Unit unit);

bool CheckValidAbilityTarget(Unit *source, Unit *target, Ability *ability);

TargetSet GenerateInferredTargetSet(Unit *source, Unit *selected_target, Ability *ability, TargetSet all_targets);

char *TraitSetString(TraitSet traits);

#endif