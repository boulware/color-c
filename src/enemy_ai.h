#ifndef ENEMY_AI_H
#define ENEMY_AI_H

namespace ai
{
    // Trait change weights
    float wt_vigor = 2.0f;
    float wt_focus = 0.0f;
    float wt_armor = 1.0f;

    float wt_rel_change = 0.7f;
    float wt_abs_change = 0.1f;

    // Ability potential weights
    float wt_ability_potential = 1.5f;
    float wt_progression = 0.2f;
}

struct BattleScore
{
    float total;

    float rel_change;
    float abs_change;
    float ally_potential;
    float enemy_potential;
};

// "actions", which correspond to the values in the permutation (0... 25 or whatever)
// This should contain:
//     1) a specific Ability (which has tiers, and each tier has a set of effects)
//     2) a specific set of targets (as unit indices into current_traitsets)
struct AiAction
{
    Ability ability;
    u8 caster_index;
    u8 target_count;
    u8 unit_indices[c::max_target_count];
};

String DoAiStuff(UnitSet active_unitset, UnitSet other_unitset, Arena *arena);
float ScoreBattleState(Array<TraitSet> ally_traitsets,
                       Array<TraitSet> enemy_traitsets,
                       Array<TraitSet> ally_traitset_changes,
                       Array<TraitSet> enemy_traitset_changes,
                       Array<TraitSet> max_ally_traitsets,
                       Array<TraitSet> max_enemy_traitsets);

BattleScore
ScoreBattleState2(Array<Unit> ally_units,
                  Array<Unit> enemy_units,
                  Array<TraitSet> ally_traitset_changes,
                  Array<TraitSet> enemy_traitset_changes);

#endif