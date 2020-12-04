#include "ability_button.h"

ButtonResponse
DrawAbilityButton(Rect rect, Id<Unit> caster_id, Id<Ability> ability_id, bool show_intent, Align align)
{
    //Rect aligned_rect = AlignRect(rect, align);

    //DrawUnfilledRect(aligned_rect, c::white);

    // new
    ///////////////////////////////
    // old

    //Id ability_id = unit->ability_ids[ability_index];

    ButtonResponse response = {};

    Ability *ability = GetAbilityFromId(ability_id);
    if(!ValidAbility(ability)) return response;
    Unit *caster = GetUnitFromId(caster_id);
    if(!ValidUnit(caster)) return response;

    Vec2f pen = rect.pos;

    int tier = DetermineAbilityTier(caster_id, ability_id);

    ButtonLayout ability_icon_button_layout = {};
    if(tier <= 0)        ability_icon_button_layout = c::cannot_use_ability_button_layout;
    else if(show_intent) ability_icon_button_layout = c::intented_button_layout;
    else                 ability_icon_button_layout = c::can_use_ability_button_layout;

    SetDrawDepth(c::field_draw_depth);
    response = DrawButton(ability_icon_button_layout,
                          {pen, c::ability_icon_size},
                          ability->name);

    Vec2f padding = {4.f,4.f};
    Rect padded_button_rect = {
        .pos = response.rect.pos + padding,
        .size = response.rect.size - 2*padding
    };


    // Tier string. e.g., "Tier: 2/3"
    Color tier_color = c::grey;
    int max_tier = ability->tiers.count - 1;
    if(tier == max_tier)          tier_color = c::gold;
    else if(tier == max_tier - 1) tier_color = c::silver;
    else if(tier == max_tier - 2) tier_color = c::bronze;

    String tier_string = AllocStringDataFromArena(20, memory::per_frame_arena_id);
    AppendCString(&tier_string, "%d/%d", tier, max_tier);
    TextLayout text_layout = ability_icon_button_layout.label_layout;
    text_layout.align = c::align_bottomleft;
    text_layout.color = tier_color;
    Vec2f tier_string_size = SizeText(text_layout, tier_string);
    Vec2f tier_string_pos = AlignSizeInRect(tier_string_size, padded_button_rect, text_layout.align);
    DrawText(text_layout, tier_string_pos, tier_string);

    // Trait requirement bars on the button
    if(tier != max_tier)
    {
        AbilityTier &cur_tier  = ability->tiers[tier];   // alias
        AbilityTier &next_tier = ability->tiers[tier+1]; // alias

        TraitSet trait_ranges = next_tier.required_traits - cur_tier.required_traits; // The number of total pips
        TraitSet cur_traits_in_range = caster->cur_traits - cur_tier.required_traits; // How many pips to color in within current range
        for(int i=0; i<c::trait_count; ++i)
        { // clamp between 0 and trait_range
            cur_traits_in_range[i] = m::Clamp(cur_traits_in_range[i], 0, trait_ranges[i]);
        }

        Vec2f req_bar_origin = RectTopRight(padded_button_rect);
        // Drawn in "opposite" trait order so from left to right it is vigor, focus, armor
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{  0.f,0.f}, {-20.f,padded_button_rect.size.y}}, c::armor_color, c::bg_armor_color, cur_traits_in_range.armor, trait_ranges.armor);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{-20.f,0.f}, {-20.f,padded_button_rect.size.y}}, c::focus_color, c::bg_focus_color, cur_traits_in_range.focus, trait_ranges.focus);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{-40.f,0.f},  {-20.f,padded_button_rect.size.y}}, c::vigor_color, c::bg_vigor_color, cur_traits_in_range.vigor, trait_ranges.vigor);
    }
    else
    { // Show excess as a req. bar (or a number if it's 'large' [10+?])
        AbilityTier &cur_tier = ability->tiers[tier]; // alias

        TraitSet excess_traits = caster->cur_traits - cur_tier.required_traits;
        for(int i=0; i<c::trait_count; ++i)
        {
            if(cur_tier.required_traits[i] == 0)
                excess_traits[i] = 0;
        }

        Vec2f req_bar_origin = RectTopRight(padded_button_rect);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{  0.f,0.f}, {-20.f,padded_button_rect.size.y}}, c::armor_color, c::bg_armor_color,  excess_traits.armor, excess_traits.armor);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{-20.f,0.f}, {-20.f,padded_button_rect.size.y}}, c::focus_color, c::bg_focus_color,  excess_traits.focus, excess_traits.focus);
        DrawVerticalNotchedHealthbar({req_bar_origin+Vec2f{-40.f,0.f},  {-20.f,padded_button_rect.size.y}}, c::vigor_color, c::bg_vigor_color, excess_traits.vigor, excess_traits.vigor);
    }

    // Draw ability card if hovered and alt/LMB is down
    if(response.hovered and (Down(vk::alt) or Down(vk::LMB)))
    {
        SetDrawDepth(c::ability_card_draw_depth);
        DrawAbilityInfoBox(MousePos(), caster_id, ability_id, c::align_topleft);
    }

    return response;
}