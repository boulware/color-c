#ifndef INTENT_H
#define INTENT_H

#include "table.h"

struct Intent
{
    Id<Unit> caster_id;
    Id<Ability> ability_id;
    Array<UnitId> target_set;
};

#endif