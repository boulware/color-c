#ifndef INTENT_H
#define INTENT_H

#include "table.h"

struct Intent
{
    Id<Unit> caster_id;
    Id<Ability> ability_id;
    Array<UnitId> target_set;
    u8 position; // (contextual) position in action sequence for multiple units. Used for AI action sequencing.
};

#endif