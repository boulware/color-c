#ifndef BATTLE_H
#define BATTLE_H

#include "draw.h"
#include "game.h"
#include "timer.h"
#include "oscillating_timer.h"

struct Intent
{
	Unit *caster;
	Ability *ability;
	TargetSet targets;
};

struct Battle
{
	Rect hud;

	//Unit *hovered_unit;
	Unit *selected_unit;
	//Ability *hovered_ability;
	Ability *selected_ability; // @note: be careful that any time you deselect a unit, you also clear these ability pointers

	bool is_player_turn;
	OscillatingTimer preview_damage_timer;
	Timer end_button_clicked_timer;
	bool ending_player_turn;

	Unit *units[c::max_target_count]; // 0 through max_party_size-1 are ally slots, the remaining max_party_size slots are enemy slots
	Vec2f unit_slots[c::max_target_count]; // concurrent array to *units
	Intent intents[c::max_target_count]; // concurrent to *units, but currently wasted space because it has room for friendly intents.

	// TargetSet hovered_ability_valid_target_set;
	// TargetSet selected_ability_valid_target_set;
	// TargetSet inferred_target_set;

	bool show_action_preview;
	Intent previewed_intent;
};

// Data associated with a battle-wide change in state
// (e.g., after an ability would be used, this holds data for trait changes for all units, death, status effects, etc.)
struct BattleEvent
{
	// trait_changes[] is concurrent to a particular battle's units[] array, so trait_changes[i]
	// corresponds to the trait change applied to units[i] if the preview were to be executed.
	Battle *battle;
	TraitSet trait_changes[c::max_target_count];
};

void DrawUnits(Battle *battle);
void DrawTargetingInfo(Battle *battle);
TargetSet AllBattleUnitsAsTargetSet(const Battle *battle);
void DrawUnitHudData(Battle *battle);
void UpdateBattle(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawAbilityInfoBox(Vec2f pos, const Ability *ability, Align align);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

void ApplyBattleEvent(const BattleEvent *event);

#endif