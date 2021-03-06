#include "battle.h"

#include "draw.h"
#include "game.h"
#include "random.h"
#include "array.h"
#include "table.h"
#include "keybinds.h"

const char *BattlePhaseAsCString(BattlePhase battle_phase)
{
    return BattlePhase_userstrings[(int)battle_phase];
}

Rect
GetAbilityHudButtonRect(Battle battle, int ability_index)
{
    if(ability_index < 0 or ability_index > c::moveset_max_size)
    {
        Log("(" __FUNCTION__ ") received invalid ability_index");
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
ValidBreed(const Breed *breed)
{
    return(breed and breed->init);
}

// Returns a UnitSet containing all units (as a subset of [all_units]) that are considered
// a valid selected unit for the given target class [tc] for a caster specified by [caster_id]
void
ValidSelectionUnitSet(Id<Unit> caster_id, TargetClass tc, Array<UnitId> all_units, Array<UnitId> *valid_selection_units)
{
    Unit *caster = GetUnitFromId(caster_id);
    if(!ValidUnit(caster)) return;

    for(Id target_id : all_units)
    {
        if(!CheckValidTarget(caster_id, target_id, tc)) continue;

        *valid_selection_units += target_id;
    }
}

// For the given [intent], determine all events that would occur if the intent
// would be executed, and add those events to [*events]
void
GenerateEventsFromIntent(Intent intent, Array<BattleEvent> *events, Table<Unit> unit_table)
{
    auto caster_id = intent.caster_id;
    auto ability_id = intent.ability_id;

    // Check that the caster and ability in intent are valid.
    Unit *caster = GetUnitFromId(caster_id, unit_table);
    Ability *ability = GetAbilityFromId(ability_id);
    { // Check caster and ability are valid
        if(!caster)
        {
            VerboseError(__FUNCTION__ "() received an intent with an invalid caster.");
            return;
        }
        if(!ability)
        {
            VerboseError(__FUNCTION__ "() received an intent with an invalid ability.");
            return;
        }
    }

    if(caster->cur_traits.vigor <= 0) return; // Don't generate events if caster is dead.

    // Determine which ability tier should be used in the preview
    int tier = DetermineAbilityTier(caster_id, ability_id);
    if(tier < 0) return; // The caster can't use the ability, so there is no effect to preview.
    AbilityTier *cur_ability_tier = &ability->tiers[tier]; // alias

    //ClearArray(battle->preview_events);
    //auto events = CreateTempArray<BattleEvent>(events_initial_size);

    bool first_effect = true;
    int target_set_index = 0; // index into intent.target_sets
    //TargetClass cur_target_class = {};
    // Create an event in [events] for each effect in the current ability tier.
    for(auto effect : cur_ability_tier->effects_)
    {
        for(Id<Unit> target_id : intent.target_set)
        {
            Unit *target = GetUnitFromId(target_id, unit_table);
            if(!target) continue;

            TraitSet trait_changes = {};

            if(effect.type == EffectType::NoEffect)
            {
                continue;
            }
            else if(effect.type == EffectType::Damage)
            {
                EffectParams_Damage *effect_params = (EffectParams_Damage*)effect.params;

                trait_changes = CalculateAdjustedDamage(target->cur_traits, effect_params->amount);
            }
            else if(effect.type == EffectType::DamageIgnoreArmor)
            {
                EffectParams_DamageIgnoreArmor *effect_params = (EffectParams_DamageIgnoreArmor*)effect.params;

                TraitSet cur_traits_with_no_armor = target->cur_traits;
                cur_traits_with_no_armor.armor = 0;
                trait_changes = CalculateAdjustedDamage(cur_traits_with_no_armor, effect_params->amount);
            }
            else if(effect.type == EffectType::Gift)
            {
                EffectParams_Gift *effect_params = (EffectParams_Gift*)effect.params;
                TraitSet base_gift_amount = effect_params->amount;

                // Adjust gift amount such that you can only gift as much trait as you actually have.
                // e.g., Gifting 5 armor when you only have 3 armor will only give the target +3 armor and the caster -3 armor
                for(int i=0; i<c::trait_count; i++)
                {
                    trait_changes[i] = m::Min(caster->cur_traits[i], base_gift_amount[i]);
                }

                // Caster change
                *events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
            }
            else if(effect.type == EffectType::Steal)
            {
                EffectParams_Steal *effect_params = (EffectParams_Steal*)effect.params;
                TraitSet base_steal_amount = effect_params->amount;

                // Adjust steal amount such that you can only steal as much trait as the target actually has.
                // e.g., Stealing 5 armor from a target with only 3 armor will only give the caster +3 armor and the target -3 armor
                for(int i=0; i<c::trait_count; i++)
                {
                    trait_changes[i] = -m::Min(target->cur_traits[i], base_steal_amount[i]);
                }

                // Caster change
                *events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
            }
            else if(effect.type == EffectType::Restore)
            {
                EffectParams_Restore *effect_params = (EffectParams_Restore*)effect.params;
                trait_changes = effect_params->amount;
            }
            else
            {
                if(c::verbose_error_logging) Log("Encountered unimplemented effect type. ((int)EffectType => %d)", int(effect.type));
            }

            // Don't add the event if its trait changes would have no net effect.
            // @note: Might change this later, if we want to track these kinds of things
            //        for the purpose of triggers (e.g., damage triggers, even when damage=0)
            if(trait_changes != TraitSet{})
            {
                *events += BattleEvent{.caster_id=caster_id, .target_id=target_id, .trait_changes=trait_changes};
            }

        }
    }
}

void
IntentTraitChangesForUnits(Array<Intent> intents, Array<UnitId> unit_ids, Array<TraitSet> *traitset_changes)
{ TIMED_BLOCK;
    g::temp_unit_table.entry_count = g::unit_table.entry_count;
    // Copy unit table into temp unit table
    for(int i=0; i<g::unit_table.entry_count; ++i)
    {
        g::temp_unit_table.entries[i] = g::unit_table.entries[i];
    }

    for(auto intent : intents)
    {
        Unit *temp_caster = GetUnitFromId(intent.caster_id, g::temp_unit_table);
        if(!ValidUnit(temp_caster)) continue;

        Ability *ability = GetAbilityFromId(intent.ability_id);
        if(!ValidAbility(ability)) continue;

        int tier_index = -1;
        for(int i=ability->tiers.count-1; i>=0; --i)
        {
            if(temp_caster->cur_traits >= ability->tiers[i].required_traits)
            {
                tier_index = i;
                break;
            }
        }

        if(tier_index <= 0) continue;

        auto &ability_tier = ability->tiers[tier_index]; // alias

        Array<BattleEvent> events = CreateTempArray<BattleEvent>(10);
        GenerateEventsFromIntent(intent, &events, g::temp_unit_table);

        for(auto event : events)
        {
            Unit *temp_target = GetUnitFromId(event.target_id, g::temp_unit_table);
            if(!ValidUnit(temp_target));

            temp_target->cur_traits += event.trait_changes;
        }
    }

    ClearArray(traitset_changes);
    for(auto unit_id : unit_ids)
    {
        Unit *unit_before_change = GetUnitFromId(unit_id, g::unit_table);
        Unit *unit_after_change =  GetUnitFromId(unit_id, g::temp_unit_table);

        TraitSet *traitset = Append(traitset_changes, TraitSet{});

        if(!ValidUnit(unit_before_change) or !ValidUnit(unit_after_change)) continue;

        *traitset = unit_after_change->cur_traits - unit_before_change->cur_traits;
    }
}

/*
void
DrawUnitSet(Battle *battle)
{
    for(int i=0; i<battle->units.size; ++i)
    {
        Id<Unit> unit_id = battle->units.ids[i];
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        // Change the outline color for selected and hovered units
        Color outline_color = c::black;

        Vec2f origin = battle->unit_slots[i];

        // Draw unit slot outline
        DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

        // Draw unit name
        TextLayout unit_name_layout = c::def_text_layout;
        unit_name_layout.font_size = 24;
        unit_name_layout.align = c::align_topcenter;
        Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name).rect.size;

        //TraitSet preview_traits = unit->cur_traits;

        TraitSet preview_trait_changes = {};
        if(battle->show_preview)
        {
            preview_trait_changes = EventTraitChangesForUnit(battle->preview_events, unit_id);
        }

        // Draw trait bars
        DrawTraitSetWithPreview(origin + Vec2f{0.f, name_size.y},
                                unit->cur_traits,
                                unit->max_traits,
                                unit->cur_traits + preview_trait_changes,
                                battle->preview_damage_timer.cur);

        DrawText(c::action_points_text_layout, origin + c::action_points_text_offset,
                 "AP: %d", unit->cur_action_points);
    }
}
*/

void
DrawAbilityInfoBox(Vec2f pos, Id<Unit> caster_id, Id<Ability> ability_id, Align align)
{
    SetDrawDepth(1.f);

    Ability *ability = GetAbilityFromId(ability_id);
    if(!ValidAbility(ability)) return;
    Unit *caster = GetUnitFromId(caster_id);
    if(!ValidUnit(caster)) return;

    int active_tier_level = DetermineAbilityTier(caster_id, ability_id);

    TextLayout name_layout = c::def_text_layout;
    name_layout.align = c::align_topcenter;
    TextLayout tier_inactive_text_layout = c::small_text_layout;
    tier_inactive_text_layout.color = c::lt_grey;
    TextLayout tier_active_text_layout = c::small_text_layout;
    tier_active_text_layout.color = c::white;

    Vec2f text_x_padding = {100.f, 0.f}; // The left-padding for where the text should start (to make room for req. bars)
    float name_line_height = LineHeight(name_layout);
    float tier_line_height = LineHeight(tier_active_text_layout);
    float tier_box_height = 2*tier_line_height + 2*c::tier_data_y_half_padding;

    int tier_count = ability->tiers.count-1; // -1 because this includes the "empty" tier
    float ability_box_height = tier_count*tier_box_height + name_line_height;

    Rect infobox_aligned_rect = AlignRect({pos, {c::ability_info_box_size.x, ability_box_height}}, align);
    Vec2f pen = infobox_aligned_rect.pos;
    DrawFilledRect(infobox_aligned_rect, c::ability_info_bg_color);
    DrawUnfilledRect(infobox_aligned_rect, c::white);

    DrawText(name_layout, RectTopCenter(infobox_aligned_rect), ability->name);
    pen.y += name_line_height;
    //pen.y += name_size.y;


    // layout.font_size = 16;
    // layout.align = c::align_rightcenter;
    // Vec2f target_text_size = DrawText(layout, RectTopRight(infobox_aligned_rect) + Vec2f{0.f, 0.5f*name_text_size.y},
    //                                "targets %s", TargetClass_userstrings[(int)ability->target_class]);

    //pen.y += name_text_size.y;
    DrawLine(pen, pen + Vec2f{c::ability_info_box_size.x, 0.f});

    Vec2f req_bar_origin = pen;

    pen.y += c::tier_data_y_half_padding; // Start padding of first tier
    for(int i=tier_count; i>=1; --i) // Iterate backwards, from highest tier to lowest, so that the lowest tier is drawn at the bottom
    {
        AbilityTier &tier = ability->tiers[i]; // alias

        String tier_requirements_text = AllocStringDataFromArena(500, memory::per_frame_arena_id);
        AppendCString(&tier_requirements_text, "Tier %d", i);
        bool first_trait_printed = false;
        if(tier.required_traits.vigor > 0)
        {
            if(first_trait_printed)
            {
                AppendCString(&tier_requirements_text, ", ");
            }
            else
            {
                first_trait_printed = true;
                AppendCString(&tier_requirements_text, " (req ");
            }

            AppendCString(&tier_requirements_text, "`red`%dV", tier.required_traits.vigor);
        }
        if(tier.required_traits.focus > 0)
        {
            if(first_trait_printed)
            {
                AppendCString(&tier_requirements_text, ", ");
            }
            else
            {
                first_trait_printed = true;
                AppendCString(&tier_requirements_text, " (req ");
            }

            AppendCString(&tier_requirements_text, "`lt_blue`%dF", tier.required_traits.focus);
        }
        if(tier.required_traits.armor > 0)
        {
            if(first_trait_printed)
            {
                AppendCString(&tier_requirements_text, ", ");
            }
            else
            {
                first_trait_printed = true;
                AppendCString(&tier_requirements_text, " (req ");
            }

            AppendCString(&tier_requirements_text, "`gold`%dA", tier.required_traits.armor);
        }

        if(first_trait_printed)
        {
            AppendCString(&tier_requirements_text, "`reset`)");
        }

        AppendCString(&tier_requirements_text, ":\n    %s", GenerateAbilityTierText(tier));

        TextLayout layout = tier_inactive_text_layout;
        if(i == active_tier_level)
            layout = tier_active_text_layout;

        pen.y += DrawTextMultiline(layout, pen + text_x_padding, tier_requirements_text).y;

        #if 0
        {
            pen.y += DrawTextMultiline(tier_active_text_layout, pen,
                                       "Tier %d (req `red`%dV, `lt_blue`%dF, `gold`%dA`reset`):\n    %s",
                                       i,
                                       tier.required_traits.vigor,
                                       tier.required_traits.focus,
                                       tier.required_traits.armor,
                                       GenerateAbilityTierText(tier)).y;
        }
        else
        {
            pen.y += DrawTextMultiline(tier_inactive_text_layout, pen,
                                       "Tier %d (req `red`%dV, `lt_blue`%dF, `gold`%dA`reset`):\n    %s",
                                       i,
                                       tier.required_traits.vigor,
                                       tier.required_traits.focus,
                                       tier.required_traits.armor,
                                       GenerateAbilityTierText(tier)).y;
        }
        #endif

        AbilityTier &prev_tier = ability->tiers[i-1]; // alias
        TraitSet trait_ranges = tier.required_traits - prev_tier.required_traits;
        TraitSet cur_traits_in_range = caster->cur_traits - prev_tier.required_traits; // How many pips to color in within current range
        for(int i=0; i<c::trait_count; ++i)
        { // clamp between 0 and trait_range
            cur_traits_in_range[i] = m::Clamp(cur_traits_in_range[i], 0, trait_ranges[i]);
        }

        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{ 0.f,0.f}, {20.f,tier_box_height}}, c::vigor_color, c::bg_vigor_color, cur_traits_in_range.vigor, trait_ranges.vigor);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{20.f,0.f}, {20.f,tier_box_height}}, c::focus_color, c::bg_focus_color, cur_traits_in_range.focus, trait_ranges.focus);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{40.f,0.f}, {20.f,tier_box_height}}, c::armor_color, c::bg_armor_color, cur_traits_in_range.armor, trait_ranges.armor);

        pen.y += c::tier_data_y_half_padding;
        req_bar_origin = pen;
        DrawLine({pen.x, pen.y}, {pen.x + c::ability_info_box_size.x, pen.y}, c::grey);
        pen.y += c::tier_data_y_half_padding;
    }
}

// @TODO:
// There's maybe some kind of coupling issue here because this method both draws the enemy intent
// boxes above the enemies' heads, but also adds events to the [preview_events] array. Because of this,
// we can't really factor this code easily to draw player "thought bubbles" as well -- although,
// fundamentally the thought bubbles for allies and enemies are kind of different, right? The enemy
// ones show which ability the enemy intends to use, but the ally ones really just want to show how
// current ability tier levels, how effects are changing ability tiers, how much each ability needs to
// be tiered up, and things like that (although enemy thought bubbles probably want to show that as well)
//
// So... the primary difference between ally and enemy thought bubbles is that enemy ones need to be able
// display enemy intent, and (at the moment because of this coupling issue) they need to generate entries
// in the [preview_events] array. If we can decouple these things a bit, then we might be able to get a bit
// of code reuse for drawing the thought bubbles, and do that separate stuff for enemy intents in some
// decoupled code.

void
DrawEnemyIntentThoughtBubbles(Battle *battle)
{
    for(UnitId caster_id : battle->unit_slots)
    {
        Unit *caster = GetUnitFromId(caster_id);
        if(!ValidUnit(caster) or caster->team != Team::enemies or caster->cur_traits.vigor <= 0) continue;

        Intent intent = caster->intent;
        if(intent.caster_id != caster_id)
        {
            VerboseError(__FUNCTION__ "(): intent.caster_id != caster_id when getting data from the parallel battle->intents[] and battle->units arrays.");
            continue;
        }


        SetDrawDepth(c::field_draw_depth);
        Vec2f pen = battle->slot_positions[caster->battle_slot_index] + c::enemy_intent_offset;
        for(int ability_index=0; ability_index<ArrayCount(caster->ability_ids); ++ability_index)
        {
            Id ability_id = caster->ability_ids[ability_index];
            Rect ability_button_rect = {pen, c::ability_button_size};
            bool show_intent = (ability_id == intent.ability_id);
            auto response = DrawAbilityButton(ability_button_rect, caster_id, ability_id, show_intent, c::align_bottomcenter);


            // Ability *ability = GetAbilityFromId(ability_id);
            // if(!ValidAbility(ability)) continue;

            //int tier = DetermineAbilityTier(caster_id, ability_id);

            if(response.hovered and (Down(vk::alt) or Down(vk::LMB)))
            {
                DrawAbilityInfoBox(MousePos(), caster_id, ability_id, c::align_topleft);
            }

            pen.y -= (1 + c::ability_button_size.y); // 1 + so the button outlines don't overlap
        }
    }

    // if(ability_id != battle->last_frame_hovered_ability_id)
    // {
    //     ResetHigh(&battle->preview_damage_timer);
    // }
    //battle->last_frame_hovered_ability_id = ability_id;
}

void
SetSelectedAbility(Battle *battle, Id<Ability> new_ability_id)
{
    battle->selected_ability_id = new_ability_id;
    ResetHigh(&battle->preview_damage_timer);
}

void
DeselectSelectedUnit(Battle *battle)
{
    battle->selected_unit_id = c::null_unit_id;
    battle->selected_ability_id = c::null_ability_id;
}

void
SetSelectedUnit(Battle *battle, Id<Unit> new_unit_id)
{
    if(UnitIsDead(new_unit_id)) return;

    battle->selected_unit_id = new_unit_id;
    battle->selected_ability_id = c::null_ability_id;
}

void InitBattle(Battle *battle, PoolId<Arena> arena_id)
{
    // Memory allocation
    battle->arena_id = arena_id;
    ClearArena(arena_id);

    battle->unit_slots = CreateArrayFromArena<UnitId>(c::battle_unit_slot_count, battle->arena_id);
    FillArray(&battle->unit_slots, c::battle_unit_slot_count);

    battle->slot_positions = CreateArrayFromArena<Vec2f>(c::battle_unit_slot_count, battle->arena_id);
    FillArray(&battle->slot_positions, c::battle_unit_slot_count);

    battle->preview_intents = CreateArrayFromArena<Intent>(c::battle_unit_slot_count, battle->arena_id);

    battle->ai_arena_id = AllocArena("AI");

    // Fill out timers
    battle->preview_damage_timer = {
        .low = 0.3f,
        .high = 1.0f,
    };
    ResetHigh(&battle->preview_damage_timer);

    battle->end_player_turn_timer = {};
    battle->end_player_turn_timer.length_s = c::end_button_clicked_time_s;

    battle->hud = {{0.f, game->window_size.y-c::hud_offset_from_bottom}, {game->window_size.x, c::hud_offset_from_bottom}};
}

void
StartBattle(Battle *battle, Array<UnitId> battle_units)
{
    // Clear unit slots
    for(UnitId &id : battle->unit_slots)
        id = c::null_unit_id;

    // Fill unit slots with UnitIds from battle_units. Slots 0-3 are allies, 4-7 are enemies (so long as max_party_size is 4)
    int ally_count = 0;
    int enemy_count = 0;
    int first_ally_index = 0;
    int first_enemy_index = c::max_party_size;
    for(auto unit_id : battle_units)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        if(unit->team == Team::allies)
        {
            if(ally_count >= c::max_party_size)
            {
                VerboseError("Too many allies given in battle_units array parameter of StartBattle(). Maximum is %d.", c::max_party_size);
            }
            else
            {
                int slot_index = first_ally_index + ally_count;
                battle->unit_slots[slot_index] = unit_id;
                unit->battle_slot_index = slot_index;
                ++ally_count;
            }
        }
        else if(unit->team == Team::enemies)
        {
            if(enemy_count >= c::max_party_size)
            {
                VerboseError("Too many enemies given in battle_units array parameter of StartBattle(). Maximum is %d.", c::max_party_size);
            }
            else
            {
                int slot_index = first_enemy_index + enemy_count;
                battle->unit_slots[slot_index] = unit_id;
                unit->battle_slot_index = slot_index;
                ++enemy_count;
            }
        }
    }

    // Set action points=1 for all units
    for(auto unit_id : battle->unit_slots)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        unit->cur_action_points = unit->max_action_points;
    }

    // Enemy AI (# of permutations)
    Array<UnitId> ally_unitset = CreateTempArray<UnitId>(4);
    Array<UnitId> enemy_unitset = CreateTempArray<UnitId>(4);
    Array<UnitId> ordered_battle_units = CreateTempArray<UnitId>(8);
    for(auto unit_id : battle->unit_slots)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit) or UnitIsDead(unit_id)) continue;

        if(     unit->team == Team::allies)  ally_unitset += unit_id;
        else if(unit->team == Team::enemies) enemy_unitset += unit_id;
    }
    AppendArrayToArray(&ordered_battle_units, enemy_unitset);
    AppendArrayToArray(&ordered_battle_units, ally_unitset);
    battle->best_choice_string = DoAiStuff(enemy_unitset, ordered_battle_units, Team::enemies, battle->ai_arena_id);

    { // Generate unit slot positions
        float x_between_slots = c::unit_slot_size.x + c::unit_slot_padding;
        Vec2f pen = {50.f, 300.f};
        for(Vec2f &slot_pos : battle->slot_positions)
        {
            slot_pos = pen;
            pen.x += x_between_slots;
        }
    }

    battle->phase = BattlePhase::start;
}

BattleState
TickBattle(Battle *battle)
{
    if(Pressed(vk::f9))
    {
        int a = 0;
    }

    BattleState battle_state = {};

    if(battle->phase == BattlePhase::invalid)
    {
        VerboseError(__FUNCTION__ "() received [battle] with invalid phase.");
        battle_state.finished = true;
    }

    if(battle->phase == BattlePhase::start)
    { // Battle start stuff
        battle->phase = BattlePhase::player_turn;
    }

    ClearArray(&battle->preview_intents);

    bool mouse_in_hud = false; // @TODO: Implement this using the TakeMouseFocus() stuff.

    { // Process hotkey input
        // Ally unit selection with number keys (1-4)
        if(Pressed(KeyBind::SelectUnit1))
        {
            SetSelectedUnit(battle, battle->unit_slots[0]);
        }
        if(Pressed(KeyBind::SelectUnit2))
        {
            SetSelectedUnit(battle, battle->unit_slots[1]);
        }
        if(Pressed(KeyBind::SelectUnit3))
        {
            SetSelectedUnit(battle, battle->unit_slots[2]);
        }
        if(Pressed(KeyBind::SelectUnit4))
        {
            SetSelectedUnit(battle, battle->unit_slots[3]);
        }

        // Right click cancels selected ability if one is selected.
        // If no ability is selected, cancels selected unit if one is selected.
        // If neither a unit nor ability is selected, do nothing
        if(Pressed(KeyBind::Deselect))
        {
            if(battle->selected_ability_id != c::null_ability_id)
            {
                SetSelectedAbility(battle, c::null_ability_id);
            }
            else if(battle->selected_unit_id != c::null_unit_id)
            {
                DeselectSelectedUnit(battle);
            }
        }

        // Tab to go to next ally unit
        if(PressedOrRepeated(KeyBind::CycleUnits))
        {
            Unit *selected_unit = GetUnitFromId(battle->selected_unit_id);
            if(ValidUnit(selected_unit))
            { // If a unit is selected, search for the next filled slot, and newly select that unit
                int start_index = selected_unit->battle_slot_index;
                int cur_index = (start_index + 1) % c::battle_unit_slot_count;

                while(cur_index != start_index)
                {
                    UnitId new_selected_id = battle->unit_slots[cur_index];
                    Unit *new_selected_unit = GetUnitFromId(new_selected_id);
                    if(!ValidUnit(new_selected_unit)) continue; // Skip empty slots

                    battle->selected_unit_id = new_selected_id;
                }
            }
            else
            { // If no unit is selected, TAB selects the first unit
                //battle->selected_unit_id = battle->units[0];
                battle->selected_unit_id = battle->unit_slots[0];
            }

            // Find index into battle->units of currently selected unit
            // int selected_unit_index = -1;
            // for(int i=0; i<battle->units.count; i++)
            // {
            //     if(battle->units[i] == battle->selected_unit_id) selected_unit_index = i;
            // }

            // if(selected_unit_index == -1)
            // {
            //     // If no unit is selected, TAB selects the first unit
            //     battle->selected_unit_id = battle->units[0];
            // }
            // else
            // {
            //     // @note: this assumes that the first [max_party_size] slots are all ally units and that
            //     //        there are no other ally slots. This is probably fine for now, but if we go away
            //     //        from that model, this will be invalidated (which should be obvious if I use the
            //     //        tab functionality frequently)

            //     // Go to next unit, except when the last unit it selected; then loop back to the first unit.
            //     battle->selected_unit_id = battle->units[(selected_unit_index+1) % c::max_party_size];
            //     battle->selected_ability_id = {};
            // }
        }

        { // Ability selection (for currently selected unit) with QWER keys
            Unit *selected_unit = GetUnitFromId(battle->selected_unit_id);
            if(ValidUnit(selected_unit) and selected_unit->cur_action_points > 0)
            {
                if(Pressed(KeyBind::SelectAbility1))
                {
                    auto ability_id = selected_unit->ability_ids[0];
                    Ability *ability = GetAbilityFromId(ability_id);
                    if(ValidAbility(ability))
                    {
                        SetSelectedAbility(battle, ability_id);
                    }
                }
                else if(Pressed(KeyBind::SelectAbility2))
                {
                    auto ability_id = selected_unit->ability_ids[1];
                    Ability *ability = GetAbilityFromId(ability_id);
                    if(ValidAbility(ability))
                    {
                        SetSelectedAbility(battle, ability_id);
                    }
                }
                else if(Pressed(KeyBind::SelectAbility3))
                {
                    auto ability_id = selected_unit->ability_ids[2];
                    Ability *ability = GetAbilityFromId(ability_id);
                    if(ValidAbility(ability))
                    {
                        SetSelectedAbility(battle, ability_id);
                    }
                }
                else if(Pressed(KeyBind::SelectAbility4))
                {
                    auto ability_id = selected_unit->ability_ids[3];
                    Ability *ability = GetAbilityFromId(ability_id);
                    if(ValidAbility(ability))
                    {
                        SetSelectedAbility(battle, ability_id);
                    }
                }
            }
        }
    }

    Id<Ability> hovered_ability_id = c::null_ability_id;
    { // Draw HUD and update hovered_ability and selected_ability
        SetDrawDepth(c::hud_draw_depth);
        Unit *selected_unit = GetUnitFromId(battle->selected_unit_id);

        if(ValidUnit(selected_unit))
        { // Only draw HUD if there's a valid selected unit.
            if(MouseInRect(battle->hud)) mouse_in_hud = true;

            Vec2f pen = battle->hud.pos;

            // Draw HUD backdrop and border
            DrawFilledRect(battle->hud, c::dk_grey);
            DrawLine(pen, pen+Vec2f{battle->hud.size.x, 0.f}, c::white);

            // Draw unit name
            pen += c::hud_unit_name_offset;
            pen.y += DrawText(c::def_text_layout, pen, selected_unit->name).rect.size.y;

            // Draw unit traits
            pen.y += DrawText(c::def_text_layout, pen, "Vigor: %d/%d",
                                    selected_unit->cur_traits.vigor, selected_unit->max_traits.vigor).rect.size.y;
            pen.y += DrawText(c::def_text_layout, pen, "Focus: %d/%d",
                                    selected_unit->cur_traits.focus, selected_unit->max_traits.focus).rect.size.y;
            pen.y += DrawText(c::def_text_layout, pen, "Armor: %d/%d",
                                    selected_unit->cur_traits.armor, selected_unit->max_traits.armor).rect.size.y;



            // Determine hovered ability and draw ability buttons simultaneously.
            for(int i=0; i<ArrayCount(selected_unit->ability_ids); i++)
            {// For each ability of selected_unit
                auto ability_id = selected_unit->ability_ids[i];

                // Skip invalid abilities.
                Ability *ability = GetAbilityFromId(ability_id);
                if(!ValidAbility(ability)) continue;

                Rect ability_button_rect = GetAbilityHudButtonRect(*battle, i);

                // Set this ability as the hovered ability if the mouse is inside its button rect.
                if(PointInRect(ability_button_rect, MousePos()))
                {
                    hovered_ability_id = ability_id;
                }

                // Draw the ability button differently depending on whether it's selected or not.
                if(battle->selected_ability_id == ability_id)
                { // Button is selected
                    DrawButton(c::selected_ability_button_layout, ability_button_rect, ability->name);
                }
                else
                { // Button is not selected -- it's either hovered or not hovered.
                    // I could process the button clicking here, but for now I'm deferring it to later,
                    // where I'll check all the LMB click conditions sort of all as a contained group.
                    DrawButton(c::unselected_ability_button_layout, ability_button_rect, ability->name);
                }
            }

            // Draw the HUD's ability info box.
            // Order of priority:
            // 1) If there is a hovered ability, draw its info box. If not...
            // 2) If there is a selected ability, draw its info box. If not...
            // 3) Don't draw an ability info box.
            if(hovered_ability_id != c::null_ability_id)
            {
                int active_tier = DetermineAbilityTier(battle->selected_unit_id, hovered_ability_id);
                if(active_tier >= 0)
                {
                    DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset,
                                       battle->selected_unit_id,
                                       hovered_ability_id,
                                       c::align_topleft);
                }
            }
            else if(battle->selected_ability_id != c::null_ability_id)
            {
                int active_tier = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
                if(active_tier >= 0)
                {
                    DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset,
                                       battle->selected_unit_id,
                                       battle->selected_ability_id,
                                       c::align_topleft);
                }
            }
        }
    }

    Id<Unit> hovered_unit_id = c::null_unit_id;
    { // Update hovered_unit
        if(!mouse_in_hud)
        {
            for(int i=0; i<battle->slot_positions.count; ++i)
            {
                Unit *hovered_unit = GetUnitFromId(battle->unit_slots[i]);
                Rect slot_rect = Rect{battle->slot_positions[i], c::unit_slot_size};

                if(ValidUnit(hovered_unit) and MouseInRect(slot_rect))
                {
                    hovered_unit_id = battle->unit_slots[i];

                    if(!PointInRect(slot_rect, PrevMousePos()))
                        ResetHigh(&battle->preview_damage_timer);
                    break;
                }
            }
        }
    }

    Array<UnitId> hovered_ability_valid_target_set  = CreateTempArray<UnitId>(2*c::max_party_size);
    Array<UnitId> selected_ability_valid_target_set = CreateTempArray<UnitId>(2*c::max_party_size);
    { // Update valid target sets for hovered_ability and selected_ability if they exist.
        int hovered_tier_index = DetermineAbilityTier(battle->selected_unit_id, hovered_ability_id);
        Ability *hovered_ability = GetAbilityFromId(hovered_ability_id);
        if(ValidAbility(hovered_ability) and hovered_tier_index > 0)
        {
            ValidSelectionUnitSet(battle->selected_unit_id,
                                  hovered_ability->tiers[hovered_tier_index].target_class,
                                  battle->unit_slots,
                                  &hovered_ability_valid_target_set);
        }

        int selected_tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
        Ability *selected_ability = GetAbilityFromId(battle->selected_ability_id);
        if(ValidAbility(selected_ability) and selected_tier_index > 0)
        {
            ValidSelectionUnitSet(battle->selected_unit_id,
                                  selected_ability->tiers[selected_tier_index].target_class,
                                  battle->unit_slots,
                                  &selected_ability_valid_target_set);
        }
    }

    Intent player_intent = {};
    player_intent.target_set = CreateTempArray<UnitId>(8);
    { // Generate player_intent
        // (If there is no unit hovered or the hovered unit is an invalid target for the selected_ability,
        // the intent target list will be empty)

        //UnitSet all_units = AllBattleUnitsAsUnitSet(battle);
        int tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
        if(tier_index > 0)
        {
            Ability *selected_ability = GetAbilityFromId(battle->selected_ability_id);

            if(ValidAbility(selected_ability))
            {
                player_intent.caster_id = battle->selected_unit_id;
                player_intent.ability_id = battle->selected_ability_id;
                // int tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);

                GenerateInferredUnitSet(battle->selected_unit_id,
                                        hovered_unit_id,
                                        selected_ability->tiers[tier_index].target_class,
                                        battle->unit_slots,
                                        &player_intent.target_set);
            }
        }
    }

    { // Process player mouse input
        if(Pressed(KeyBind::Select))
        {
            // @note: Strictly speaking, these mostly probably don't need to be else ifs, but
            // I probably don't want one mouse click to be able to perform more than one kind of action sequence,
            // so I think it's better just to be on the safe side even though they should probably be
            // orthogonal cases if my game/UI state is correct. Either way, else ifs make it a bit
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

            Unit *selected_unit = GetUnitFromId(battle->selected_unit_id);
            Unit *hovered_unit = GetUnitFromId(hovered_unit_id);
            //Ability *selected_ability = GetAbilityFromId(battle->selected_ability)

            if(hovered_ability_id != c::null_ability_id and ValidUnit(selected_unit) and selected_unit->cur_action_points > 0)
            {
                battle->selected_ability_id = hovered_ability_id;
            }
            else if(ValidUnit(hovered_unit) and hovered_unit->team == Team::allies and battle->selected_ability_id == c::null_ability_id)
            {// If the player clicked on a valid unit and no ability was selected, select the clicked unit.
                // @note: I'm intuitively sensing a bit of execution order weirdness here where you have to be careful
                // with changing the selected_unit with left click, but I think it's ok if there is no selected ability.
                // Just leaving this note here to remind me if I notice something strange going on later.
                SetSelectedUnit(battle, hovered_unit_id);
            }
            else if(    ValidUnit(hovered_unit)
                    and battle->selected_ability_id != c::null_ability_id
                    and selected_unit->cur_action_points > 0
                    and UnitInUnitSet(hovered_unit_id, player_intent.target_set))
            { // Execute the player intent if a valid target is clicked and the selected unit has enough action points.

                // Generate the event associated with the player intent and execute it.
                auto events = CreateTempArray<BattleEvent>(10);
                GenerateEventsFromIntent(player_intent, &events);
                for(auto event : events)
                {
                    Unit *event_target_unit = GetUnitFromId(event.target_id);
                    if(!ValidUnit(event_target_unit)) continue;

                    event_target_unit->cur_traits += event.trait_changes;
                }

                // After executing the player intent, remove AP, clear selected ability,
                // clear related target sets, and clear player intent.
                selected_unit->cur_action_points -= 1;
                battle->selected_ability_id = c::null_ability_id;
                selected_ability_valid_target_set = {};
                player_intent = {};
            }
        }
    }

    DrawEnemyIntentThoughtBubbles(battle);

    if(Pressed(vk::shift))
    {
        ResetHigh(&battle->preview_damage_timer);
    }

    ButtonResponse end_button_turn_response = {};
    { // Draw end turn button, get its response, enter end_of_player_turn phase if button is pressed.
        if(battle->phase == BattlePhase::player_turn)
        {
            end_button_turn_response = DrawButton(c::end_button_normal_layout, c::end_turn_button_rect, "End Turn");

            if(end_button_turn_response.just_now_hovered)
            {
                ResetHigh(&battle->preview_damage_timer);
            }

            if(end_button_turn_response.pressed)
            { // Enter end_of_player_turn phase if the end turn button is clicked.
                Reset(&battle->end_player_turn_timer);
                battle->phase = BattlePhase::end_of_player_turn;
            }
        }
        else if(battle->phase == BattlePhase::end_of_player_turn)
        { // It's the player's turn and the player turn is currently ending, so draw the end turn button red
            DrawButton(c::end_button_clicked_layout, c::end_turn_button_rect, "End Turn");
        }
        else
        { // It's not the player turn, so just grey out the end turn button.
            DrawButton(c::end_button_disabled_layout, c::end_turn_button_rect, "End Turn");
        }
    }

    bool player_intent_exists = false;
    bool enemy_intent_hovered = false;
    {
        if(player_intent.target_set.count > 0) player_intent_exists = true;
        if(battle->preview_intents.count > 0) enemy_intent_hovered = true;
    }

    { // Generate preview_events
        // Priority list for which preview events to show
        // (1 to N, highest priority to lowest priority)
        //
        // 1) Player has ability selected and is hovering valid target => show effect of that ability
        //    [(if SHIFT) => maybe show effect of player ability + all ordered enemy intents?]
        // 2) Hovering enemy intent
        //    (if NO SHIFT) => show effect of that enemy intent without considering enemy intent ordering
        //    (if SHIFT)    => show effect of that enemy intent considering enemy intent ordering UP TO and including that unit
        // 3) SHIFT or end turn button hovered => show effect of all enemy intents (with order consideration)

        // So what information do we need to completely generate preview_events?:
        //
        // 1) Whether shift is down
        // 2) Whether end turn button is hovered
        // 3) Whether there is an ally unit selected, ability selected, and a valid target hovered (see "Generate player_intent")
        // 4) Whether an enemy intent is hovered (see DrawEnemyIntentThoughtBubble())
        //    ... If preview_events has any elements at this point in the method, then an enemy intent was hovered.

        if(player_intent_exists)
        { // Priority 1 (ignoring shift functionality)
            ClearArray(&battle->preview_intents);
            battle->preview_intents += player_intent;
        }
        else if(Down(vk::shift) or end_button_turn_response.hovered)
        { // Priority 3
            ClearArray(&battle->preview_intents); // Clear preview_events and generate from scratch for each enemy

            int enemy_count = 0;
            for(auto unit_id : battle->unit_slots)
            {
                Unit *unit = GetUnitFromId(unit_id);
                if(!ValidUnit(unit) or unit->team != Team::enemies) continue;

                ++enemy_count;
            }
            for(int i=0; i<enemy_count; ++i)
            {
                for(auto caster_id : battle->unit_slots)
                { // Execute enemy intents
                    Unit *caster = GetUnitFromId(caster_id);
                    if(!ValidUnit(caster) or caster->team != Team::enemies or caster->cur_traits.vigor <= 0) continue;

                    if(caster->intent.position == i)
                    {
                        battle->preview_intents += caster->intent;
                    }
                }
            }
        }
        else if(enemy_intent_hovered)
        { // Priority 2 (ignoring shift functionality)
            // battle->preview_events should already contain the correct events, because they
            // should have been generated in the DrawEnemyIntentThoughtBubbles() call above
        }
    }

    {

    }

    { // Draw "TARGET" contextually in yellow/orange/red
        if(player_intent_exists or (!enemy_intent_hovered and !Pressed(vk::shift))) // @TODO: A bit janky
        {
            TextLayout target_indication_layout = c::def_text_layout;
            target_indication_layout.font_size = 16;
            target_indication_layout.align = c::align_bottomcenter;
            Array<UnitId> target_set = {};

            bool nothing_drawn = false;
            if(UnitInUnitSet(hovered_unit_id, selected_ability_valid_target_set))
            { // An ability is selected AND a valid target for that ability is hovered => draw TARGET in red over inferred target set

                //battle->preview_events = GenerateEventsFromIntent(battle, player_intent);
                target_indication_layout.color = c::red;
                AssignArray(&target_set, player_intent.target_set);
            }
            else
            {
                if(hovered_ability_id != battle->selected_ability_id and hovered_ability_id != c::null_ability_id)
                {
                    // 2) An ability button is being hovered => draw valid targets for hovered ability
                    AssignArray(&target_set, hovered_ability_valid_target_set);
                    target_indication_layout.color = c::yellow;
                }
                else if(battle->selected_ability_id != c::null_ability_id)
                {
                    // 3) An ability is selected => draw valid targets for selected ability
                    AssignArray(&target_set, selected_ability_valid_target_set);
                    target_indication_layout.color = c::orange;
                }
                else
                {
                    // There is no hovered nor selected ability, so there's no targeting
                    // info to draw.
                    nothing_drawn = true;
                }
            }

            // For each unit in the battle, draw TARGET above its unit slot if it's in the relevant target set
            if(!nothing_drawn)
            {
                //for(int i=0; i<battle->unit_slots.count; i++)
                for(auto unit_id : battle->unit_slots)
                {
                    Unit *unit = GetUnitFromId(unit_id);
                    if(!ValidUnit(unit)) continue;
                    if(!UnitInUnitSet(unit_id, target_set)) continue;

                    Vec2f origin = battle->slot_positions[unit->battle_slot_index];
                    DrawText(target_indication_layout,
                             origin + Vec2f{0.5f*c::unit_slot_size.x, 0.f},
                             "TARGET");
                }
            }
        }
    }

    { // Tick preview damage timer
        Tick(&battle->preview_damage_timer);
    }

    { // Draw units
        //for(int i=0; i<battle->unit_slots.size; ++i)
        Array<TraitSet> preview_traitset_changes = CreateTempArray<TraitSet>(battle->unit_slots.count);
        IntentTraitChangesForUnits(battle->preview_intents, battle->unit_slots, &preview_traitset_changes);

        //for(auto unit_id : battle->unit_slots)
        for(int i=0; i<battle->unit_slots.count; ++i)
        {
            auto unit_id = battle->unit_slots[i];
            SetDrawDepth(c::field_draw_depth);

            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;

            if(unit->cur_traits.vigor == 0)
            { // Unit is dead
                TextLayout dead_text_layout = c::def_text_layout;
                dead_text_layout.color = c::dk_red;
                dead_text_layout.align = c::align_center;

                Vec2f origin = battle->slot_positions[unit->battle_slot_index];
                Rect unit_rect = Rect{origin, c::unit_slot_size};

                DrawText(dead_text_layout, RectCenter(unit_rect), "DEAD");
            }
            else
            {
                // Change the outline color for selected and hovered units
                Color outline_color = c::dk_grey;
                if(unit_id == battle->selected_unit_id)
                {
                    outline_color = c::white;
                }
                else if(unit_id == hovered_unit_id)
                {
                    outline_color = c::grey;
                }

                Vec2f origin = battle->slot_positions[unit->battle_slot_index];

                // Draw unit slot outline
                DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

                // Draw unit name
                TextLayout unit_name_layout = c::def_text_layout;
                unit_name_layout.font_size = 24;
                unit_name_layout.align = c::align_topcenter;
                Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name).rect.size;

                // Generate preview_trait_changes for this unit
//                TraitSet preview_trait_changes = EventTraitChangesForUnit(battle->preview_intents, unit_id);
                TraitSet preview_trait_change = preview_traitset_changes[i];


                // Draw trait bars
                DrawTraitSetWithPreview(origin + Vec2f{0.f, name_size.y},
                                        unit->cur_traits,
                                        unit->max_traits,
                                        unit->cur_traits + preview_trait_change,
                                        battle->preview_damage_timer.cur);

                // AP text
                DrawText(c::action_points_text_layout, origin + c::action_points_text_offset,
                         "AP: %d", unit->cur_action_points);

                SetDrawDepth(c::battle_arrow_draw_depth);
                // Draw directed arrows for previewed events
                #if 0
                for(auto event : battle->preview_events)
                {
                    if(event.target_id != unit_id) continue;

                    Unit *caster = GetUnitFromId(event.caster_id);
                    if(!ValidUnit(caster)) continue;
                    Unit *target = unit; // alias

                    DirectedLineLayout dline_layout = {};
                    Rect caster_rect = {caster->slot_pos, c::unit_slot_size};
                    Rect target_rect = {target->slot_pos, c::unit_slot_size};

                    float line_speed = 800.f;
                    Vec2f caster_pos = RectTopLeft(caster_rect);
                    Vec2f target_pos = RectTopRight(target_rect);
                    Vec2f arrow_direction = Normalize(target_pos - caster_pos);
                    Vec2f start_vel = 0.5f*(Vec2f{0.f,-1.f} + arrow_direction);
                    Vec2f end_vel = 0.5f*(  Vec2f{0.f, 1.f} + arrow_direction);
                    DrawDirectedLine(dline_layout,
                                     caster_pos,
                                     target_pos,
                                     line_speed*start_vel,
                                     line_speed*end_vel,
                                     StringFromCString(""));
                }
                #endif
            }

            // Ability "buttons"/icons for allied units (enemy ones are drawn slightly differently
            // in order to show intent)
            if(unit->team == Team::allies)
            {
                Vec2f pen = battle->slot_positions[unit->battle_slot_index] + c::ability_icon_offset;
                // Iterate abilities backwards because we draw the icons from top to bottom
                // and we want them to be in the same order as they appear in the HUD.
                for(int ability_index=ArrayCount(unit->ability_ids)-1; ability_index>=0; --ability_index)
                {
                    Id ability_id = unit->ability_ids[ability_index];
                    // Ability *ability = GetAbilityFromId(ability_id);
                    // if(!ValidAbility(ability)) continue;

                    Rect ability_button_rect = {pen, c::ability_button_size};
                    DrawAbilityButton(ability_button_rect, unit_id, ability_id, false, c::align_bottomcenter);

                    pen.y -= (1 + c::ability_button_size.y); // 1 + so the button outlines don't overlap
                }
            }
        }
    }

    { // If it's end_of_player_turn, tick the timer, and go to enemy_turn phase if timer is finished
        if(battle->phase == BattlePhase::end_of_player_turn and Tick(&battle->end_player_turn_timer))
        {
            battle->phase = BattlePhase::enemy_turn;
        }
    }

    if(battle->phase == BattlePhase::enemy_turn)
    {
        auto events = CreateTempArray<BattleEvent>(10);
        int enemy_count = 0;
        for(auto unit_id : battle->unit_slots)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit) or unit->team != Team::enemies) continue;

            ++enemy_count;
        }

        for(int i=0; i<enemy_count; ++i)
        {
            for(auto caster_id : battle->unit_slots)
            { // Execute enemy intents
                Unit *caster = GetUnitFromId(caster_id);
                if(!ValidUnit(caster) or caster->team != Team::enemies or caster->cur_traits.vigor <= 0) continue;

                if(caster->intent.position == i)
                {
                    ClearArray(&events);
                    GenerateEventsFromIntent(caster->intent, &events);

                    for(auto event : events)
                    {
                        Unit *target = GetUnitFromId(event.target_id);
                        if(!ValidUnit(target)) continue;

                        target->cur_traits += event.trait_changes;
                    }
                }
            }
        }



        // Reset action points of all units
        for(Id unit_id : battle->unit_slots)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;

            unit->cur_action_points = unit->max_action_points;
        }

        //GenerateEnemyIntents(battle);
        // Enemy AI (# of permutations)
        Array<UnitId> ally_unitset = CreateTempArray<UnitId>(4);
        Array<UnitId> enemy_unitset = CreateTempArray<UnitId>(4);
        Array<UnitId> ordered_battle_units = CreateTempArray<UnitId>(8);
        for(Id<Unit> unit_id : battle->unit_slots)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit) or UnitIsDead(unit_id)) continue;

            if(     unit->team == Team::allies)  ally_unitset += unit_id;
            else if(unit->team == Team::enemies) enemy_unitset += unit_id;
        }
        AppendArrayToArray(&ordered_battle_units, enemy_unitset);
        AppendArrayToArray(&ordered_battle_units, ally_unitset);
        battle->best_choice_string = DoAiStuff(enemy_unitset, ordered_battle_units, Team::enemies, battle->ai_arena_id);

        // Set player turn
        battle->phase = BattlePhase::player_turn;
    }

    { // Automatically end the turn if no ally unit has remaining action points.
        if(battle->phase == BattlePhase::player_turn)
        {
            bool any_ally_has_ap = false;
            for(Id unit_id : battle->unit_slots)
            {
                Unit *unit = GetUnitFromId(unit_id);
                if(!ValidUnit(unit) or unit->cur_traits.vigor <= 0) continue;

                if(unit->team == Team::allies and unit->cur_action_points > 0)
                {
                    any_ally_has_ap = true;
                    break;
                }
            }

            if(!any_ally_has_ap)
            { // Go to end_of_player_turn phase if no allies have AP left
                Reset(&battle->end_player_turn_timer);
                battle->phase = BattlePhase::end_of_player_turn;
            }
        }
    }


    { // Battle Score
        Array<TraitSet> ally_traitsets  = CreateTempArray<TraitSet>(4);
        Array<TraitSet> enemy_traitsets = CreateTempArray<TraitSet>(4);
        for(Id<Unit> unit_id : battle->unit_slots)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;

            if     (unit->team == Team::allies)  ally_traitsets  += unit->cur_traits;
            else if(unit->team == Team::enemies) enemy_traitsets += unit->cur_traits;
        }
        //float battle_state_score = ScoreBattleState(ally_traitsets, enemy_traitsets);

        #if 0
        DrawTextMultiline(c::small_text_layout, {}, "%.*s",
                          battle->best_choice_string.length, battle->best_choice_string.data);
        #endif
    }

    { // Set battle to finished if either all allies or all enemies are dead.
        bool any_ally_is_alive = false;
        bool any_enemy_is_alive = false;
        for(auto unit_id : battle->unit_slots)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!unit) continue;

            if(unit->cur_traits.vigor > 0)
            {
                if     (unit->team == Team::allies) any_ally_is_alive = true;
                else if(unit->team == Team::enemies) any_enemy_is_alive = true;
            }
        }

        if(!any_ally_is_alive or !any_enemy_is_alive) battle_state.finished = true;
    }

    return battle_state;
}