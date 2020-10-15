#include "enemy_ai.h"

// struct AiEvent
// {

// };

// 1) Enumerate each action and assign it a number. (Initially we can just look at enemy turn, without considering friendly turn)
// 2) Generate all permutations of an integer sequence containing all of those numbers.
// 3) For each permutation of numbers, calculate the final state.

// active_unitset contains all units for which actions are being considered.
// other_unitset contains the remaining units in the battle.
s64
DoAiStuff(UnitSet active_unitset, UnitSet other_unitset) // @TODO: BigArena?
{
    TIMED_BLOCK;

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

    s64 total_option_count = 0; // Number of possible actions that should be permutated

    u8 cur_option_counter = 0;
    s64 permutation_count = m::Factorial(active_unitset.size); // Incomplete value. We'll add a product_of_options
    s64 product_of_options = 1;
    for(auto caster_id : active_unitset)
    {
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
            #if 1
            if(tier.target_class == TargetClass::self)
            {
                option_count_for_this_unit += 1;
            }
            else if(tier.target_class == TargetClass::single_ally)
            {
                option_count_for_this_unit += team_counts[(size_t)caster->team];
            }
            else if(tier.target_class == TargetClass::single_ally_not_self)
            {
                option_count_for_this_unit += (team_counts[(size_t)caster->team] - 1);
            }
            else if(tier.target_class == TargetClass::all_allies)
            {
                option_count_for_this_unit += 1;
            }
            else if(tier.target_class == TargetClass::all_allies_not_self)
            {
                option_count_for_this_unit += 1;
            }
            else if(tier.target_class == TargetClass::single_enemy)
            {
                size_t opposite_team_index;
                if(caster->team == Team::allies) opposite_team_index = (size_t)Team::enemies;
                else                             opposite_team_index = (size_t)Team::allies;

                option_count_for_this_unit += team_counts[opposite_team_index];
            }
            else if(tier.target_class == TargetClass::all_enemies)
            {
                option_count_for_this_unit += 1;
            }
            else if(tier.target_class == TargetClass::single_unit)
            {
                option_count_for_this_unit += (team_counts[0] + team_counts[1]);
            }
            else if(tier.target_class == TargetClass::single_unit_not_self)
            {
                option_count_for_this_unit += (team_counts[0] + team_counts[1] - 1);
            }
            else if(tier.target_class == TargetClass::all_units)
            {
                option_count_for_this_unit += 1;
            }
            #endif
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

    int length_of_one_permutation = active_unitset.size; // 1234, 1235, 1236, etc.
    size_t permutation_values_byte_count = length_of_one_permutation * permutation_count;
    u8 *permutation_values = (u8 *)platform->AllocateMemory(permutation_values_byte_count);
    u8 *p = permutation_values;


    GenerateU8Permutations(arrays, permutation_values, permutation_values_byte_count);
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
    return permutation_count;
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
ScoreBattleState(Array<TraitSet> ally_traitsets, Array<TraitSet> enemy_traitsets)
{
    float score = 0.f;

    for(auto this_traits : ally_traitsets)
    {
        score += this_traits.vigor;
        score += this_traits.focus;
        score += this_traits.armor;
    }

    for(auto this_traits : enemy_traitsets)
    {
        score -= this_traits.vigor;
        score -= this_traits.focus;
        score -= this_traits.armor;
    }

    return score;
}