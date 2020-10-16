#include "effect.h"

float
EffectPotential(Effect effect, TargetClass target_class)
{
    float potential = 0.f;

    if(effect.type == EffectType::NoEffect)
    {
        // Do nothing: 0.0 potential
        potential = 0.f;
    }
    else if(effect.type == EffectType::Damage)
    {
        int sum = 0;
        for(int i=0; i<c::trait_count; ++i)
            sum += ((EffectParams_Damage *)effect.params)->amount[i];

        potential = (float)sum * TargetClassToPotentialMult(target_class);
    }
    else if(effect.type == EffectType::DamageIgnoreArmor)
    {
        int sum = 0;
        for(int i=0; i<c::trait_count; ++i)
            sum += ((EffectParams_DamageIgnoreArmor *)effect.params)->amount[i];

        potential = (float)sum * TargetClassToPotentialMult(target_class);
    }
    else if(effect.type == EffectType::Restore)
    {
        int sum = 0;
        for(int i=0; i<c::trait_count; ++i)
            sum += ((EffectParams_Restore *)effect.params)->amount[i];

        potential = (float)sum * TargetClassToPotentialMult(target_class);
    }
    else if(effect.type == EffectType::Gift)
    {
        potential = 0.f; // Net 0 if you gift to an ally
        // int sum = 0;
        // for(int i=0; i<c::trait_count; ++i)
        //     sum += ((EffectParams_Gift *)effect.params)->amount[i];

        // potential = (float)sum * TargetClassToPotentialMult(target_class);
    }
    else if(effect.type == EffectType::Steal)
    {
        int sum = 0;
        for(int i=0; i<c::trait_count; ++i)
            sum += ((EffectParams_Steal *)effect.params)->amount[i];

        // 2.f * ... because Steal takes from enemy AND adds to you.
        potential = 2.f * (float)sum * TargetClassToPotentialMult(target_class);
    }

    return potential;
}