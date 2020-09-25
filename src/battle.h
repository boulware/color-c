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

	Unit *hovered_unit;
	Unit *selected_unit;
	Ability *hovered_ability;
	Ability *selected_ability; // @note: be careful that any time you deselect a unit, you also clear these ability pointers

	bool is_player_turn;
	OscillatingTimer preview_damage_timer;
	Timer end_button_clicked_timer;
	bool ending_player_turn;

	Unit *units[c::max_target_count]; // 0 through max_party_size-1 are ally slots, the remaining max_party_size slots are enemy slots
	Vec2f unit_slots[c::max_target_count]; // concurrent array to *units
	Intent intents[c::max_target_count]; // concurrent to *units, but currently wasted space because it has room for friendly intents.

	TargetSet hovered_ability_valid_target_set;
	TargetSet selected_ability_valid_target_set;
	TargetSet inferred_target_set;

	bool show_action_preview;
	Intent previewed_intent;
};

void DrawUnits(const Battle *battle);
void DrawTargetingInfo(Battle *battle);
TargetSet AllBattleUnitsAsTargetSet(const Battle *battle);
void DrawUnitHudData(Battle *battle);
void UpdateBattle(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

#endif