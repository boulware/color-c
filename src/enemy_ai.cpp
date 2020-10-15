#include "enemy_ai.h"

#include "array.h"

// struct AiEvent
// {

// };

// 1) Enumerate each action and assign it a number. (Initially we can just look at enemy turn, without considering friendly turn)
// 2) Generate all permutations of an integer sequence containing all of those numbers.
// 3) For each permutation of numbers, calculate the final state.

// active_unitset contains all units for which actions are being considered.
// other_unitset contains the remaining units in the battle.
String
DoAiStuff(UnitSet active_unitset, UnitSet other_unitset, Arena *arena) // @TODO: BigArena?
{
    TIMED_BLOCK;

    String best_choice_string = AllocStringDataFromArena(1000, arena);

    int team_counts[2] = {};
    for(auto unit_id : active_unitset)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        if(     unit->team == Team::allies)  team_counts[(size_t)Team::allies]  += 1;
        else if(unit->team == Team::enemies) team_counts[(size_t)Team::enemies] += 1;
    }
    for(auto unit_id : other_unitset)
    {
        Unit *unit = GetUnitFromId(unit_id);
        if(!ValidUnit(unit)) continue;

        if(     unit->team == Team::allies)  team_counts[(size_t)Team::allies]  += 1;
        else if(unit->team == Team::enemies) team_counts[(size_t)Team::enemies] += 1;
    }

    Array<Array<u8>> arrays = CreateTempArray<Array<u8>>(4);

    UnitSet all_unitset = CombineUnitSets(&active_unitset, &other_unitset);
    Array<Unit> all_units = CreateTempArray<Unit>(all_unitset.size);

    // Confirm that all units are valid.
    for(int i=0; i<all_unitset.size; ++i)
    {
        Id unit_id = all_unitset.ids[i];
        Unit *unit = GetUnitFromId(unit_id);
        all_units += *unit;
        if(!ValidUnit(unit))
        {
            AppendCString(&best_choice_string, __FUNCTION__ "Not all units were valid. Aborting.");
            return best_choice_string;
        }
    }

    // Array of "actions", which correspond to the values in the permutation (0... 25 or whatever)
    // This should contain:
    //     1) a specific Effect (which has an EffectType and an traitset damage/heal amount)
    //     2) a specific target (as a unit index into current_traitsets)
    Array<AiAction> actions = CreateTempArray<AiAction>(50);

    s64 total_option_count = 0; // Number of possible actions that should be permutated

    u8 cur_option_counter = 0;
    s64 permutation_count = m::Factorial(active_unitset.size); // Incomplete value. We'll add a product_of_options
    s64 product_of_options = 1;

    for(int i=0; i<active_unitset.size; ++i)
    {
        Id caster_id = active_unitset.ids[i];
        Unit *caster = GetUnitFromId(caster_id);
        if(!ValidUnit(caster)) continue;

        u8 option_count_for_this_unit = 0;
        for(auto ability_id : caster->ability_ids)
        {
            Ability *ability = GetAbilityFromId(ability_id);
            if(!ValidAbility(ability)) continue; // Skip invalid abilities

            int tier_index = DetermineAbilityTier(caster_id, ability_id);
            if(tier_index < 0) continue; // Skip abilities which can't be used with current traits

            AbilityTier &tier = ability->tiers[tier_index]; // alias

            // Determine number of possible options there are for this ability,
            // depending on the target_class and what units are in the battle.

            AiAction action = {};
            action.ability = *ability;
            action.caster_index = i;

            if(tier.target_class == TargetClass::self)
            {
                option_count_for_this_unit += 1;

                action.target_count = 1;
                action.unit_indices[0] = i;

                actions += action;
            }
            else if(tier.target_class == TargetClass::single_ally)
            {
                option_count_for_this_unit += team_counts[(size_t)caster->team];

                action.target_count = 1;
                int actions_added = 0;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(caster->team == all_units[unit_index].team)
                    {
                        action.unit_indices[0] = unit_index;
                        actions += action;
                        ++actions_added;
                    }
                }

                Assert(actions_added == team_counts[(size_t)caster->team]);
            }
            else if(tier.target_class == TargetClass::single_ally_not_self)
            {
                option_count_for_this_unit += (team_counts[(size_t)caster->team] - 1);

                action.target_count = 1;
                int actions_added = 0;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(unit_index != i and caster->team == all_units[unit_index].team)
                    {
                        action.unit_indices[0] = unit_index;
                        actions += action;
                        ++actions_added;
                    }
                }

                Assert(actions_added == team_counts[(size_t)caster->team]);
            }
            else if(tier.target_class == TargetClass::all_allies)
            {
                option_count_for_this_unit += 1;

                //action.target_count = option_count_for_this_unit;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(caster->team == all_units[unit_index].team)
                        action.unit_indices[action.target_count++] = unit_index;
                }
                Assert(action.target_count == team_counts[(size_t)caster->team]);

                actions += action;
            }
            else if(tier.target_class == TargetClass::all_allies_not_self)
            {
                option_count_for_this_unit += 1;

                //action.target_count = option_count_for_this_unit;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(unit_index != i and caster->team == all_units[unit_index].team)
                        action.unit_indices[action.target_count++] = unit_index;
                }
                Assert(action.target_count == team_counts[(size_t)caster->team]);

                actions += action;
            }
            else if(tier.target_class == TargetClass::single_enemy)
            {
                size_t opposite_team_index;
                if(caster->team == Team::allies) opposite_team_index = (size_t)Team::enemies;
                else                             opposite_team_index = (size_t)Team::allies;

                option_count_for_this_unit += team_counts[opposite_team_index];

                action.target_count = 1;
                int actions_added = 0;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(caster->team != all_units[unit_index].team)
                    {
                        action.unit_indices[0] = unit_index;
                        actions += action;
                        ++actions_added;
                    }
                }

                Assert(actions_added == team_counts[(size_t)caster->team]);
            }
            else if(tier.target_class == TargetClass::all_enemies)
            {
                size_t opposite_team_index;
                if(caster->team == Team::allies) opposite_team_index = (size_t)Team::enemies;
                else                             opposite_team_index = (size_t)Team::allies;

                option_count_for_this_unit += 1;

                //action.target_count = option_count_for_this_unit;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(caster->team != all_units[unit_index].team)
                        action.unit_indices[action.target_count++] = unit_index;
                }
                Assert(action.target_count == team_counts[opposite_team_index]);

                actions += action;
            }
            else if(tier.target_class == TargetClass::single_unit)
            {
                option_count_for_this_unit += (team_counts[0] + team_counts[1]);

                action.target_count = 1;
                int actions_added = 0;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    action.unit_indices[0] = unit_index;
                    actions += action;
                    ++actions_added;
                }

                Assert(actions_added == (team_counts[0] + team_counts[1]));
            }
            else if(tier.target_class == TargetClass::single_unit_not_self)
            {
                option_count_for_this_unit += (team_counts[0] + team_counts[1] - 1);

                action.target_count = 1;
                int actions_added = 0;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    if(i == unit_index) continue; // Skip self

                    action.unit_indices[0] = unit_index;
                    actions += action;
                    ++actions_added;
                }

                Assert(actions_added == (team_counts[0] + team_counts[1] - 1));
            }
            else if(tier.target_class == TargetClass::all_units)
            {
                option_count_for_this_unit += 1;

                //action.target_count = option_count_for_this_unit;
                for(int unit_index = 0; unit_index < all_units.count; ++unit_index)
                {
                    action.unit_indices[action.target_count++] = unit_index;
                }
                Assert(action.target_count == all_units.count);

                actions += action;
            }
        }

        Array<u8> sub_array = CreateTempArray<u8>(8);
        for(u8 i=0; i<option_count_for_this_unit; ++i)
        {
            sub_array += (u8)(cur_option_counter + i);
        }
        arrays += sub_array;

        cur_option_counter += option_count_for_this_unit;
        total_option_count += option_count_for_this_unit;
        product_of_options *= option_count_for_this_unit;
    }

    permutation_count *= product_of_options;
    Log("%zu", permutation_count);

    int length_of_one_permutation = active_unitset.size; // 1234, 1235, 1236, etc.
    size_t permutation_values_byte_count = length_of_one_permutation * permutation_count;
    u8 *permutation_values = (u8 *)platform->AllocateMemory(permutation_values_byte_count);
    u8 *p = permutation_values;


    GenerateU8Permutations(arrays, permutation_values, permutation_values_byte_count);

    // Methods we might need:
    // ...
    // CalculateAdjustedDamage()
    //



    // [current_traitsets] will be parallel to [all_unitset]
    Array<TraitSet> initial_traitsets = CreateTempArray<TraitSet>(active_unitset.size + other_unitset.size);
    Array<TraitSet> current_traitsets = CreateTempArray<TraitSet>(active_unitset.size + other_unitset.size);
    Array<TraitSet> traitset_changes  = CreateTempArray<TraitSet>(active_unitset.size + other_unitset.size);
    Array<TraitSet> max_traitsets     = CreateTempArray<TraitSet>(active_unitset.size + other_unitset.size);
    for(int i=0; i<all_unitset.size; ++i)
    {
        Id unit_id = all_unitset.ids[i];
        Unit *unit = GetUnitFromId(unit_id);
        initial_traitsets += unit->cur_traits;
        max_traitsets += unit->max_traits;
    }
    for(auto e : current_traitsets)
    {
        current_traitsets += e;
        traitset_changes += {};
    }


    Array<TraitSet> ally_traitsets = CreateTempArray<TraitSet>(c::max_party_size);
    Array<TraitSet> enemy_traitsets = CreateTempArray<TraitSet>(c::max_party_size);

    Array<TraitSet> ally_traitset_changes = CreateTempArray<TraitSet>(c::max_party_size);
    Array<TraitSet> enemy_traitset_changes = CreateTempArray<TraitSet>(c::max_party_size);

    Array<TraitSet> max_ally_traitsets = CreateTempArray<TraitSet>(c::max_party_size);
    Array<TraitSet> max_enemy_traitsets = CreateTempArray<TraitSet>(c::max_party_size);
    for(int i=0; i<active_unitset.size; ++i)
    {
        max_ally_traitsets += max_traitsets[i];
    }
    for(int i=0; i<other_unitset.size; ++i)
    {
        // if(current_traitsets[active_unitset.size + i].vigor < 0)
        // {
        //     Log("tick");
        // }
        max_enemy_traitsets += max_traitsets[active_unitset.size + i];
    }

    int permutation_counter = 0;
    float best_score = -1000.f;
    int best_permutation_index = -1;
    int equivalent_line_count = 0;
    for(int i=0; i<permutation_count; ++i)
    {
        ++permutation_counter;
        traitset_changes.count = 0;
        for(auto e : current_traitsets) traitset_changes += {};

        for(int j=0; j<length_of_one_permutation; ++j)
        {
            size_t action_index = permutation_values[length_of_one_permutation*i + j];

            AiAction &cur_action = actions[action_index]; // alias

            int tier_index = -1;
            { // Determine ability tier
                //cur_action.ability;
                //TraitSet cur_traits = current_traitsets[cur_action.caster_index];

                for(int i=cur_action.ability.tiers.count-1; i>=0; --i)
                {
                    if(current_traitsets[cur_action.caster_index] >= cur_action.ability.tiers[i].required_traits)
                    {
                        tier_index = i;
                        break;
                    }
                }

                if(tier_index == -1) break;
            }
            AbilityTier &cur_ability_tier = cur_action.ability.tiers[tier_index]; // alias

            // Apply each effect to each target
            for(Effect effect : cur_ability_tier.effects_)
            {
                for(int unit_indices_index=0; unit_indices_index<cur_action.target_count; ++unit_indices_index)
                {
                    int target_index = cur_action.unit_indices[unit_indices_index];

                    TraitSet trait_changes = {};

                    if(effect.type == EffectType::NoEffect)
                    {
                        continue;
                    }
                    else if(effect.type == EffectType::Damage)
                    {
                        EffectParams_Damage *effect_params = (EffectParams_Damage*)effect.params;

                        trait_changes = CalculateAdjustedDamage(current_traitsets[target_index], effect_params->amount);
                    }
                    else if(effect.type == EffectType::DamageIgnoreArmor)
                    {
                        EffectParams_DamageIgnoreArmor *effect_params = (EffectParams_DamageIgnoreArmor*)effect.params;

                        TraitSet cur_traits_with_no_armor = current_traitsets[target_index];
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
                            trait_changes[i] = m::Min(current_traitsets[target_index][i], base_gift_amount[i]);
                        }

                        // Caster change
                        current_traitsets[cur_action.caster_index] -= trait_changes;
                        //events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
                    }
                    else if(effect.type == EffectType::Steal)
                    {
                        EffectParams_Steal *effect_params = (EffectParams_Steal*)effect.params;
                        TraitSet base_steal_amount = effect_params->amount;

                        // Adjust steal amount such that you can only steal as much trait as the target actually has.
                        // e.g., Stealing 5 armor from a target with only 3 armor will only give the caster +3 armor and the target -3 armor
                        for(int i=0; i<c::trait_count; i++)
                        {
                            trait_changes[i] = -m::Min(current_traitsets[target_index][i], base_steal_amount[i]);
                        }


                        // Caster change
                        current_traitsets[cur_action.caster_index] -= trait_changes;
                        //events += BattleEvent{.caster_id=caster_id, .target_id=caster_id, .trait_changes = -trait_changes};
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
                    // if(trait_changes != TraitSet{})
                    // {
                    //     events += BattleEvent{.caster_id=caster_id, .target_id=target_id, .trait_changes=trait_changes};
                    // }
                    //current_traitsets[cur_action.]
                    current_traitsets[target_index] += trait_changes;
                    traitset_changes[target_index] = trait_changes;
                }
            }
        }

        ally_traitsets.count = 0;
        enemy_traitsets.count = 0;
        ally_traitset_changes.count = 0;
        enemy_traitset_changes.count = 0;
        for(int i=0; i<active_unitset.size; ++i)
        {
            ally_traitsets += initial_traitsets[i];
            ally_traitset_changes += traitset_changes[i];
        }
        for(int i=0; i<other_unitset.size; ++i)
        {
            // if(current_traitsets[active_unitset.size + i].vigor < 0)
            // {
            //     Log("tick");
            // }
            enemy_traitsets += initial_traitsets[active_unitset.size + i];
            enemy_traitset_changes += traitset_changes[active_unitset.size + i];
        }
        //if(i == 12802)
        if(    permutation_values[length_of_one_permutation*i + 0] == 6
           and permutation_values[length_of_one_permutation*i + 1] == 27
           //and permutation_values[length_of_one_permutation*i + 2] == 30
           and permutation_values[length_of_one_permutation*i + 3] == 48)
        {
            Log("tick");
        }
        float score = ScoreBattleState(ally_traitsets, enemy_traitsets,
                                       ally_traitset_changes, enemy_traitset_changes,
                                       max_ally_traitsets, max_enemy_traitsets);
        if(score == best_score)
            ++equivalent_line_count;

        if(score > best_score)
        {
            best_score = score;
            best_permutation_index = i;
            equivalent_line_count = 0;
        }

        // Reset current_traitsets
        current_traitsets.count = 0;
        for(auto e : initial_traitsets)
            current_traitsets += e;
    }

    u8 *permutation_start = &permutation_values[length_of_one_permutation*best_permutation_index];

    AppendCString(&best_choice_string,
        "Best score: %.2f [%u %u %u %u] (%d equivalent lines) [permutation index = %zu]\n",
        best_score,
        permutation_start[0], permutation_start[1], permutation_start[2], permutation_start[3],
        equivalent_line_count,
        best_permutation_index);

    for(int i=0; i<length_of_one_permutation; ++i)
    {
        u8 option_index = permutation_start[i];
        AiAction action = actions[option_index];
        Unit caster = all_units[action.caster_index];
        Unit first_target = all_units[action.unit_indices[0]];

        AppendCString(&best_choice_string,
            "\"%.*s\" uses \"%.*s\" on \"%.*s\" [%d]\n",
            caster.name.length, caster.name.data,
            action.ability.name.length, action.ability.name.data,
            first_target.name.length, first_target.name.data,
            action.unit_indices[0]);
    }

    //Log("%d", current_traitsets.count);
    // String permutation_string = {};
    // permutation_string.data = (char *)platform->AllocateMemory(Megabyte(100));
    // permutation_string.max_length = Megabyte(100);

    //String permutation_string = AllocStringDataFromArena(100, &memory::per_frame_arena);
    // for(int i=0; i<permutation_count; ++i)
    // {
    //     permutation_string.length = 0;
    //     for(int j=0; j<length_of_one_permutation; ++j)
    //     {
    //         AppendCString(&permutation_string, "%02u ", p[j]);
    //     }
    //     //AppendChar(&permutation_string, '\0');

    //     p += length_of_one_permutation;
    // }

    /*
    int permutation_length = 4;
    Array<Array<u8>> arrays = CreateTempArray<Array<u8>>(permutation_length);
        Array<u8> sub_array1 = CreateTempArray<u8>(3);
            sub_array1 += (u8)0;
            arrays += sub_array1;
        Array<u8> sub_array2 = CreateTempArray<u8>(2);
            sub_array2 += (u8)2;
            arrays += sub_array2;
        Array<u8> sub_array3 = CreateTempArray<u8>(2);
            sub_array3 += (u8)4;
            sub_array3 += (u8)5;
            arrays += sub_array3;
        Array<u8> sub_array4 = CreateTempArray<u8>(2);
            sub_array4 += (u8)6;
            arrays += sub_array4;

    // Determine permutation count
    int permutation_count = m::Factorial(permutation_length); // temporary value. sum of products will be multiplied into this below this.
    int product_of_options = 1;
    for(auto array : arrays)
    {
        product_of_options *= array.count;
    }
    permutation_count *= product_of_options;

    // Allocate array to place permutations
    int dst_byte_count = permutation_count * permutation_length;
    u8 *dst = (u8 *)malloc(dst_byte_count);
    u8 *p = dst;
    GenerateU8Permutations(arrays, dst, dst_byte_count);
    String permutation_string = AllocStringDataFromArena(100, &memory::per_frame_arena);
    for(int i=0; i<permutation_count; ++i)
    {
        permutation_string.length = 0;
        for(int j=0; j<permutation_length; ++j)
        {
            AppendCString(&permutation_string, "%u", p[j]);
        }
        //AppendChar(&permutation_string, '\0');

        Log("%.*s", permutation_string.length, permutation_string.data);
        p += permutation_length;
    }
    free(dst);
    */

    platform->FreeMemory(permutation_values);
    //platform->FreeMemory(permutation_string.data);
    return best_choice_string;
    //Log("permutation count: %zu", permutation_count);

    // Array<Unit> allies;
    // Array<Unit>
    // for(int i=0; i<ai_units.size; ++i)
    // {
    //     Unit *ai_unit = GetUnitFromId(ai_units.ids[i]);
    //     if(!ValidUnit(ai_unit)) continue;


    // }
}

// For scoring, what data do we need?

// 1) cur_traits for each unit in the battle
// ... That's it, as long as we know which units are allies vs enemies
// Of course, at some point, we may want to add some other heuristics,
// such as which skills tiers are active (which could be rolled into some kind of
// damage/threat potential)...
//        e.g., if in one state, an enemy has an ability that can deal 12 damage to all units,
//              but in another state, that same enemy ability can only deal 6 damage to a single
//              unit, you could shift the score by the difference in potential damage output.
//              So, here... the score would shift in favor of allies by (12*number of ally units) - 6
//              = 48 - 6 = +42 [for 4 ally units]. This is sort of a "cheaty" way to look ahead one turn,
//              because instead of calculating all permutations, we just calculate on average how
//              much the trait values could change if the enemy uses this ability.
//              ...
//              Similarly, killing a unit might shift the score by the damage amount of their currently
//              highest damage output ability.
//
// if units are dead, etc.. But for now, we'll just compare
// the sum total of traits for allies vs enemies and give a single score based on that.
// For now, likely SCORE = Sum(ally traits) - Sum(enemy_traits)

float
ScoreBattleState(Array<TraitSet> initial_ally_traitsets,
                 Array<TraitSet> initial_enemy_traitsets,
                 Array<TraitSet> ally_traitset_changes,
                 Array<TraitSet> enemy_traitset_changes,
                 Array<TraitSet> max_ally_traitsets,
                 Array<TraitSet> max_enemy_traitsets)
{
    float score = 0.f;

    for(int i=0; i<initial_ally_traitsets.count; ++i)
    {
        for(int j=0; j<c::trait_count; ++j)
            score += ((float)(ally_traitset_changes[i][j]) / (float)max_ally_traitsets[i][j]);
    }

    for(int i=0; i<initial_enemy_traitsets.count; ++i)
    {
        if(initial_enemy_traitsets[i].vigor + enemy_traitset_changes[i].vigor <= 0)
        {
            score += 10.f;
            continue;
        }

        for(int j=0; j<c::trait_count; ++j)
            score -= ((float)(enemy_traitset_changes[i][j])/ (float)max_enemy_traitsets[i][j]);

    }

    return score;
}