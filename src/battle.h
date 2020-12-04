#ifndef BATTLE_H
#define BATTLE_H

#include "draw.h"
#include "game.h"
#include "timer.h"
#include "oscillating_timer.h"
#include "array.h"
#include "unit.h"
#include "game_state.h"
#include "intent.h"

// Data associated with a battle-wide change in state
// (e.g., after an ability would be used, this holds data for trait changes for all units, death, status effects, etc.)
struct BattleEvent
{
    Id<Unit> caster_id;
    Id<Unit> target_id;
    TraitSet trait_changes;
};

enum class BattlePhase : int
{
    invalid,
    start,
    player_turn,
    end_of_player_turn,
    enemy_turn,
    end_of_enemy_turn,
    end,
    COUNT
};

const char *BattlePhase_userstrings[] = {
    "invalid",
    "start",
    "player_turn",
    "end_of_player_turn",
    "enemy_turn",
    "end_of_enemy_turn",
    "end",
};

const char *BattlePhaseAsCString(BattlePhase battle_phase);

struct BattleState
{
    bool finished;
};

struct Battle
{
    PoolId<Arena> arena_id;
    PoolId<Arena> ai_arena_id;

    Rect hud;

    BattlePhase phase;

    UnitId selected_unit_id;
    AbilityId selected_ability_id;

    Array<UnitId> unit_slots;
    Array<Vec2f> slot_positions;

    //UnitId unit_slots[c::battle_unit_slot_count];
    //Intent unit_intents[c::battle_unit_slot_count];
    //Vec2f slot_positions[c::battle_unit_slot_count];

    Array<Intent> preview_intents;

    OscillatingTimer preview_damage_timer;
    Timer end_player_turn_timer;

    String best_choice_string;
};

void InitBattle(Battle *battle, PoolId<Arena> arena_id);
BattleState TickBattle(Battle *battle);

//void GenerateEventsFromIntent(Intent intent, Array<BattleEvent> *events);
void IntentTraitChangesForUnits(Array<Intent> intents, Array<UnitId> unit_ids, Array<TraitSet> *traitset_changes);

void DrawUnits(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawUnitHudData(Battle *battle);
void DrawTargetingInfo(Battle *battle);
void DrawAbilityInfoBox(Vec2f pos, Id<Unit> caster_id, Id<Ability> ability_id, Align align = c::align_topleft);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

void GenerateEventsFromIntent(Intent intent, Array<BattleEvent> *events, Table<Unit> unit_table = g::unit_table);

void ApplyBattleEvent(BattleEvent event);

#endif