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

// Applies the [damage] to the [current] traitset, accounting for things like armor reducing damage to vigor,
// clamping final traits to positive values, and anything additionally that needs to be done on top of simple
// trait-by-trait addition and subtraction.
TraitSet
CalculateAdjustedDamage(TraitSet current, TraitSet damage)
{
	TIMED_BLOCK;

	TraitSet adjusted = {};

	// Vigor
	// vigor is reduced by (change_to_target.vigor - current armor), but reduced to a minimum of 1 total damage.
	// positive (healing) change_to_target values are unaffected by armor. 1 damage is unaffected because armor could only have reduced it to 1 anyway.
	if(damage.vigor <= 1)
	{
		adjusted.vigor = -damage.vigor;
	}
	else
	{
		adjusted.vigor = -m::Max(1, damage.vigor - current.armor);
	}

	// Focus
	adjusted.focus = -damage.focus;

	// Armor
	adjusted.armor = -damage.armor;

	// Clamp adjusted damage so that current+adjusted will not make trait values negative.
	for(int i=0; i<c::trait_count; i++)
	{
		if(adjusted[i] >= 0) continue; // Don't clamp positive (healing) changes

		adjusted[i] = -m::Min(-adjusted[i], current[i]);
	}

	return adjusted;
}

bool
ValidUnit(const Unit *unit)
{
	return(unit and unit->init);
}

bool
ValidAbility(const Ability *ability)
{
	return(ability and ability->init);
}

BattleEvent
GenerateBattlePreviewEvent(Battle *battle, Intent intent)
{
	TIMED_BLOCK;

	if(!battle or !ValidUnit(intent.caster) or !ValidAbility(intent.ability))
	{
		if(c::verbose_error_logging) log( __FUNCTION__ "() [ln:%u] received nullptr battle or invalid intent)", __LINE__);
		return BattleEvent{};
	}

	// Check for valid intent target set size.
	if(intent.targets.size > c::max_target_count)
	{
		if(c::verbose_error_logging) log(__FUNCTION__"() received an intent whose target set had an invalid size (%d)", intent.targets.size);

		#if DEBUG_BUILD
			Assert(false);
		#else
			return BattleEvent{};
		#endif
	}

	// Find index into battle->units of the caster (intent.caster)
	int caster_index = -1;
	for(int i=0; i<ArrayCount(battle->units); i++)
	{
		if(intent.caster == battle->units[i])
		{
			caster_index = i;
			break;
		}
	}
	if(caster_index == -1) return BattleEvent{}; // Caster isn't part of this battle.

	// Determine which ability tier should be used in the preview
	int tier = DetermineAbilityTier(intent.caster, intent.ability);
	if(tier < 0) return BattleEvent{}; // The caster can't use the ability, so there is no effect to preview.
	const AbilityTier *cur_ability_tier = &intent.ability->tiers[tier];

	BattleEvent event = {};
	event.battle = battle;

	// For each effect in cur_ability_tier, update the event based on what type of effect it is.
	for(int i=0; i<ArrayCount(cur_ability_tier->effects); i++)
	{
		Effect effect = cur_ability_tier->effects[i];

		if(effect.type == EffectType::NoEffect)
		{
			continue;
		}
		else if(effect.type == EffectType::Damage)
		{
			EffectParams_Damage *effect_params = (EffectParams_Damage*)effect.params;

			// Iterate over each unit and change the corresponding event.trait_changes
			for(int i=0; i<intent.targets.size; i++)
			{
				Unit *target_unit = intent.targets[i];
				event.trait_changes[i] += CalculateAdjustedDamage(target_unit->cur_traits, effect_params->amount);
			}
		}
		else if(effect.type == EffectType::DamageIgnoreArmor)
		{
			EffectParams_DamageIgnoreArmor *effect_params = (EffectParams_DamageIgnoreArmor*)effect.params;

			// Iterate over each unit and change the corresponding event.trait_changes
			for(int i=0; i<intent.targets.size; i++)
			{
				Unit *target_unit = intent.targets[i];

				TraitSet cur_traits_with_no_armor = target_unit->cur_traits;
				cur_traits_with_no_armor.armor = 0;
				event.trait_changes[i] += CalculateAdjustedDamage(cur_traits_with_no_armor, effect_params->amount);
			}
		}
		else if(effect.type == EffectType::DamageIgnoreArmor)
		{
			EffectParams_Restore *effect_params = (EffectParams_Restore*)effect.params;

			// Iterate over each unit and change the corresponding event.trait_changes
			for(int i=0; i<intent.targets.size; i++)
			{
				Unit *target_unit = intent.targets[i];
				event.trait_changes[i] += effect_params->amount;
			}
		}
		else if(effect.type == EffectType::Gift)
		{
			EffectParams_Gift *effect_params = (EffectParams_Gift*)effect.params;
			TraitSet base_gift_amount = effect_params->amount;

			// Iterate over each unit and change the corresponding event.trait_changes
			for(int i=0; i<intent.targets.size; i++)
			{
				Unit *target_unit = intent.targets[i];
				// Adjust gift amount such that you can only gift as much trait as you actually have.
				// e.g., Gifting 5 armor when you only have 3 armor will only give the target +3 armor and the caster -3 armor
				TraitSet adjusted_gift_amount = {};
				for(int i=0; i<c::trait_count; i++)
				{
					adjusted_gift_amount[i] = m::Min(intent.caster->cur_traits[i], base_gift_amount[i]);
				}

				event.trait_changes[caster_index] -= adjusted_gift_amount;
				event.trait_changes[i] += adjusted_gift_amount;
			}
		}
		else if(effect.type == EffectType::Steal)
		{
			EffectParams_Steal *effect_params = (EffectParams_Steal*)effect.params;
			TraitSet base_steal_amount = effect_params->amount;

			// Iterate over each unit and change the corresponding event.trait_changes
			for(int i=0; i<intent.targets.size; i++)
			{
				Unit *target_unit = intent.targets[i];
				// Adjust steal amount such that you can only steal as much trait as the target actually has.
				// e.g., Stealing 5 armor from a target with only 3 armor will only give the caster +3 armor and the target -3 armor
				TraitSet adjusted_steal_amount = {};
				for(int i=0; i<c::trait_count; i++)
				{
					adjusted_steal_amount[i] = m::Min(target_unit->cur_traits[i], base_steal_amount[i]);
				}

				event.trait_changes[caster_index] += adjusted_steal_amount;
				event.trait_changes[i] -= adjusted_steal_amount;
			}
		}
	}

	return event;
}

void
ApplyBattleEvent(const BattleEvent *event)
{
	for(int i=0; i<ArrayCount(event->battle->units); i++)
	{
		if(!ValidUnit(event->battle->units[i])) continue;

		event->battle->units[i]->cur_traits += event->trait_changes[i];
	}
}

void
DrawUnits(Battle *battle)
{
	BattleEvent preview_event = {};
	if(battle->show_action_preview)
	{
		// Generate preview for: a particular caster using a particular ability on a particular target set.
		GenerateBattlePreviewEvent(battle, battle->previewed_intent);
	}

	for(int i=0; i<c::max_target_count; i++)
	{
		Unit *unit = battle->units[i];
		if(unit == nullptr) continue;
		if(!unit->init) continue;

		// Change the outline color for selected and hovered units
		Color outline_color = c::black;
		// if(unit == battle->selected_unit)
		// {
		// 	outline_color = c::green;
		// }
		// else if(unit == battle->hovered_unit)
		// {
		// 	outline_color = c::grey;
		// }

		Vec2f origin = battle->unit_slots[i];

		// Draw unit slot outline
		DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

		// Draw unit name
		TextLayout unit_name_layout = c::def_text_layout;
		unit_name_layout.font_size = 32;
		unit_name_layout.align = c::align_topcenter;
		Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name);

		//TraitSet preview_traits = unit->cur_traits;

		// Draw trait bars
		DrawTraitSetWithPreview(origin + Vec2f{0.f, name_size.y},
								unit->cur_traits,
								unit->max_traits,
								unit->cur_traits+preview_event.trait_changes[i],
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
	// if(battle->selected_ability and UnitInTargetSet(battle->hovered_unit, battle->selected_ability_valid_target_set))
	// {
	// 	// 1) An ability is selected AND a valid target for that ability is hovered => draw inferred target set and outcome if that ability were to be used.
	// 	target_set = battle->inferred_target_set;
	// 	target_indication_layout.color = c::red;

	// 	battle->previewed_intent = {battle->selected_unit, battle->selected_ability, target_set};
	// 	battle->show_action_preview = true;
	// }
	// else
	// {
	// 	if(battle->hovered_ability and battle->hovered_ability != battle->selected_ability)
	// 	{
	// 		// 2) An ability button is being hovered => draw valid targets for hovered ability
	// 		target_set = battle->hovered_ability_valid_target_set;
	// 		target_indication_layout.color = c::yellow;
	// 	}
	// 	else if(battle->selected_ability)
	// 	{
	// 		// 3) An ability is selected => draw valid targets for selected ability
	// 		target_set = battle->selected_ability_valid_target_set;
	// 		target_indication_layout.color = c::orange;
	// 	}
	// 	else
	// 	{
	// 		// There is no hovered nor selected ability, so there's no targeting
	// 		// info to draw.
	// 		return;
	// 	}
	// }

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
AllBattleUnitsAsTargetSet(Battle *battle)
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
	ImguiContainer container = c::def_ui_container;
	container.pos = battle->hud.pos + c::hud_ability_buttons_offset;
	SetActiveContainer(&container);

	for(int i=0; i<c::moveset_max_size; i++)
	{
		Ability *ability = &unit->abilities[i];
		if(!ability or !ability->init) continue;

		Rect button_rect = GetAbilityHudButtonRect(*battle, i);

		// if(battle->selected_ability == ability)
		// {
		// 	// This ability button corresponds to the selected ability
		// 	DrawButton(c::selected_ability_button_layout, button_rect, "%s", ability->name);
		// }
		// else if(ability == battle->hovered_ability)
		// {
		// 	// This ability button is being hovered
		// 	DrawButton(c::hovered_ability_button_layout, button_rect, "%s", ability->name);

		// 	if(Pressed(vk::LMB) and battle->selected_unit->cur_action_points > 0)
		// 	{
		// 		battle->selected_ability = ability;
		// 	}
		// }
		// else
		// {
		// 	// This ability button isn't hovered or selected, so just draw it normally
		// 	DrawButton(c::ability_button_layout, button_rect, "%s", ability->name);
		// }
	}
}

// void
// GenerateEnemyIntents(Battle *battle)
// {
// 	TIMED_BLOCK;

// 	// Choose enemy abilities randomly and perform them
// 	for(int i=0; i<c::max_target_count; i++)
// 	{
// 		Unit *unit = battle->units[i];
// 		if(!unit or !unit->init) continue; // Skip non-existent units
// 		if(unit->team != Team::enemies) continue; // Skip ally units

// 		u32 chosen_ability_index; // The index of the ability chosen to cast for this enemy
// 		u32 chosen_target_index; // The index (into battle->units[])
// 		TargetSet all_targets = AllBattleUnitsAsTargetSet(battle);

// 		int valid_ability_count = 0; // Number of valid abilities
// 		int valid_ability_indices[c::moveset_max_size] = {}; // Indices of abilities that are initialized.
// 		TargetSet valid_target_sets[c::moveset_max_size] = {}; // Valid target sets corresponding to valid_ability_indices
// 		for(int i=0; i<c::moveset_max_size; i++)
// 		{
// 			Ability *ability = &unit->abilities[i];
// 			if(!ability->init) continue;

// 			TargetSet valid_target_set = GenerateValidTargetSet(unit, ability, all_targets);
// 			if(valid_target_set.size > 0)
// 			{
// 				valid_ability_indices[valid_ability_count] = i;
// 				valid_target_sets[i] = valid_target_set;
// 				++valid_ability_count;
// 			}

// 		}

// 		if(valid_ability_count <= 0) continue;

// 		chosen_ability_index = valid_ability_indices[RandomU32(0, valid_ability_count-1)];
// 		Ability *chosen_ability = &unit->abilities[chosen_ability_index];
// 		TargetSet chosen_ability_valid_targets = valid_target_sets[chosen_ability_index];


// 		chosen_target_index = RandomU32(0, chosen_ability_valid_targets.size-1);
// 		Unit *chosen_target = chosen_ability_valid_targets.units[chosen_target_index];

// 		TargetSet inferred_targets = GenerateInferredTargetSet(unit, chosen_target, chosen_ability, all_targets);

// 		battle->intents[i] = {unit, chosen_ability, inferred_targets};

// 		//ApplyAbilityToTargetSet(unit, *chosen_ability, inferred_targets);
// 	}
// }

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

//	GenerateEnemyIntents(battle);

	// Set to player turn
	battle->is_player_turn = true;
}

void
UpdateHoveredUnit(Battle *battle)
{
	// for(int i=0; i<c::max_target_count; i++)
	// {
	// 	if(PointInRect(Rect{battle->unit_slots[i], c::unit_slot_size}, MousePos()))
	// 	{
	// 		if(!battle->units[i] or !battle->units[i]->init) continue;
	// 		battle->hovered_unit = battle->units[i];
	// 		break;
	// 	}
	// }
}

void
UpdateHoveredAbility(Battle *battle)
{
	// if(!battle->selected_unit) return; // No selected unit implies no ability buttons to be hovered.

	// for(int i=0; i<c::moveset_max_size; i++)
	// {
	// 	Ability *ability = &battle->selected_unit->abilities[i];
	// 	if(!ability or !ability->init) continue;

	// 	Rect ability_button_rect = GetAbilityHudButtonRect(*battle, i);
	// 	if(PointInRect(ability_button_rect, MousePos()))
	// 	{
	// 		battle->hovered_ability = ability;
	// 		return;
	// 	}
	// }
}

void
DrawAbilityInfoBox(Vec2f pos, const Ability *ability, Align align = c::align_topleft)
{
	if(!ValidAbility(ability)) return;

	Rect infobox_aligned_rect = AlignRect({pos, c::ability_info_box_size}, align);
	Vec2f pen = infobox_aligned_rect.pos;
	DrawFilledRect(infobox_aligned_rect, c::ability_info_bg_color);
	DrawUnfilledRect(infobox_aligned_rect, c::white);

	TextLayout layout = c::def_text_layout;
	layout.align = c::align_topleft;
	Vec2f name_text_size = DrawText(layout, RectTopLeft(infobox_aligned_rect), ability->name);
	//pen.y += name_size.y;


	// layout.font_size = 16;
	// layout.align = c::align_rightcenter;
	// Vec2f target_text_size = DrawText(layout, RectTopRight(infobox_aligned_rect) + Vec2f{0.f, 0.5f*name_text_size.y},
	// 		 						  "targets %s", TargetClass_userstrings[(int)ability->target_class]);

	pen.y += name_text_size.y;
	DrawLine(pen, pen + Vec2f{c::ability_info_box_size.x, 0.f});

	for(int i=0; i<ArrayCount(ability->tiers); i++)
	{
		pen.y += DrawText(c::small_text_layout, pen,
						  "%d: %s",
						  i, GenerateAbilityTierText(&ability->tiers[i])).y;
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
	{ // Process hotkey input
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
				battle->selected_ability;
			}
		}

		// Ability selection (for currently selected unit) with QWER keys
		if(ValidUnit(battle->selected_unit) and battle->selected_unit->cur_action_points > 0)
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
	}

	Ability *hovered_ability = nullptr;
	{ // Draw HUD and update hovered_ability and selected_ability
		if(ValidUnit(battle->selected_unit))
		{ // Only draw HUD if there's a valid selected unit.

			Vec2f pen = battle->hud.pos;
			SetDrawDepth(c::hud_draw_depth);

			// Draw HUD backdrop and border
			DrawFilledRect(battle->hud, c::dk_grey);
			DrawLine(pen, pen+Vec2f{battle->hud.size.x, 0.f}, c::white);

			// Draw unit name
			pen += c::hud_unit_name_offset;
			pen.y += DrawText(c::def_text_layout, pen, battle->selected_unit->name).y;

			// Draw unit traits
			pen.y += DrawText(c::def_text_layout, pen, "Vigor: %d/%d",
									battle->selected_unit->cur_traits.vigor, battle->selected_unit->max_traits.vigor).y;
			pen.y += DrawText(c::def_text_layout, pen, "Focus: %d/%d",
									battle->selected_unit->cur_traits.focus, battle->selected_unit->max_traits.focus).y;
			pen.y += DrawText(c::def_text_layout, pen, "Armor: %d/%d",
									battle->selected_unit->cur_traits.armor, battle->selected_unit->max_traits.armor).y;



			// Determine hovered ability and draw ability buttons simultaneously.
			for(int i=0; i<ArrayCount(battle->selected_unit->abilities); i++)
			{ // For each ability of selected_unit

				// Skip invalid abilities.
				Ability *ability = &battle->selected_unit->abilities[i];
				if(!ValidAbility(ability)) continue;

				Rect ability_button_rect = GetAbilityHudButtonRect(*battle, i);

				// Set this ability as the hovered ability if the mouse is inside its button rect.
				if(PointInRect(ability_button_rect, MousePos()))
				{
					hovered_ability = ability;
				}

				// Draw the ability button differently depending on whether it's selected or not.
				if(battle->selected_ability == ability)
				{ // Button is selected
					DrawButton(c::selected_ability_button_layout, ability_button_rect, "%s", ability->name);
				}
				else
				{ // Button is not selected -- it's either hovered or not hovered.
					// I could process the button clicking here, but for now I'm deferring it to later,
					// where I'll check all the LMB click conditions sort of all as a contained group.
					DrawButton(c::unselected_ability_button_layout, ability_button_rect, "%s", ability->name);
				}
			}

			// Draw the HUD's ability info box.
			// Order of priority:
			// 1) If there is a hovered ability, draw its info box. If not...
			// 2) If there is a selected ability, draw its info box. If not...
			// 3) Don't draw an ability info box.
			if(hovered_ability)
			{
				DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset, hovered_ability, c::align_topleft);
			}
			else if(battle->selected_ability)
			{
				DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset, battle->selected_ability, c::align_topleft);
			}
		}
	}

	Unit *hovered_unit = nullptr;
	{ // Update hovered_unit
		for(int i=0; i<c::max_target_count; i++)
		{
			if(PointInRect(Rect{battle->unit_slots[i], c::unit_slot_size}, MousePos()))
			{
				if(ValidUnit(battle->units[i]))
				{
					hovered_unit = battle->units[i];
				}

				break;
			}
		}
	}

	TargetSet hovered_ability_valid_target_set = {};
	TargetSet selected_ability_valid_target_set = {};
	{ // Update valid target sets for hovered_ability and selected_ability if they exist.
		TargetSet all_units = AllBattleUnitsAsTargetSet(battle);
//		selected_ability_valid_target_set = GenerateValidTargetSet(battle->selected_unit, battle->selected_ability, all_units);
//		hovered_ability_valid_target_set = GenerateValidTargetSet(battle->selected_unit, hovered_ability, all_units);
	}

	Intent player_intent = {};
	{ // Generate player_intent
		// (If there is no unit hovered or the hovered unit is an invalid target for the selected_ability,
		// the intent target list will be empty)

		TargetSet all_units = AllBattleUnitsAsTargetSet(battle);
		player_intent.caster = battle->selected_unit;
		player_intent.ability = battle->selected_ability;
		// player_intent.targets = GenerateInferredTargetSet(battle->selected_unit, hovered_unit,
		// 												  battle->selected_ability, all_units);
	}

	{ // Draw end turn button.
		if(battle->is_player_turn)
		{
			if(battle->ending_player_turn)
			{ // It's the player's turn and the player turn is currently ending
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
		{ // It's not the player turn, so just grey out the end turn button.
			DrawButton(c::end_button_disabled_layout, c::end_turn_button_rect, "End Turn");
		}
	}

	{ // Process player mouse input
		if(Pressed(vk::LMB))
		{
			// @note: Strictly speaking, these mostly probably don't need to be else ifs, but
			// I probably don't want one mouse click to be able to perform more than one kind of action sequence,
			// so I think it's better just to be on the safe side even though they should probably be
			// orthogonal cases if my game state is correct. Either way, else ifs make it a bit
			// more performant anyway since we don't check as many conditions.
			//
			// For example, it might be possible to hover both an ability button and a unit slot
			// at the same time -- it's not something I've actually checked against, so we're probably
			// better off here just using the else ifs and giving some kind of priority to certain actions.
			//
			// At some point, it might be a good idea to do something like check if the mouse is inside the HUD,
			// and if it is, only check for mouse click events that are relevant for HUD interaction, in the case
			// there is some weird thing where you're clicking through the HUD on some hidden element and doing
			// something unintended.

			if(ValidAbility(hovered_ability) and ValidUnit(battle->selected_unit) and battle->selected_unit->cur_action_points > 0)
			{
				battle->selected_ability = hovered_ability;
			}
			else if(ValidUnit(hovered_unit) and !battle->selected_ability)
			{ // If the player clicked on a valid unit and no ability was selected, select the clicked unit.
				// @note: I'm intuitively sensing a bit of execution order weirdness here where you have to be careful
				// with changing the selected_unit with left click, but I think it's ok if there is no selected ability.
				// Just leaving this note here to remind me if I notice something strange going on later.
				battle->selected_unit = hovered_unit;
			}
			else if(ValidUnit(battle->selected_unit) and battle->selected_unit->cur_action_points > 0 and player_intent.targets.size > 0)
			{ // Execute the player intent if a valid target is clicked and the selected unit has enough action points.

				// Generate the event associated with the player intent and execute it.
				BattleEvent event = GenerateBattlePreviewEvent(battle, player_intent);
				ApplyBattleEvent(&event);

				// After executing the player intent, remove AP, clear selected ability,
				// clear related target sets, and clear player intent.
				battle->selected_unit->cur_action_points -= 1;
				battle->selected_ability = {};
				selected_ability_valid_target_set = {};
				player_intent = {};
			}
		}
	}

	// Reset these and newly generate them each frame.
	{
		battle->show_action_preview = false;
	}

	//Tick(&battle->preview_damage_timer);




	// 	// draws:
	// 	//		"TARGET" over unit slots contextually
	// 	//		(yellow for hovered ability, orange for selected ability, red for targeting ability)
	// 	// may change:
	// 	//		show_action_preview
	// 	DrawTargetingInfo(battle);
	// }

	// if(battle->ending_player_turn and Tick(&battle->end_button_clicked_timer))
	// {
	// 	if(battle->is_player_turn)
	// 	{
	// 		battle->ending_player_turn = false;
	// 		battle->is_player_turn = false;
	// 	}
	// }

	// // Draw end-turn button



	// if(!battle->is_player_turn)
	// {
	// 	TargetSet all_targets = AllBattleUnitsAsTargetSet(battle);

	// 	for(int i=0; i<c::max_target_count; i++)
	// 	{
	// 		Intent intent = battle->intents[i];
	// 		if(!ValidUnit(intent.caster) or !ValidAbility(intent.ability)) continue;

	// 		BattleEvent event = GenerateBattlePreviewEvent(battle, intent);
	// 		ApplyBattleEvent(&event);

	// //			ApplyAbilityToTargetSet(caster, *intent.ability, intent.targets);
	// 	}

	// 	battle->is_player_turn = true;
	// 	for(Unit *unit : battle->units)
	// 	{
	// 		if(!unit or !unit->init) continue;
	// 		unit->cur_action_points = unit->max_action_points;
	// 	}

	// 	GenerateEnemyIntents(battle);
	// }

	// // Automatically end the turn if no ally unit has remaining action points.
	// {
	// 	bool any_ally_has_ap = false;
	// 	for(Unit *unit : battle->units)
	// 	{
	// 		if(!unit or !unit->init) continue;
	// 		//unit->cur_action_points = unit->max_action_points;
	// 		if(unit->team == Team::allies and unit->cur_action_points > 0) any_ally_has_ap = true;
	// 	}

	// 	if(!any_ally_has_ap) battle->ending_player_turn = true;
	// }

	// DrawEnemyIntents(battle);
	// // if(Down(vk::alt))
	// // {
	// // 	battle->show_action_preview = true;
	// // 	battle->previewed_intent = intent;
	// // }

	// if(!battle->show_action_preview) ResetLow(&battle->preview_damage_timer);

	// // draws:
	// //		unit slots (names, traitset bars, unit slot outline)
	// DrawUnits(battle);
}