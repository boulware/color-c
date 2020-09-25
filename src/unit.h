#ifndef UNIT_H
#define UNIT_H

#include "data_table.h"
#include "const.h"
#include "text_parsing.h"
#include "ability.h"

enum class Team
{
	allies,
	enemies
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