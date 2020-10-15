#ifndef ENEMY_AI_H
#define ENEMY_AI_H

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

#endif