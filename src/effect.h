#ifndef EFFECT_H
#define EFFECT_H

enum class EffectType : u8
{
	NoEffect,
	Damage,
	DamageIgnoreArmor,
	Restore,
	Gift,
	Steal,
};

struct Effect
{
	// Examples of effects:
	//		Deal 5 vigor damage
	//		Steal 1 armor
	//		Lifelink (possibly redundantt with Steal)
	//		Ignores armor

	EffectType type;
	void *params;
};

struct EffectParams_Damage
{
	TraitSet amount;
};

struct EffectParams_DamageIgnoreArmor
{
	TraitSet amount;
};

struct EffectParams_Restore
{
	TraitSet amount;
};

struct EffectParams_Gift
{
	TraitSet amount;
};

struct EffectParams_Steal
{
	TraitSet amount;
};

float EffectPotential(Effect effect, TargetClass target_class);

#endif