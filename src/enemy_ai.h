#ifndef ENEMY_AI_H
#define ENEMY_AI_H

s64 DoAiStuff(UnitSet active_unitset, UnitSet other_unitset);
float ScoreBattleState(Array<TraitSet> ally_traitsets, Array<TraitSet> enemy_traitsets);

#endif