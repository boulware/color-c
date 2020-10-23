#ifndef BATTLE_H
#define BATTLE_H

#include "draw.h"
#include "game.h"
#include "timer.h"
#include "oscillating_timer.h"
#include "array.h"
#include "unit.h"
#include "game_state.h"

struct Intent
{
	Id<Unit> caster_id;
	Id<Ability> ability_id;
	UnitSet target_set;
};

// Data associated with a battle-wide change in state
// (e.g., after an ability would be used, this holds data for trait changes for all units, death, status effects, etc.)
struct BattleEvent
{
	Id<Unit> caster_id;
	Id<Unit> target_id;
	TraitSet trait_changes;
};

struct BattleState
{
	bool finished;
};

struct Battle
{
	PoolId<Arena> arena_id;

	Rect hud;

	Id<Unit> selected_unit_id;
	Id<Ability> selected_ability_id; // @note: be careful that any time you deselect a unit, you also clear these ability pointers

	UnitSet units; // 0 through max_party_size-1 are ally slots, the remaining max_party_size slots are enemy slots
	Vec2f unit_slots[c::max_target_count]; // concurrent array to *units
	Array<Intent> intents; // concurrent to *units, but currently wasted space because it has room for friendly intents.


	Intent player_intent;

	bool show_preview;
	Array<BattleEvent> preview_events;

	bool is_player_turn;
	OscillatingTimer preview_damage_timer;
	Timer end_button_clicked_timer;
	bool ending_player_turn;

	String best_choice_string;
};

void InitBattle(Battle *battle, PoolId<Arena> arena_id);
BattleState TickBattle(Battle *battle);

void DrawUnits(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawUnitHudData(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawAbilityInfoBox(Vec2f pos, Id<Ability> ability_id, int tier, Align align = c::align_topleft);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

void ApplyBattleEvent(BattleEvent event);

#endif