#include "battle.h"

#include "draw.h"
#include "game.h"
#include "random.h"
#include "array.h"
#include "table.h"
#include "keybinds.h"

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
UnitSet
ValidSelectionUnitSet(Id<Unit> caster_id, TargetClass tc, UnitSet all_units)
{
    UnitSet target_set = {};

    Unit *caster = GetUnitFromId(caster_id);
    if(!ValidUnit(caster)) return target_set;

    for(Id target_id : all_units)
    {
        if(!CheckValidTarget(caster_id, target_id, tc)) continue;

        AddUnitToUnitSet(target_id, &target_set);
    }

    return target_set;
}

Array<BattleEvent>
GenerateBattlePreviewEvent(Battle *battle, Intent intent)
{
    TIMED_BLOCK;

    if(!battle)
    {
        VerboseError(__FUNCTION__ "() [ln:%u] received nullptr battle.", __LINE__);
        return {};
    }

    // // Check for valid intent target set size.
    // if(intent.targets.size < 0 or intent.targets.size > c::max_target_count)
    // {
    //     VerboseError(__FUNCTION__"() received an intent whose target set had an invalid size (%d).", intent.targets.size);

    //     #if DEBUG_BUILD
    //         Assert(false);
    //     #else
    //         return {};
    //     #endif
    // }

    auto caster_id = intent.caster_id;
    auto ability_id = intent.ability_id;

    // Check that the caster and ability in intent are valid.
    Unit *caster = GetUnitFromId(caster_id);
    Ability *ability = GetAbilityFromId(ability_id);
    if(!caster)
    {
        VerboseError(__FUNCTION__"() received an intent with an invalid caster.");
        return {};
    }
    if(!ability)
    {
        VerboseError(__FUNCTION__"() received an intent with an invalid ability.");
        return {};
    }

    if(caster->cur_traits.vigor <= 0) return{};

    // Determine which ability tier should be used in the preview
    int tier = DetermineAbilityTier(caster_id, ability_id);
    if(tier < 0) return {}; // The caster can't use the ability, so there is no effect to preview.
    AbilityTier *cur_ability_tier = &ability->tiers[tier]; // alias

    int events_initial_size = 20; // We want this big enough that most of the time, it won't need to be resized,
                                  // but not wasting too much memory for the average preview event.
                                  // It probably doesn't really matter that much -- BattleEvent is (AOWTC) only something
                                  // like 24 bytes, and we'd probably only expect to be dealing with
                                  // on the order of 10 events at a time.
    auto events = CreateTempArray<BattleEvent>(events_initial_size);

    bool first_effect = true;
    int target_set_index = 0; // index into intent.target_sets
    //TargetClass cur_target_class = {};
    // Create an event in [events] for each effect in the current ability tier.
    for(auto effect : cur_ability_tier->effects_)
    {
        for(Id<Unit> target_id : intent.target_set)
        {
            Unit *target = GetUnitFromId(target_id);
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
                events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
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
                events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
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
                events += BattleEvent{.caster_id=caster_id, .target_id=target_id, .trait_changes=trait_changes};
            }

        }
    }

    return events;
}

TraitSet
EventTraitChangesForUnit(Array<BattleEvent> events, Id<Unit> target_id)
{
    TraitSet trait_changes = {};

    for(auto event : events)
    {
        Unit *caster = GetUnitFromId(event.caster_id);
        if(!ValidUnit(caster) or caster->cur_traits.vigor <= 0) continue;
        if(target_id != event.target_id) continue;

        trait_changes += event.trait_changes;
    }

    return trait_changes;
}

void
DrawUnits(Battle *battle)
{
    for(int i=0; i<battle->units.size; ++i)
    {
        Id<Unit> unit_id = battle->units.ids[i];
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        // Change the outline color for selected and hovered units
        Color outline_color = c::black;
        // if(unit == battle->selected_unit)
        // {
        //  outline_color = c::green;
        // }
        // else if(unit == battle->hovered_unit)
        // {
        //  outline_color = c::grey;
        // }

        Vec2f origin = battle->unit_slots[i];

        // Draw unit slot outline
        DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

        // Draw unit name
        TextLayout unit_name_layout = c::def_text_layout;
        unit_name_layout.font_size = 24;
        unit_name_layout.align = c::align_topcenter;
        Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name).size;

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

void
DrawAbilityInfoBox(Vec2f pos, Id<Ability> ability_id, int active_tier_level, Align align)
{
    SetDrawDepth(1.f);
    Ability *ability = GetAbilityFromId(ability_id);
    if(!ValidAbility(ability)) return;

    Rect infobox_aligned_rect = AlignRect({pos, c::ability_info_box_size}, align);
    Vec2f pen = infobox_aligned_rect.pos;
    DrawFilledRect(infobox_aligned_rect, c::ability_info_bg_color);
    DrawUnfilledRect(infobox_aligned_rect, c::white);

    TextLayout layout = c::def_text_layout;
    layout.align = c::align_topcenter;
    Vec2f name_text_size = DrawText(layout, RectTopCenter(infobox_aligned_rect), ability->name).size;
    //pen.y += name_size.y;


    // layout.font_size = 16;
    // layout.align = c::align_rightcenter;
    // Vec2f target_text_size = DrawText(layout, RectTopRight(infobox_aligned_rect) + Vec2f{0.f, 0.5f*name_text_size.y},
    //                                "targets %s", TargetClass_userstrings[(int)ability->target_class]);

    pen.y += name_text_size.y;
    DrawLine(pen, pen + Vec2f{c::ability_info_box_size.x, 0.f});
    pen.y += c::tier_data_y_half_padding;

    TextLayout tier_inactive_text_layout = c::small_text_layout;
    tier_inactive_text_layout.color = c::lt_grey;
    TextLayout tier_active_text_layout = c::small_text_layout;
    tier_active_text_layout.color = c::white;

    // i=1 because we skip the lowest tier (which is the "empty" tier)
    for(int i=1; i<ability->tiers.count; i++)
    {
        AbilityTier &tier = ability->tiers[i]; // alias
        //if(!tier.init) continue;

        if(i == active_tier_level)
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

        pen.y += c::tier_data_y_half_padding;
        DrawLine({pen.x, pen.y}, {pen.x + c::ability_info_box_size.x, pen.y}, c::grey);
        pen.y += c::tier_data_y_half_padding;
    }
}

void
DrawEnemyIntentThoughtBubble(Battle *battle)
{
    for(int unit_index=0; unit_index<battle->units.size; ++unit_index)
    {
        SetDrawDepth(c::field_draw_depth);

        auto caster_id = battle->units.ids[unit_index];
        Unit *caster = GetUnitFromId(caster_id);
        if(!ValidUnit(caster) or caster->team != Team::enemies or caster->cur_traits.vigor <= 0) continue;

        Intent intent = battle->intents[unit_index];
        if(intent.caster_id != caster_id)
        {
            VerboseError(__FUNCTION__"(): intent.caster_id != caster_id when getting data from the parallel battle->intents[] and battle->units arrays.");
            continue;
        }

        Vec2f pen = battle->unit_slots[unit_index] + c::enemy_intent_offset;
        for(int j=0; j<ArrayCount(caster->ability_ids); ++j)
        {
            int ability_index = j;//ArrayCount(caster->ability_ids)-j-1;
            Id ability_id = caster->ability_ids[ability_index];
            Ability *ability = GetAbilityFromId(ability_id);
            if(!ValidAbility(ability)) continue;

            int tier = DetermineAbilityTier(caster_id, ability_id);

            ButtonResponse response = {};
            if(tier <= 0)
            {
                response = DrawButton(c::enemy_cannot_use_button_layout,
                                      {pen, c::enemy_intent_button_size},
                                      ability->name);
            }
            else if(ability_id == intent.ability_id)
            {
                response = DrawButton(c::enemy_intented_button_layout,
                                      {pen, c::enemy_intent_button_size},
                                      ability->name);
                if(response.just_now_hovered)
                {
                    ResetHigh(&battle->preview_damage_timer);
                }
                if(response.hovered)
                {
                    battle->show_preview = true;
                    battle->preview_events = GenerateBattlePreviewEvent(battle, intent);
                }
            }
            else
            {
                response = DrawButton(c::enemy_can_use_button_layout,
                                      {pen, c::enemy_intent_button_size},
                                      ability->name);
            }

            if(response.hovered and (Down(vk::alt) or Down(vk::LMB)))
            {
                DrawAbilityInfoBox(MousePos(), ability_id, tier, c::align_topleft);
            }

            pen.y -= (1 + response.rect.size.y); // 1 + so the button outlines don't overlap
        }
    }

    // if(ability_id != battle->last_frame_hovered_ability_id)
    // {
    //     ResetHigh(&battle->preview_damage_timer);
    // }
    //battle->last_frame_hovered_ability_id = ability_id;
}

void
GenerateEnemyIntents(Battle *battle)
{
    // Choose enemy intents randomly, equally distributed between abilities and possible targets.
    for(int i=0; i<battle->units.size; i++)
    {
        Id unit_id = battle->units.ids[i];
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit) or unit->team != Team::enemies or unit->cur_traits.vigor <= 0) continue;

        u32 chosen_ability_index; // The index of the ability chosen to cast for this enemy
        u32 chosen_target_index; // The index (into battle->units[])

        int valid_ability_count = 0; // Number of valid abilities
        int valid_ability_indices[c::moveset_max_size] = {}; // Indices of abilities that are initialized.
        UnitSet valid_target_sets[c::moveset_max_size] = {}; // Valid target sets corresponding to valid_ability_indices
        for(int i=0; i<ArrayCount(unit->ability_ids); i++)
        {
            Id ability_id = unit->ability_ids[i];
            int tier_index = DetermineAbilityTier(unit_id, ability_id);
            if(tier_index < 0) continue;

            Ability *ability = GetAbilityFromId(ability_id);
            if(!ValidAbility(ability)) continue;

            UnitSet valid_target_set = ValidSelectionUnitSet(unit_id, ability->tiers[tier_index].target_class, battle->units);
            if(valid_target_set.size > 0)
            {
                valid_ability_indices[valid_ability_count] = i;
                valid_target_sets[i] = valid_target_set;
                ++valid_ability_count;
            }

        }

        if(valid_ability_count <= 0) continue;

        chosen_ability_index = valid_ability_indices[RandomU32(0, valid_ability_count-1)];
        Id chosen_ability_id = unit->ability_ids[chosen_ability_index];
        UnitSet chosen_ability_valid_targets = valid_target_sets[chosen_ability_index];


        chosen_target_index = RandomU32(0, chosen_ability_valid_targets.size-1);
        Id chosen_target_id = chosen_ability_valid_targets.ids[chosen_target_index];

        int tier_index = DetermineAbilityTier(unit_id, chosen_ability_id);
        if(tier_index >= 0)
        {
            Ability *chosen_ability = GetAbilityFromId(chosen_ability_id);
            if(!ValidAbility(chosen_ability)) continue;
            AbilityTier &tier = chosen_ability->tiers[tier_index]; // alias

            //AbilityTier &active_tier = chosen_ability->tiers[tier_index];

            battle->intents[i].caster_id = unit_id;
            battle->intents[i].ability_id = chosen_ability_id;
            battle->intents[i].target_set = GenerateInferredUnitSet(unit_id,
                                                                    chosen_target_id,
                                                                    tier.target_class,
                                                                    battle->units);
        }
    }
}

void
SetSelectedAbility(Battle *battle, Id<Ability> new_ability_id)
{
    battle->selected_ability_id = new_ability_id;
    ResetHigh(&battle->preview_damage_timer);
}

void
SetSelectedUnit(Battle *battle, Id<Unit> new_unit_id)
{
    battle->selected_unit_id = new_unit_id;
    battle->selected_ability_id = c::null_ability_id;
}

void InitBattle(Battle *battle, PoolId<Arena> arena_id)
{
    // Memory allocation
    battle->arena_id = arena_id;//AllocArena("Battle");
    ClearArena(arena_id);
    battle->preview_events = CreateArrayFromArena<BattleEvent>(100, battle->arena_id);
    battle->intents =        CreateArrayFromArena<Intent>(8, battle->arena_id);
    for(int i=0; i<8; ++i) battle->intents += {};

    // Fill out timers
    battle->preview_damage_timer = {
        .low = 0.3f,
        .high = 1.0f,
    };
    ResetHigh(&battle->preview_damage_timer);

    battle->end_button_clicked_timer = {};
    battle->end_button_clicked_timer.length_s = c::end_button_clicked_time_s;

    battle->hud = {{0.f, game->window_size.y-c::hud_offset_from_bottom}, {game->window_size.x, c::hud_offset_from_bottom}};
}

void
StartBattle(Battle *battle, UnitSet battle_units)
{
    battle->units = battle_units;

    // Set action points=1 for all units
    for(Id<Unit> unit_id : battle->units)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        unit->cur_action_points = unit->max_action_points;
    }

    GenerateEnemyIntents(battle);

    battle->is_player_turn = true;

    // Enemy AI (# of permutations)
    UnitSet ally_unitset = {};
    UnitSet enemy_unitset  = {};
    for(Id<Unit> unit_id : battle->units)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        if(     unit->team == Team::allies)  AddUnitToUnitSet(unit_id, &ally_unitset);
        else if(unit->team == Team::enemies) AddUnitToUnitSet(unit_id, &enemy_unitset);
    }
    //battle->best_choice_string = DoAiStuff(ally_unitset, enemy_unitset, &battle->arena);

    { // Generate unit slots
        Vec2f pos = {50.f, 300.f};
        float x_between_slots = c::unit_slot_size.x + c::unit_slot_padding;
        for(int i=0; i<ally_unitset.size; ++i)
        {
            battle->unit_slots[i] = pos;
            pos.x += x_between_slots;
        }

        pos = {50.f + c::max_party_size*x_between_slots, 300.f};
        for(int i=0; i<enemy_unitset.size; ++i)
        {
            battle->unit_slots[ally_unitset.size + i] = pos;
            pos.x += x_between_slots;
        }
    }
}

BattleState
TickBattle(Battle *battle)
{
    ClearArray<BattleEvent>(&battle->preview_events);
    //ClearArray<Intent>(&battle->intents);

    { // Restart battle button

    }

    bool mouse_in_hud = false;
    { // Generate this stuff fresh every frame.
        battle->show_preview = false;
    }

    { // Process hotkey input
        // Ally unit selection with number keys (1-4)
        if(Pressed(KeyBind::SelectUnit1))
        {
            SetSelectedUnit(battle, battle->units.ids[0]);
        }
        if(Pressed(KeyBind::SelectUnit2))
        {
            SetSelectedUnit(battle, battle->units.ids[1]);
        }
        if(Pressed(KeyBind::SelectUnit3))
        {
            SetSelectedUnit(battle, battle->units.ids[2]);
        }
        if(Pressed(KeyBind::SelectUnit4))
        {
            SetSelectedUnit(battle, battle->units.ids[3]);
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
                SetSelectedUnit(battle, c::null_unit_id);
            }
        }

        // Tab to go to next ally unit
        if(PressedOrRepeated(KeyBind::CycleUnits))
        {
            // Find index into battle->units of currently selected unit
            int selected_unit_index = -1;
            for(int i=0; i<battle->units.size; i++)
            {
                if(battle->units.ids[i] == battle->selected_unit_id) selected_unit_index = i;
            }

            if(selected_unit_index == -1)
            {
                // If no unit is selected, TAB selects the first unit
                battle->selected_unit_id = battle->units.ids[0];
            }
            else
            {
                // @note: this assumes that the first [max_party_size] slots are all ally units and that
                //        there are no other ally slots. This is probably fine for now, but if we go away
                //        from that model, this will be invalidated (which should be obvious if I use the
                //        tab functionality frequently)

                // Go to next unit, except when the last unit it selected; then loop back to the first unit.
                battle->selected_unit_id = battle->units.ids[(selected_unit_index+1) % c::max_party_size];
                battle->selected_ability_id = {};
            }
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
            pen.y += DrawText(c::def_text_layout, pen, selected_unit->name).size.y;

            // Draw unit traits
            pen.y += DrawText(c::def_text_layout, pen, "Vigor: %d/%d",
                                    selected_unit->cur_traits.vigor, selected_unit->max_traits.vigor).size.y;
            pen.y += DrawText(c::def_text_layout, pen, "Focus: %d/%d",
                                    selected_unit->cur_traits.focus, selected_unit->max_traits.focus).size.y;
            pen.y += DrawText(c::def_text_layout, pen, "Armor: %d/%d",
                                    selected_unit->cur_traits.armor, selected_unit->max_traits.armor).size.y;



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
                                       hovered_ability_id,
                                       active_tier,
                                       c::align_topleft);
                }
            }
            else if(battle->selected_ability_id != c::null_ability_id)
            {
                int active_tier = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
                if(active_tier >= 0)
                {
                    DrawAbilityInfoBox(battle->hud.pos + c::hud_ability_info_offset,
                                       battle->selected_ability_id,
                                       active_tier,
                                       c::align_topleft);
                }
            }
        }
    }

    Id<Unit> hovered_unit_id = c::null_unit_id;
    { // Update hovered_unit
        if(!mouse_in_hud)
        {
            for(int i=0; i<battle->units.size; i++)
            {
                Rect unit_slot_rect = Rect{battle->unit_slots[i], c::unit_slot_size};
                if(MouseInRect(unit_slot_rect))
                {
                    auto unit_id = battle->units.ids[i];
                    Unit *unit = GetUnitFromId(unit_id);
                    if(ValidUnit(unit)) hovered_unit_id = unit_id;

                    //DrawTextMultiline(c::small_text_layout, MousePos(), MetaString(unit));

                    break;
                }
            }
        }
    }

    UnitSet hovered_ability_valid_target_set = {};
    UnitSet selected_ability_valid_target_set = {};
    { // Update valid target sets for hovered_ability and selected_ability if they exist.
        int hovered_tier_index = DetermineAbilityTier(battle->selected_unit_id, hovered_ability_id);
        Ability *hovered_ability = GetAbilityFromId(hovered_ability_id);
        if(ValidAbility(hovered_ability) and hovered_tier_index >= 0)
        {

            hovered_ability_valid_target_set = ValidSelectionUnitSet(battle->selected_unit_id,
                                                                     hovered_ability->tiers[hovered_tier_index].target_class,
                                                                     battle->units);
        }

        int selected_tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
        Ability *selected_ability = GetAbilityFromId(battle->selected_ability_id);
        if(ValidAbility(selected_ability) and selected_tier_index >= 0)
        {

            selected_ability_valid_target_set = ValidSelectionUnitSet(battle->selected_unit_id,
                                                                     selected_ability->tiers[selected_tier_index].target_class,
                                                                     battle->units);
        }
    }

    Intent player_intent = {};
    { // Generate player_intent
        // (If there is no unit hovered or the hovered unit is an invalid target for the selected_ability,
        // the intent target list will be empty)

        //UnitSet all_units = AllBattleUnitsAsUnitSet(battle);
        int tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);
        if(tier_index >= 0)
        {
            Ability *selected_ability = GetAbilityFromId(battle->selected_ability_id);

            if(ValidAbility(selected_ability))
            {
                player_intent.caster_id = battle->selected_unit_id;
                player_intent.ability_id = battle->selected_ability_id;
                int tier_index = DetermineAbilityTier(battle->selected_unit_id, battle->selected_ability_id);

                if(tier_index >= 0)
                {
                    player_intent.target_set = GenerateInferredUnitSet(battle->selected_unit_id,
                                                                       hovered_unit_id,
                                                                       selected_ability->tiers[tier_index].target_class,
                                                                       battle->units);
                }
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
                battle->selected_unit_id = hovered_unit_id;
            }
            else if(ValidUnit(hovered_unit) and battle->selected_ability_id != c::null_ability_id and selected_unit->cur_action_points > 0)
            { // Execute the player intent if a valid target is clicked and the selected unit has enough action points.

                // Generate the event associated with the player intent and execute it.
                auto events = GenerateBattlePreviewEvent(battle, player_intent);
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
                battle->show_preview = false;
                player_intent = {};
            }
        }
    }

    DrawEnemyIntentThoughtBubble(battle);

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

                if(response.just_now_hovered or Pressed(vk::shift))
                {
                    ResetHigh(&battle->preview_damage_timer);
                }

                if(response.hovered or Down(vk::shift))
                {
                    // Draw the net effect of all enemy intents
                    TraitSet traitset_changes[c::max_target_count] = {};
                    ClearArray(&battle->preview_events);
                    for(Intent intent : battle->intents)
                    {
                        Unit *caster = GetUnitFromId(intent.caster_id);
                        if(!ValidUnit(caster) or caster->team != Team::enemies) continue;

                        Array events = GenerateBattlePreviewEvent(battle, intent);
                        AppendArrayToArray(&battle->preview_events, events);
                        // for(auto event : events)
                        // {
                        //     Unit *event_target = GetUnitFromId(event.unit_id);
                        //     if(!ValidUnit(event_target)) continue;

                        //     int target_index_in_battle_units;
                        //     if(UnitInUnitSet(event.unit_id, battle->units, &target_index_in_battle_units))
                        //     {
                        //         traitset_changes[target_index_in_battle_units] += event.trait_changes;
                        //         //traitset_changes->cur_traits += event.trait_changes;
                        //     }
                        // }
                    }

                    battle->show_preview = true;
                    // for(int i=0; i<ArrayCount(traitset_changes); ++i)
                    // {

                    // }
                }
                if(response.pressed)
                {
                    // Start ending the turn if the end turn button is clicked.
                    battle->ending_player_turn = true;
                    Reset(&battle->end_button_clicked_timer);
                    Log("End turn pressed");
                }
            }
        }
        else
        { // It's not the player turn, so just grey out the end turn button.
            DrawButton(c::end_button_disabled_layout, c::end_turn_button_rect, "End Turn");
        }
    }

    //DrawEnemyIntentThoughtBubble(battle);

    { // Draw "TARGET" contextually in yellow/orange/red
        TextLayout target_indication_layout = c::def_text_layout;
        target_indication_layout.font_size = 16;
        target_indication_layout.align = c::align_bottomcenter;
        UnitSet target_set = {};

        bool nothing_drawn = false;
        if(UnitInUnitSet(hovered_unit_id, selected_ability_valid_target_set))
        {
            // 1) An ability is selected AND a valid target for that ability is hovered => draw inferred target set and outcome if that ability were to be used.
            battle->show_preview = true;
            battle->preview_events = GenerateBattlePreviewEvent(battle, player_intent);
            target_indication_layout.color = c::red;
            target_set = player_intent.target_set;
        }
        else
        {
            if(hovered_ability_id != battle->selected_ability_id and hovered_ability_id != c::null_ability_id)
            {
                // 2) An ability button is being hovered => draw valid targets for hovered ability
                target_set = hovered_ability_valid_target_set;
                target_indication_layout.color = c::yellow;
            }
            else if(battle->selected_ability_id != c::null_ability_id)
            {
                // 3) An ability is selected => draw valid targets for selected ability
                target_set = selected_ability_valid_target_set;
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
            for(int i=0; i<battle->units.size; i++)
            {
                Id<Unit> unit_id = battle->units.ids[i];
                Unit *unit = GetUnitFromId(unit_id);
                if(!ValidUnit(unit)) continue;
                if(!UnitInUnitSet(unit_id, target_set)) continue;

                Vec2f origin = battle->unit_slots[i];
                DrawText(target_indication_layout,
                         origin + Vec2f{0.5f*c::unit_slot_size.x, 0.f},
                         "TARGET");
            }
        }
    }

    { // Tick preview damage timer
        // @todo: We should reset this timer when a new selection is hovered, so it kind
        //        of "snaps" to the full color value.
        Tick(&battle->preview_damage_timer);
    }

    { // Draw units
        for(int i=0; i<battle->units.size; ++i)
        {
            SetDrawDepth(c::field_draw_depth);

            Id<Unit> unit_id = battle->units.ids[i];
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;

            if(unit->cur_traits.vigor == 0)
            { // Unit is dead
                TextLayout dead_text_layout = c::def_text_layout;
                dead_text_layout.color = c::dk_red;
                dead_text_layout.align = c::align_center;

                Vec2f origin = battle->unit_slots[i];
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

                Vec2f origin = battle->unit_slots[i];

                // Draw unit slot outline
                DrawUnfilledRect(origin, c::unit_slot_size, outline_color);

                // Draw unit name
                TextLayout unit_name_layout = c::def_text_layout;
                unit_name_layout.font_size = 24;
                unit_name_layout.align = c::align_topcenter;
                Vec2f name_size = DrawText(unit_name_layout, origin + c::unit_slot_name_offset, unit->name).size;

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

                // AP text
                DrawText(c::action_points_text_layout, origin + c::action_points_text_offset,
                         "AP: %d", unit->cur_action_points);
            }

            // Ability "buttons"/icons for allied units (enemy ones are drawn slightly differently
            // in order to show intent)
            if(unit->team == Team::allies)
            {
                Vec2f pen = battle->unit_slots[i] + c::ability_icon_offset;
                // Iterate abilities backwards because we draw the icons from top to bottom
                // and we want them to be in the same order as they appear in the HUD.
                for(int ability_index=ArrayCount(unit->ability_ids)-1; ability_index>=0; --ability_index)
                {
                    Id ability_id = unit->ability_ids[ability_index];
                    Ability *ability = GetAbilityFromId(ability_id);
                    if(!ValidAbility(ability)) continue;

                    int tier = DetermineAbilityTier(unit_id, ability_id);

                    ButtonResponse response = {};
                    ButtonLayout ability_icon_button_layout = {};
                    if(tier <= 0) ability_icon_button_layout = c::cannot_use_ability_button_layout;
                    else          ability_icon_button_layout = c::can_use_ability_button_layout;

                    SetDrawDepth(c::field_draw_depth);
                    response = DrawButton(ability_icon_button_layout,
                                          {pen, c::ability_icon_size},
                                          ability->name);

                    Vec2f padding = {4.f,4.f};
                    Rect padded_button_rect = {
                        .pos = response.rect.pos + padding,
                        .size = response.rect.size - 2*padding
                    };


                    // Tier string. e.g., "Tier: 2/3"
                    Color tier_color = c::grey;
                    int max_tier = ability->tiers.count - 1;
                    if(tier == max_tier)          tier_color = c::gold;
                    else if(tier == max_tier - 1) tier_color = c::silver;
                    else if(tier == max_tier - 2) tier_color = c::bronze;

                    String tier_string = AllocStringDataFromArena(20, memory::per_frame_arena_id);
                    AppendCString(&tier_string, "%d/%d", tier, max_tier);
                    TextLayout text_layout = ability_icon_button_layout.label_layout;
                    text_layout.align = c::align_bottomleft;
                    text_layout.color = tier_color;
                    Vec2f tier_string_size = SizeText(text_layout, tier_string);
                    Vec2f tier_string_pos = AlignSizeInRect(tier_string_size, padded_button_rect, text_layout.align);
                    DrawText(text_layout, tier_string_pos, tier_string);

                    // Required trait change to level up ability.
                    if(tier != max_tier)
                    {
                        int next_tier = tier + 1;

                        TraitSet cur_traits = unit->cur_traits;//ability->tiers[tier].required_traits;
                        TraitSet next_required = ability->tiers[next_tier].required_traits;

                        TraitSet required_diff = next_required - cur_traits;
                        for(int &value : required_diff)
                            value = m::Max(0, value);

                        // Vigor
                        String trait_change_string = AllocStringDataFromArena(30, memory::per_frame_arena_id);

                        bool first_trait = true;
                        if(required_diff.vigor > 0)
                        {
                            if(first_trait) {
                                first_trait = false;
                                AppendCString(&trait_change_string, "`green`+");
                            }
                            else {
                                AppendCString(&trait_change_string, "");
                            }
                            AppendCString(&trait_change_string, "`red`%dV", required_diff.vigor);
                        }
                        if(required_diff.focus > 0)
                        {
                            if(first_trait) {
                                first_trait = false;
                                AppendCString(&trait_change_string, "`green`+");
                            }
                            else {
                                AppendCString(&trait_change_string, "");
                            }
                            AppendCString(&trait_change_string, "`lt_blue`%dF", required_diff.focus);
                        }
                        if(required_diff.armor > 0)
                        {
                            if(first_trait) {
                                first_trait = false;
                                AppendCString(&trait_change_string, "`green`+");
                            }
                            else {
                                AppendCString(&trait_change_string, "");
                            }
                            AppendCString(&trait_change_string, "`gold`%dA", required_diff.armor);
                        }

                        TextLayout trait_change_text_layout = c::small_text_layout;
                        trait_change_text_layout.font_size = 32;
                        trait_change_text_layout.align = c::align_rightcenter;
                        DrawTextMultiline(trait_change_text_layout, RectRightCenter(padded_button_rect), trait_change_string);
                    }

                    // Draw ability card if hovered and alt/LMB is down
                    if(response.hovered and (Down(vk::alt) or Down(vk::LMB)))
                    {
                        SetDrawDepth(c::ability_card_draw_depth);
                        DrawAbilityInfoBox(MousePos(), ability_id, tier, c::align_topleft);
                    }

                    pen.y -= (1 + response.rect.size.y); // 1 + so the button outlines don't overlap
                }
            }
        }
    }

    { // While the player turn is ending, tick the timer that makes the end turn button turn red temporarily.
        if(battle->ending_player_turn and Tick(&battle->end_button_clicked_timer))
        {
            if(battle->is_player_turn)
            {
                battle->ending_player_turn = false;
                battle->is_player_turn = false;
            }
        }
    }

    if(!battle->is_player_turn)
    {
        for(int i=0; i<battle->intents.count; ++i)
        {
            Id caster_id = battle->units.ids[i];
            Unit *caster = GetUnitFromId(caster_id);
            if(!ValidUnit(caster) or caster->cur_traits.vigor <= 0) continue;

            auto &intent = battle->intents[i];
            //Intent intent = battle->intents[i];
            //if(!ValidUnit(intent.caster) or !ValidAbility(intent.ability)) continue;
            Array events = GenerateBattlePreviewEvent(battle, intent);
            //BattleEvent event = GenerateBattlePreviewEvent(battle, intent);
            for(auto event : events)
            {
                Unit *target = GetUnitFromId(event.target_id);
                if(!ValidUnit(target)) continue;

                target->cur_traits += event.trait_changes;
            }

        //           ApplyAbilityToUnitSet(caster, *intent.ability, intent.targets);
        }

        battle->is_player_turn = true;

        // Enemy AI (# of permutations)
        UnitSet active_unitset = {};
        UnitSet other_unitset  = {};
        for(Id<Unit> unit_id : battle->units)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;
            if(unit->cur_traits.vigor <= 0) continue;

            if(     unit->team == Team::allies)  AddUnitToUnitSet(unit_id, &active_unitset);
            else if(unit->team == Team::enemies) AddUnitToUnitSet(unit_id, &other_unitset);
        }
        //battle->best_choice_string = DoAiStuff(active_unitset, other_unitset, &battle->arena);

        for(Id unit_id : battle->units)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit)) continue;

            unit->cur_action_points = unit->max_action_points;
        }

        GenerateEnemyIntents(battle);
    }

    { // Automatically end the turn if no ally unit has remaining action points.
        bool any_ally_has_ap = false;
        for(Id unit_id : battle->units)
        {
            Unit *unit = GetUnitFromId(unit_id);
            if(!ValidUnit(unit) or unit->cur_traits.vigor <= 0) continue;

            if(unit->team == Team::allies and unit->cur_action_points > 0)
            {
                any_ally_has_ap = true;
                break;
            }
        }

        if(!any_ally_has_ap) battle->ending_player_turn = true;
    }

    // DrawEnemyIntents(battle);
    // // if(Down(vk::alt))
    // // {
    // //   battle->show_action_preview = true;
    // //   battle->previewed_intent = intent;
    // // }

    // Battle Score
    Array<TraitSet> ally_traitsets  = CreateTempArray<TraitSet>(4);
    Array<TraitSet> enemy_traitsets = CreateTempArray<TraitSet>(4);
    for(Id<Unit> unit_id : battle->units)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        if     (unit->team == Team::allies)  ally_traitsets  += unit->cur_traits;
        else if(unit->team == Team::enemies) enemy_traitsets += unit->cur_traits;
    }
    //float battle_state_score = ScoreBattleState(ally_traitsets, enemy_traitsets);

    //DrawUiText(c::def_text_layout, {0.f, )

    DrawTextMultiline(c::small_text_layout, {}, "%.*s",
                      battle->best_choice_string.length, battle->best_choice_string.data);

    BattleState battle_state = {};
    // Check if all allies are dead.
    bool any_ally_is_alive = false;
    bool any_enemy_is_alive = false;
    for(auto unit_id : battle->units)
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

    return battle_state;
}