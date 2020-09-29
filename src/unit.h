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

	String name;
	TraitSet max_traits;
	int ability_table_indices[c::moveset_max_size];
};

struct Unit
{
	bool init;

	String name;
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

struct UnitSet
{
	int size;
	Unit *units[c::max_target_count];

	Unit *operator[](int index);
};

Unit **begin(UnitSet &target_set);
Unit **end(UnitSet &target_set);

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);

bool LoadAbilityFile(const char *filename, DataTable *table);
bool LoadUnitSchematicFile(const char *filename, DataTable *table, DataTable ability_table);

Unit *CreateUnit(int schematic_index, Team team);
void DrawUnitHudData(Unit unit);
void DrawTraitSet(Vec2f pos, TraitSet cur_traits, TraitSet max_traits);
Vec2f DrawTraitBarWithPreview(Vec2f pos, int current, int max, int preview, Color color, float flash_timer);
void DrawTraitSetWithPreview(Vec2f pos, TraitSet cur_traits, TraitSet max_traits, TraitSet preview_traits, float flash_timer);
//void DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align);

void AddUnitToUnitSet(Unit *unit, UnitSet *target_set);
UnitSet GenerateValidUnitSet(Unit *caster, Effect *effect, UnitSet all_targets);
//bool AbilityIsSelected();
//bool IsSelectedAbility(Ability *ability);
void SetSelectedAbility(Ability *ability);

bool UnitInUnitSet(Unit *unit, UnitSet target_set);
void AddUnitToUnitSet(Unit *unit, UnitSet *target_set);

UnitSchematic *GetUnitSchematic(Unit unit);

bool CheckValidAbilityTarget(Unit *source, Unit *target, Effect *effect);

UnitSet GenerateInferredUnitSet(Unit *source, Unit *selected_target, Effect *effect, UnitSet all_targets);

char *TraitSetString(TraitSet traits);
int DetermineAbilityTier(Unit *caster, Ability *ability);

#endif