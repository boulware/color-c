#ifndef UNIT_H
#define UNIT_H


#include "const.h"
#include "text_parsing.h"
#include "ability.h"

enum class Team
{
	allies,
	enemies
};

struct Breed
{
	bool init;

	String name;
	TraitSet max_traits;
	Id<Ability> ability_ids[c::moveset_max_size];
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
	Id<Ability> ability_ids[c::moveset_max_size];
};

struct UnitSlot
{
	Vec2f pos; // physical location on the screen
};

Introspect
struct UnitSet
{
	int size;
	Id<Unit> ids[c::max_target_count];
};

// Unit *UnitAtIndex(UnitSet set, int index);
Id<Unit> *begin(UnitSet &target_set);
Id<Unit> *end(UnitSet &target_set);

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);

bool LoadBreedFile(const char *filename, Table<Breed> *table, Table<Ability> ability_table);

Unit *CreateUnit(int breed_index, Team team);
void DrawUnitHudData(Unit unit);
void DrawTraitSet(Vec2f pos, TraitSet cur_traits, TraitSet max_traits);
Vec2f DrawTraitBarWithPreview(Vec2f pos, int current, int max, int preview, Color color, float flash_timer);
void DrawTraitSetWithPreview(Vec2f pos, TraitSet cur_traits, TraitSet max_traits, TraitSet preview_traits, float flash_timer);
//void DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align);

void AddUnitToUnitSet(Id<Unit> unit_id, UnitSet *target_set);
UnitSet _GenerateValidUnitSet(Unit *caster, Effect *effect, UnitSet all_targets);
//bool AbilityIsSelected();
//bool IsSelectedAbility(Ability *ability);
void _SetSelectedAbility(Ability *ability);

bool UnitInUnitSet(Id<Unit> unit_id, UnitSet target_set, int *index = nullptr);
void _AddUnitToUnitSet(Unit *unit, UnitSet *target_set);

Breed *GetBreed(Unit unit);

bool CheckValidTarget(Id<Unit> caster_id, Id<Unit> target_id, TargetClass tc);

UnitSet GenerateInferredUnitSet(Id<Unit> caster_id, Id<Unit> selected_target_id, TargetClass target_class, UnitSet all_targets);

char *TraitSetString(TraitSet traits);
int DetermineAbilityTier(Id<Unit> caster_id, Id<Ability> ability_id);

Breed *GetBreedFromId(Id<Breed> id);
Unit *GetUnitFromId(Id<Unit> id);

#endif