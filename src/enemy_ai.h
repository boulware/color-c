#ifndef ENEMY_AI_H
#define ENEMY_AI_H

namespace ai
{
    // Trait change weights
    float wt_rel_change = 1.f;
    float wt_abs_change = 0.1f;

    // Ability potential weights
    float wt_ability_potential = 1.f;
    float wt_pot_upgrade_1 = 1.f;   // Weight for increasing traits that will help achieve the closest tier upgrade.
    float wt_pot_upgrade_2 = 0.1f;  // ''                                                      next to closest tier upgrade.
    float wt_pot_upgrade_3 = 0.01f; // ''                                                      next2 next2 closest tier upgrade.
}

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

float
ScoreBattleState2(Array<Unit> ally_units,
                 Array<Unit> enemy_units,
                 Array<TraitSet> ally_traitset_changes,
                 Array<TraitSet> enemy_traitset_changes);

#endif