#ifndef BATTLE_H
#define BATTLE_H

#include "draw.h"
#include "game.h"
#include "timer.h"
#include "oscillating_timer.h"
#include "array.h"

struct Intent
{
	Id<Unit> caster_id;
	Id<Ability> ability_id;
	UnitSet targets;
};

// Data associated with a battle-wide change in state
// (e.g., after an ability would be used, this holds data for trait changes for all units, death, status effects, etc.)
struct BattleEvent
{
	Id<Unit> unit_id;
	TraitSet trait_changes;
};

struct Battle
{
	Rect hud;

	Id<Unit> selected_unit_id;
	Id<Ability> selected_ability_id; // @note: be careful that any time you deselect a unit, you also clear these ability pointers

	bool is_player_turn;
	OscillatingTimer preview_damage_timer;
	Timer end_button_clicked_timer;
	bool ending_player_turn;

	UnitSet units; // 0 through max_party_size-1 are ally slots, the remaining max_party_size slots are enemy slots
	Vec2f unit_slots[c::max_target_count]; // concurrent array to *units
	Intent intents[c::max_target_count]; // concurrent to *units, but currently wasted space because it has room for friendly intents.

	// UnitSet hovered_ability_valid_target_set;
	// UnitSet selected_ability_valid_target_set;
	// UnitSet inferred_target_set;

	bool show_preview;
	Intent preview_intent;
	Array<BattleEvent> preview_events;
};

void DrawUnits(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawUnitHudData(Battle *battle);
void UpdateBattle(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawAbilityInfoBox(Vec2f pos, Id<Ability> ability_id, int tier, Align align = c::align_topleft);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

void ApplyBattleEvent(BattleEvent event);

#endif