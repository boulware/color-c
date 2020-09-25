#include "battle.h"

#include "draw.h"
#include "game.h"
#include "random.h"

Rect
GetAbilityHudButtonRect(Battle battle, int ability_index)
{
	if(ability_index < 0 or ability_index > c::moveset_max_size)
	{
		log("(" __FUNCTION__ ") received invalid ability_index");
		return Rect{};
	}

	return Rect{battle.hud.pos + c::hud_ability_buttons_offset + Vec2f{0.f, ability_index*(c::hud_ability_button_size.y+c::hud_ability_button_padding)},
				c::hud_ability_button_size};
}

TraitSet
CalculateAdjustedDamage(TraitSet current, TraitSet damage)
{
	TIMED_BLOCK;

	TraitSet adjusted = {};

	// Vigor
	// vigor is reduced by (change_to_target.vigor - current armor), but reduced to a minimum of 1 total damage.
	// positive (healing) change_to_target values are unaffected by armor. -1 is unaffected because armor could only have reduced it to -1 anyway.
	if(damage.vigor >= -1)
	{
		adjusted.vigor = damage.vigor;
	}
	else
	{
		adjusted.vigor = m::Min(-1, damage.vigor + current.armor);
	}

	// Focus
	adjusted.focus = damage.focus;

	// Armor
	adjusted.armor = damage.armor;

	// Clamp adjusted damage so that current+adjusted will not make trait values negative.
	for(int i=0; i<c::trait_count; i++)
	{
		if(adjusted[i] >= 0) continue; // Don't clamp positive (healing) changes

		adjusted[i] = -m::Min(-adjusted[i], current[i]);
	}

	return adjusted;
}

void
DrawUnits(const Battle *battle)
{
	if(Pressed(vk::F12))
	{
		game->test_int++;
	}

	for(int i=0; i<c::max_target_count; i++)
	{
		Unit *unit = battle->units[i];
		if(unit == nullptr) continue;
		if(!unit->init) continue;

		// Change the outline color for selected and hovered units
		Color outline_color = c::black;
		if(unit == battle->selected_unit)
		{
			outline_color = c::green;
		}
		else if(unit == battle->hovered_unit)
		{
			outline_color = c::grey;
		}

		Vec2f origin = battle->unit_slots[i];

		// Draw unit slot outline
		DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

		// Draw unit name
		TextLayout unit_name_layout = c::def_text_layout;
		unit_name_layout.font_size = 32;
		unit_name_layout.align = c::align_topcenter;
		Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name);

		TraitSet preview_traits = unit->cur_traits;
		if(battle->show_action_preview)
		{
			// explicit conditions:
			//		1) 	show_action_preview is true (which indicates that this function should draw
			//			preview_damage information for relevant targets)
			//
			// response:
			// 		1) 	calculate the "preview_traits", which are the trait values that would result
			// 			if the currently selected ability would be confirmed onto the currently hovered unit
			//		2) (aowtc: not currently implemented) calculate other previewed results of the action (including
			//			things like unit death, trait breaks, status effects, etc.)

			// Confirm that there is a selected unit, selected ability, and hovered unit and that they're initialized.
			// if(!battle->selected_unit or !battle->selected_ability or !battle->hovered_unit) break;
			// if(!battle->selected_unit->init or !battle->selected_ability->init or !battle->hovered_unit->init) break;

			if(unit == battle->previewed_intent.caster)
			{
				// Current unit is caster => apply change_to_self
				preview_traits += battle->previewed_intent.ability->change_to_self;
			}
			if(UnitInTargetSet(unit, battle->previewed_intent.targets))
			{
				// Current unit is inferred target, so we apply change_to_target
				preview_traits += CalculateAdjustedDamage(unit->cur_traits, battle->previewed_intent.ability->change_to_target);
			}
			// note: both change_to_self and change_to_target could be applied to the same unit,
			//		 in the case that an ability targets self and has a change_to_self parameter.
		}

		for(s32 &trait : preview_traits) trait = m::Max(0, trait); // Clamp preview traits to positive values

		// Draw trait bars
		DrawTraitSetWithPreview(origin + Vec2f{0.f, name_size.y},
								unit->cur_traits,
								unit->max_traits,
								preview_traits,
								battle->preview_damage_timer.cur);

		DrawText(c::action_points_text_layout, origin + c::action_points_text_offset,
				 "AP: %d", unit->cur_action_points);
	}
}

void
DrawTargetingInfo(Battle *battle)
{
	// Targeting info has a priority list:
	// [decreasing, beginning at highest priority]
	// 1) An ability is selected AND a valid target for that ability is hovered => draw inferred target set and outcome if that ability were to be used.
	// 2) An ability button is being hovered => draw valid targets for hovered ability
	// 3) An ability is selected => draw valid targets for selected ability

	TextLayout target_indication_layout = c::def_text_layout;
	target_indication_layout.font_size = 16;
	target_indication_layout.align = c::align_bottomcenter;

	TargetSet target_set = {};
	if(battle->selected_ability and UnitInTargetSet(battle->hovered_unit, battle->selected_ability_valid_target_set))
	{
		// 1) An ability is selected AND a valid target for that ability is hovered => draw inferred target set and outcome if that ability were to be used.
		target_set = battle->inferred_target_set;
		target_indication_layout.color = c::red;

		battle->previewed_intent = {battle->selected_unit, battle->selected_ability, target_set};
		battle->show_action_preview = true;
	}
	else
	{
		if(battle->hovered_ability and battle->hovered_ability != battle->selected_ability)
		{
			// 2) An ability button is being hovered => draw valid targets for hovered ability
			target_set = battle->hovered_ability_valid_target_set;
			target_indication_layout.color = c::yellow;
		}
		else if(battle->selected_ability)
		{
			// 3) An ability is selected => draw valid targets for selected ability
			target_set = battle->selected_ability_valid_target_set;
			target_indication_layout.color = c::orange;
		}
		else
		{
			// There is no hovered nor selected ability, so there's no targeting
			// info to draw.
			return;
		}
	}

	// For each unit in the battle, draw TARGET above its unit slot if it's in the relevant target set
	for(int i=0; i<c::max_target_count; i++)
	{
		Unit *unit = battle->units[i];
		if(unit == nullptr) continue;
		if(!unit->init) continue;
		if(!UnitInTargetSet(unit, target_set)) continue;

		Vec2f origin = battle->unit_slots[i];
		DrawText(target_indication_layout,
				 origin + Vec2f{0.5f*c::unit_slot_size.x, 0.f},
				 "TARGET");
	}
}

TargetSet
AllBattleUnitsAsTargetSet(const Battle *battle)
{
	TargetSet target_set = {};
	target_set.size = c::max_target_count;
	for(int i=0; i<c::max_target_count; i++)
	{
		target_set.units[i] = battle->units[i];
	}

	return target_set;
}

void
DrawUnitHudData(Battle *battle)
{
	Unit *unit = battle->selected_unit;

	if(unit == nullptr) return; // No unit is selected.
	if(!unit->init) return;		// A unit is selected, but the unit isn't initialized.

	// Draw HUD frame along bottom of screen.
	float bottom_offset = 225.f;
	Vec2f pen = battle->hud.pos;
	DrawFilledRect(battle->hud, c::dk_grey);
	DrawLine(pen, pen+Vec2f{battle->hud.size.x, 0.f}, c::white);

	// Draw unit name
	float left_padding = 20.f;
	float top_padding = 10.f;
	pen.x += left_padding;
	pen.y += top_padding;

	TextLayout name_layout = c::def_text_layout;
	name_layout.font_size = 32;
	pen.y += DrawText(name_layout, pen, unit->name).y;

	// Draw unit traits
	TextLayout trait_layout = c::def_text_layout;
	trait_layout.font_size = 32;
	float name_trait_padding = 20.f;
	pen.y += name_trait_padding;
	pen.y += DrawText(trait_layout, pen, "Vigor: %d/%d",
							unit->cur_traits.vigor, unit->max_traits.vigor).y;
	pen.y += DrawText(trait_layout, pen, "Focus: %d/%d",
							unit->cur_traits.focus, unit->max_traits.focus).y;
	pen.y += DrawText(trait_layout, pen, "Armor: %d/%d",
							unit->cur_traits.armor, unit->max_traits.armor).y;

	// Ability buttons
	ImguiContainer container = g::def_ui_container;
	container.pos = battle->hud.pos + c::hud_ability_buttons_offset;
	SetActiveContainer(&container);

	for(int i=0; i<c::moveset_max_size; i++)
	{
		Ability *ability = &unit->abilities[i];
		if(!ability or !ability->init) continue;

		Rect button_rect = GetAbilityHudButtonRect(*battle, i);

		if(battle->selected_ability == ability)
		{
			// This ability button corresponds to the selected ability
			DrawButton(c::selected_ability_button_layout, button_rect, "%s", ability->name);
		}
		else if(ability == battle->hovered_ability)
		{
			// This ability button is being hovered
			DrawButton(c::hovered_ability_button_layout, button_rect, "%s", ability->name);

			if(Pressed(vk::LMB) and battle->selected_unit->cur_action_points > 0)
			{
				battle->selected_ability = ability;
			}
		}
		else
		{
			// This ability button isn't hovered or selected, so just draw it normally
			DrawButton(c::ability_button_layout, button_rect, "%s", ability->name);
		}
	}
}

void
GenerateEnemyIntents(Battle *battle)
{
	TIMED_BLOCK;

	// Choose enemy abilities randomly and perform them
	for(int i=0; i<c::max_target_count; i++)
	{
		Unit *unit = battle->units[i];
		if(!unit or !unit->init) continue; // Skip non-existent units
		if(unit->team != Team::enemies) continue; // Skip ally units

		u32 chosen_ability_index; // The index of the ability chosen to cast for this enemy
		u32 chosen_target_index; // The index (into battle->units[])
		TargetSet all_targets = AllBattleUnitsAsTargetSet(battle);

		int valid_ability_count = 0; // Number of valid abilities
		int valid_ability_indices[c::moveset_max_size] = {}; // Indices of abilities that are initialized.
		TargetSet valid_target_sets[c::moveset_max_size] = {}; // Valid target sets corresponding to valid_ability_indices
		for(int i=0; i<c::moveset_max_size; i++)
		{
			Ability *ability = &unit->abilities[i];
			if(!ability->init) continue;

			TargetSet valid_target_set = GenerateValidTargetSet(unit, ability, all_targets);
			if(valid_target_set.size > 0)
			{
				valid_ability_indices[valid_ability_count] = i;
				valid_target_sets[i] = valid_target_set;
				++valid_ability_count;
			}

		}

		if(valid_ability_count <= 0) continue;

		chosen_ability_index = valid_ability_indices[RandomU32(0, valid_ability_count-1)];
		Ability *chosen_ability = &unit->abilities[chosen_ability_index];
		TargetSet chosen_ability_valid_targets = valid_target_sets[chosen_ability_index];


		chosen_target_index = RandomU32(0, chosen_ability_valid_targets.size-1);
		Unit *chosen_target = chosen_ability_valid_targets.units[chosen_target_index];

		TargetSet inferred_targets = GenerateInferredTargetSet(unit, chosen_target, chosen_ability, all_targets);

		battle->intents[i] = {unit, chosen_ability, inferred_targets};

		//ApplyAbilityToTargetSet(unit, *chosen_ability, inferred_targets);
	}
}

void
InitiateBattle(Battle *battle)
{
	// Fill out timers
	battle->preview_damage_timer = {};
	battle->preview_damage_timer.max = 1.f;
	battle->preview_damage_timer.speed = 0.02f;

	battle->end_button_clicked_timer = {};
	battle->end_button_clicked_timer.length_s = c::end_button_clicked_time_s;

	// Set action points=1 for all units
	for(Unit *unit : battle->units)
	{
		if(!unit or !unit->init) continue;
		unit->cur_action_points = unit->max_action_points;
	}

	GenerateEnemyIntents(battle);

	// Set to player turn
	battle->is_player_turn = true;
}

void
UpdateHoveredUnit(Battle *battle)
{
	for(int i=0; i<c::max_target_count; i++)
	{
		if(PointInRect(Rect{battle->unit_slots[i], c::unit_slot_size}, MousePos()))
		{
			if(!battle->units[i] or !battle->units[i]->init) continue;
			battle->hovered_unit = battle->units[i];
			break;
		}
	}
}

void
UpdateHoveredAbility(Battle *battle)
{
	if(!battle->selected_unit) return; // No selected unit implies no ability buttons to be hovered.

	for(int i=0; i<c::moveset_max_size; i++)
	{
		Ability *ability = &battle->selected_unit->abilities[i];
		if(!ability or !ability->init) continue;

		Rect ability_button_rect = GetAbilityHudButtonRect(*battle, i);
		if(PointInRect(ability_button_rect, MousePos()))
		{
			battle->hovered_ability = ability;
			return;
		}
	}
}

void
DrawAbilityInfoBox(Vec2f pos, Ability ability, Align align = c::align_topleft)
{
	Rect box_aligned_rect = AlignRect({pos, c::ability_info_box_size}, align);
	Vec2f pen = box_aligned_rect.pos;
	DrawFilledRect(box_aligned_rect, c::ability_info_bg_color);
	DrawUnfilledRect(box_aligned_rect, c::white);

	float center_distance = 0.5f*c::ability_info_box_size.x;

	TextLayout layout = c::def_text_layout;
	layout.align = c::align_topcenter;
	Vec2f name_size = DrawText(layout, pen+Vec2f{center_distance,0.f}, ability.name);
	DrawLine(pen + Vec2f{0.f,name_size.y}, pen + Vec2f{c::ability_info_box_size.x, name_size.y});
	pen.y += name_size.y;

	layout.font_size = 16;
	pen.y += DrawText(layout, pen+Vec2f{center_distance,0.f},
			 		  "target: %s", TargetClass_userstrings[(int)ability.target_class]).y;

	char *format_string = ScratchString(100);

	char *traitset_string = TraitSetString(ability.self_required);
	if(StringLength(traitset_string) > 0)
	{
		sprintf(format_string, "self required: %s", traitset_string);
		pen.y += DrawText(layout, pen+Vec2f{center_distance,0.f},
					  	format_string).y;
	}

	traitset_string = TraitSetString(ability.target_required);
	if(StringLength(traitset_string) > 0)
	{
		sprintf(format_string, "target required: %s", traitset_string);
		pen.y += DrawText(layout, pen+Vec2f{center_distance,0.f},
					  	format_string).y;
	}

	traitset_string = TraitSetString(ability.change_to_self);
	if(StringLength(traitset_string) > 0)
	{
		sprintf(format_string, "change to self: %s", traitset_string);
		pen.y += DrawText(layout, pen+Vec2f{center_distance,0.f},
					  	format_string).y;
	}

	traitset_string = TraitSetString(ability.change_to_target);
	if(StringLength(traitset_string) > 0)
	{
		sprintf(format_string, "change to target: %s", traitset_string);
		pen.y += DrawText(layout, pen+Vec2f{center_distance,0.f},
					  	format_string).y;
	}
}

void
DrawEnemyIntents(Battle *battle)
{
	for(int i=0; i<c::max_target_count; i++)
	{
		Unit *unit = battle->units[i];
		if(!unit or !unit->init or unit->team != Team::enemies) continue;

		Intent intent = battle->intents[i];
		if(!intent.caster or !intent.caster->init or !intent.ability or !intent.ability->init) continue;

		ButtonResponse response =
			DrawButton(c::enemy_intent_button_layout,
					   {battle->unit_slots[i]+c::enemy_intent_offset, c::enemy_intent_button_size},
					   "(%s)", intent.ability->name);

		if(response.hovered)
		{
			battle->show_action_preview = true;
			battle->previewed_intent = intent;

			//DrawAbilityInfoBox(MousePos(), *intent.ability);
		}
	}
}

void
UpdateBattle(Battle *battle)
{
	// Reset these and newly generate them each frame.
	battle->selected_ability_valid_target_set = {};
	battle->hovered_ability_valid_target_set = {};
	battle->inferred_target_set = {};
	battle->hovered_unit = nullptr;
	battle->hovered_ability = nullptr;
	battle->show_action_preview = false;

	Tick(&battle->preview_damage_timer);

	// may change:
	//		hovered_unit
	//		selected_unit
	//		inferred_target_set
	//
	//		(upon executing a unit's action)
	//		inferred_target_set
	//		selected_ability
	//		selected_ability_valid_target_set

	UpdateHoveredUnit(battle);
	UpdateHoveredAbility(battle);

	// Ally unit selection with number keys (1-4)
	if(Pressed(vk::num1))
	{
		battle->selected_unit = battle->units[0];
		battle->selected_ability = nullptr;
	}
	if(Pressed(vk::num2))
	{
		battle->selected_unit = battle->units[1];
		battle->selected_ability = nullptr;
	}
	if(Pressed(vk::num3))
	{
		battle->selected_unit = battle->units[2];
		battle->selected_ability = nullptr;
	}
	if(Pressed(vk::num4))
	{
		battle->selected_unit = battle->units[3];
		battle->selected_ability = nullptr;
	}

	// Tab to go to next ally unit
	if(Pressed(vk::tab))
	{
		// Find index into battle->units of currently selected unit
		int selected_unit_index = -1;
		for(int i=0; i<c::max_target_count; i++)
		{
			Unit *unit = battle->units[i];
			if(!unit or !unit->init) continue;

			if(unit == battle->selected_unit) selected_unit_index = i;
		}

		if(selected_unit_index == -1)
		{
			// If no unit is selected, TAB selects the first unit
			battle->selected_unit = battle->units[0];
		}
		else
		{
			// Go to next unit, except when the last unit it selected; then loop back to the first unit.
			battle->selected_unit = battle->units[(selected_unit_index+1) % c::max_party_size];
		}
	}

	// Ability selection (for currently selected unit) with QWER keys
	if(battle->selected_unit and battle->selected_unit->init and battle->selected_unit->cur_action_points > 0)
	{
		if(Pressed(vk::q))
		{
			Ability *ability = &battle->selected_unit->abilities[0];
			if(ability->init)
			{
				battle->selected_ability = ability;
			}
		}
		if(Pressed(vk::w))
		{
			Ability *ability = &battle->selected_unit->abilities[1];
			if(ability->init)
			{
				battle->selected_ability = ability;
			}
		}
		if(Pressed(vk::e))
		{
			Ability *ability = &battle->selected_unit->abilities[2];
			if(ability->init)
			{
				battle->selected_ability = ability;
			}
		}
		if(Pressed(vk::r))
		{
			Ability *ability = &battle->selected_unit->abilities[3];
			if(ability->init)
			{
				battle->selected_ability = ability;
			}
		}
	}

	if(battle->selected_ability)
	{
		for(Unit *target : battle->units)
		{
			if(CheckValidAbilityTarget(battle->selected_unit, target, battle->selected_ability))
			{
				AddUnitToTargetSet(target, &battle->selected_ability_valid_target_set);
			}
		}

		TargetSet all_targets = AllBattleUnitsAsTargetSet(battle);
		battle->inferred_target_set =
			GenerateInferredTargetSet(battle->selected_unit, battle->hovered_unit,
									  battle->selected_ability, all_targets);

		if(Pressed(vk::LMB) and battle->selected_unit->cur_action_points > 0 and battle->inferred_target_set.size > 0)
		{
			// event: 	Unit slot was clicked while ability selected.
			// action:	Apply selected ability to inferred target set (e.g., lock-in trait changes, deaths, etc.)
			ApplyAbilityToTargetSet(battle->selected_unit,
									*battle->selected_ability,
									battle->inferred_target_set);
			battle->selected_unit->cur_action_points -= 1;
			battle->inferred_target_set = {};
			battle->selected_ability = {};
			battle->selected_ability_valid_target_set = {};
		}
	}
	else
	{
		if(Pressed(vk::LMB))
		{
			// Unit slot was clicked while no ability selected, so just select this unit.
			if(battle->hovered_unit) battle->selected_unit = battle->hovered_unit;
		}
	}



	if(battle->is_player_turn)
	{
		// draws:
		//		unit information (name, traits, ability buttons, ability info box) on HUD for currently selected unit.
		// may change:
		//		selected_ability, selected_ability_valid_target_set,
		//		hovered_ability, hovered_ability_valid_target_set
		DrawUnitHudData(battle);

		if(battle->hovered_ability)
		{
			DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset, *battle->hovered_ability, c::align_topleft);
		}
		else if(battle->selected_ability)
		{
			DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset, *battle->selected_ability, c::align_topleft);
		}

		// draws:
		//		"TARGET" over unit slots contextually
		//		(yellow for hovered ability, orange for selected ability, red for targeting ability)
		// may change:
		//		show_action_preview
		DrawTargetingInfo(battle);
	}

	if(battle->ending_player_turn and Tick(&battle->end_button_clicked_timer))
	{
		if(battle->is_player_turn)
		{
			battle->ending_player_turn = false;
			battle->is_player_turn = false;
		}
	}

	// Draw end-turn button

	if(battle->is_player_turn)
	{
		if(battle->ending_player_turn)
		{
			// It's player turn and the player turn is currently ending
			DrawButton(c::end_button_clicked_layout, c::end_turn_button_rect, "End Turn");
		}
		else
		{
			// It's player turn, but the turn is not ending. Draw the normal end turn button.
			ButtonResponse response = DrawButton(c::end_button_normal_layout, c::end_turn_button_rect, "End Turn");
			if(response.pressed)
			{
				// Start ending the turn if the end turn button is clicked.
				battle->ending_player_turn = true;
				Reset(&battle->end_button_clicked_timer);
			}
		}
	}
	else
	{
		// It's not the player turn, so just grey out the end turn button.
		DrawButton(c::end_button_disabled_layout, c::end_turn_button_rect, "End Turn");
	}

	if(!battle->is_player_turn)
	{
		for(int i=0; i<c::max_target_count; i++)
		{
			Unit *caster = battle->units[i];
			if(!caster or !caster->init) continue;

			Intent intent = battle->intents[i];
			if(!intent.ability)
			{
				//if(c::verbose_error_logging) log("Enemy intent was invalid upon cast attempt (unit index=%d). Skipping.", i);
				continue;
			}
			ApplyAbilityToTargetSet(caster, *intent.ability, intent.targets);
		}

		battle->is_player_turn = true;
		for(Unit *unit : battle->units)
		{
			if(!unit or !unit->init) continue;
			unit->cur_action_points = unit->max_action_points;
		}

		GenerateEnemyIntents(battle);
	}

	// Automatically end the turn if no ally unit has remaining action points.
	{
		bool any_ally_has_ap = false;
		for(Unit *unit : battle->units)
		{
			if(!unit or !unit->init) continue;
			//unit->cur_action_points = unit->max_action_points;
			if(unit->team == Team::allies and unit->cur_action_points > 0) any_ally_has_ap = true;
		}

		if(!any_ally_has_ap) battle->ending_player_turn = true;
	}

	DrawEnemyIntents(battle);
	// if(Down(vk::alt))
	// {
	// 	battle->show_action_preview = true;
	// 	battle->previewed_intent = intent;
	// }

	if(!battle->show_action_preview) ResetLow(&battle->preview_damage_timer);

	// draws:
	//		unit slots (names, traitset bars, unit slot outline)
	DrawUnits(battle);
}