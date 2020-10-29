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

    Id<Unit> selected_unit_id;
    Id<Ability> selected_ability_id; // @note: be careful that any time you deselect a unit, you also clear these ability pointers

    Array<UnitId> units;
    Array<Intent> preview_intents;

    //bool is_player_turn;
    OscillatingTimer preview_damage_timer;
    Timer end_player_turn_timer;
    //bool ending_player_turn;

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
void DrawAbilityInfoBox(Vec2f pos, Id<Ability> ability_id, int tier, Align align = c::align_topleft);

void UpdateHoveredUnit(Battle *battle);
void UpdateHoveredAbility(Battle *battle);

void GenerateEventsFromIntent(Intent intent, Array<BattleEvent> *events, Table<Unit> unit_table = g::unit_table);

void ApplyBattleEvent(BattleEvent event);

#endif