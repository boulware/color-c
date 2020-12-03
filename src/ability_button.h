#ifndef ABILITY_BUTTON_H
#define ABILITY_BUTTON_H

ButtonResponse DrawAbilityButton(Rect rect, Id<Unit> caster_id, Id<Ability> ability_id,
                                 bool show_intent, Align align = c::align_topleft);

#endif