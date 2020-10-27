#ifndef UNIT_H
#define UNIT_H
#include "const.h"
#include "text_parsing.h"
#include "ability.h"
#include "intent.h"

enum class Team : u8
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
	int tier; // tier 0 is reserved for non-enemy breeds, tiers 1-N are to bucket breeds into difficulties. tier 1 is the easiest.
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

	// Battle-relevant members
	Intent intent;
	Vec2f slot_pos;
};

typedef Array<UnitId> UnitSet;
// struct UnitSet
// {
// 	int size;
// 	Id<Unit> ids[c::max_target_count];
// };

// // Unit *UnitAtIndex(UnitSet set, int index);
// Id<Unit> *begin(UnitSet &target_set);
// Id<Unit> *end(UnitSet &target_set);

bool ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set);
bool ParseNextAsAbilityData(Buffer *buffer, Ability *ability);

bool LoadBreedFile(const char *filename, Table<Breed> *table, Table<Ability> ability_table);

Id<Unit> CreateUnit(Id<Breed> breed_id, Team team, PoolId<Arena> arena_id);
Id<Unit> CreateUnitByName(String name, Team team, PoolId<Arena> arena_id);
Id<Unit> CreateUnitByName(char *name, Team team, PoolId<Arena> arena_id);

void DrawUnitHudData(Unit unit);
void DrawTraitSet(Vec2f pos, TraitSet cur_traits, TraitSet max_traits);
Vec2f DrawTraitBarWithPreview(Vec2f pos, int current, int max, int preview, Color color, float flash_timer);
void DrawTraitSetWithPreview(Vec2f pos, TraitSet cur_traits, TraitSet max_traits, TraitSet preview_traits, float flash_timer);
//void DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align);

//void AddUnitToUnitSet(Id<Unit> unit_id, UnitSet *target_set);
UnitSet _GenerateValidUnitSet(Unit *caster, Effect *effect, UnitSet all_targets);
//bool AbilityIsSelected();
//bool IsSelectedAbility(Ability *ability);
void _SetSelectedAbility(Ability *ability);

bool UnitInUnitSet(Id<Unit> unit_id, UnitSet target_set, int *index = nullptr);
void _AddUnitToUnitSet(Unit *unit, UnitSet *target_set);

Breed *GetBreed(Unit unit);

bool CheckValidTarget(Id<Unit> caster_id, Id<Unit> target_id, TargetClass tc);

void GenerateInferredUnitSet(Id<Unit> caster_id,
	                         Id<Unit> selected_target_id,
	                         TargetClass tc,
	                         Array<UnitId> all_targets,
	                         Array<UnitId> *inferred_target_set);

char *TraitSetString(TraitSet traits);
int DetermineAbilityTier(Id<Unit> caster_id, Id<Ability> ability_id);

Breed *GetBreedFromId(Id<Breed> id);
Unit *GetUnitFromId(Id<Unit> id);

#endif