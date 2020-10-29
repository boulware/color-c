#include "unit.h"

#include "util.h"
#include "text_parsing.h"

// Unit *UnitAtIndex(UnitSet set, int index)
// {
//  if(index < 0 or index >= set.size) return nullptr;

//  return set.units[index];
// }

bool
UnitIsDead(UnitId unit_id)
{
    Unit *unit = GetUnitFromId(unit_id);
    if(!ValidUnit(unit) or unit->cur_traits.vigor <= 0) return true;
    else return false;
}

bool
ParseNextAsBreedData(Buffer *buffer, Breed *breed, Table<Ability> ability_table)
{
    if(!buffer or !breed) return false;

    bool valid_unit_data = true;
    char *initial = buffer->p;
    Breed temp_breed = {};
    for(int i=0; i<c::moveset_max_size; i++)
    {
        temp_breed.ability_ids[i] = c::null_ability_id;
    }

    bool header_valid = ConfirmNextTokenAsIdentifier(buffer, "unit");
    if(!header_valid)
    {
        // The current location in the buffer doesn't point to the beginning of a unit's data.
        valid_unit_data = false;
    }

    Token name_token;
    bool is_valid_name = NextTokenAsDoubleQuotedString(buffer, &name_token);
    if(!is_valid_name)
    {
        // There is no name given or the string is misformatted for the unit's name.
        valid_unit_data = false;
    }

    bool end_of_unit_data = false;
    while(valid_unit_data and !end_of_unit_data)
    {
        char *before_token = buffer->p;
        Token token = NextToken(buffer);

        if(BufferBytesRemaining(*buffer) == 0 or TokenMatchesString(token, "unit"))
        {
            // reset buffer to before the previous token was fetched, since it's not part of the current
            // ability data
            buffer->p = before_token;
            end_of_unit_data = true;
        }
        else if(TokenMatchesString(token, "traits"))
        {
            valid_unit_data = ParseNextAsTraitSet(buffer, &temp_breed.max_traits);
        }
        else if(TokenMatchesString(token, "moveset"))
        {
            // Look for *up to* moveset_max_size strings of ability names
            for(int i=0; i<c::moveset_max_size; i++)
            {
                Token ability_name_token;
                if(NextTokenAsDoubleQuotedString(buffer, &ability_name_token))
                {
                    temp_breed.ability_ids[i] = GetIndexFromName(ability_table, StringFromToken(ability_name_token));

                    if(temp_breed.ability_ids[i] == c::null_ability_id)
                    {
                        // Found an ability name string for moveset, but it's not an ability
                        // that exists in the ability table.
                        valid_unit_data = false;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else if(TokenMatchesString(token, "tier"))
        {
            valid_unit_data = ParseNextAsS32(buffer, &temp_breed.tier);
        }
        else
        {
            // Encountered invalid identifier token in unit data.
            valid_unit_data = false;
        }
    }

    if(valid_unit_data)
    {
        *breed = temp_breed;
        breed->name = StringFromToken(name_token, memory::permanent_arena_id);
        // CopyString()
        // CopyString(breed->name, name_token.start, m::Min(sizeof(breed->name), name_token.length+1));

        return true;
    }
    else
    {
        buffer->p = initial;
        size_t number_of_bytes_to_print = m::Min(BufferBytesRemaining(*buffer), size_t(32));
        Log("Encountered invalid unit data in buffer at address: %p (\"%.*s\")",
            buffer->p, number_of_bytes_to_print, buffer->p);

        return false;
    }
}

bool
LoadBreedFile(const char *filename, Table<Breed> *breed_table, Table<Ability> ability_table)
{
    if(!filename or !breed_table) return false;

    Buffer file;
    bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
    if(!load_success) return false;

    size_t unit_count_loaded = 0;

    while(BufferBytesRemaining(file) > 0)
    {
        bool found_unit = SeekNextLineThatBeginsWith(&file, "unit");
        if(!found_unit) break;

        Breed temp_breed = {};
        if(ParseNextAsBreedData(&file, &temp_breed, ability_table))
        {
            auto breed_id = CreateEntry(breed_table);
            Breed *breed = GetBreedFromId(breed_id);
            if(breed == nullptr) break;

            *breed = temp_breed;
            breed->init = true;
            ++unit_count_loaded;
        }
        else
        {
            ++file.p;
            continue;
        }
    }

    if(c::verbose_success_logging)
    {
        Log("Loaded %zu unit breeds from file: %s", unit_count_loaded, filename);
    }
    FreeBuffer(&file);
    return true;
}

// On success, returns pointer to newly created unit, which is placed in g::unit_table.
// Returns nullptr if unit creation fails for any reason.
Id<Unit>
CreateUnit(Id<Breed> breed_id, Team team, PoolId<Arena> arena_id)
{
    Breed *breed = GetBreedFromId(breed_id);
    if(!ValidBreed(breed)) return c::null_unit_id;

    auto unit_id = CreateEntry(&g::unit_table);
    Unit *unit = GetUnitFromId(unit_id);
    if(!unit) return c::null_unit_id;

    unit->name = CopyString(breed->name, arena_id);
    unit->team = team;
    unit->max_traits = breed->max_traits;
    unit->cur_traits = breed->max_traits;
    for(int i=0; i<c::moveset_max_size; i++)
    {
        // @note: do we need to actually check that this ability is valid?
        //        i.e., can a unit have a reference to an invalid ability? Maybe.
        // Ability *ability = GetAbilityFromId(breed->ability_ids[i]);
        // if(ability == nullptr) continue;

        unit->ability_ids[i] = breed->ability_ids[i];
    }
    unit->cur_action_points = 0;
    unit->max_action_points = 1;

    unit->intent.target_set = CreateArrayFromArena<UnitId>(8, arena_id);

    unit->init = true;
    return unit_id;
}

Id<Unit>
CreateUnitByName(String name, Team team, PoolId<Arena> arena_id)
{
    Id breed_id = GetIndexFromName(g::breed_table, name);
    return CreateUnit(breed_id, team, arena_id);
}

Id<Unit>
CreateUnitByName(char *name, Team team, PoolId<Arena> arena_id)
{
    String name_string = StringFromCString(name);
    return CreateUnitByName(name_string, team, arena_id);
}

// bool
// _CheckValidEffectTarget(Unit *caster, Unit *target, Effect *effect)
// {
//  if(!ValidUnit(caster) or !ValidUnit(target)) return false;

//  TargetClass tc = effect->target_class;
//  if(tc == TargetClass::self)
//  {
//      return(caster == target);
//  }
//  else if(tc == TargetClass::single_ally)
//  {
//      return(caster->team == target->team);
//  }
//  else if(tc == TargetClass::single_ally_not_self)
//  {
//      return(caster->team == target->team and caster != target);
//  }
//  else if(tc == TargetClass::all_allies)
//  {
//      return(caster->team == target->team);
//  }
//  else if(tc == TargetClass::all_allies_not_self)
//  {
//      return(caster->team == target->team and caster != target);
//  }
//  else if(tc == TargetClass::single_enemy)
//  {
//      return(caster->team != target->team);
//  }
//  else if(tc == TargetClass::all_enemies)
//  {
//      return(caster->team != target->team);
//  }
//  else if(tc == TargetClass::single_unit)
//  {
//      return true;
//  }
//  else if(tc == TargetClass::single_unit_not_self)
//  {
//      return(caster != target);
//  }
//  else if(tc == TargetClass::all_units)
//  {
//      return true;
//  }

//  Log("Invalid TargetClass encountered (%d)", int(tc));
//  return false;
// }

// void AddUnitToUnitSet(Id<Unit> unit_id, Array<UnitId> *unit_set)
// {
//  if(unit_set->count >= ArrayCount(unit_set->ids)) return; // Set is already full.

//  bool unit_already_in_set = false;
//  for(auto unit_in_set_id : *unit_set)
//  {
//      if(unit_id == unit_in_set_id) unit_already_in_set = true;
//  }

//  if(!unit_already_in_set)
//  {
//      unit_set->ids[unit_set->size++] = unit_id;
//  }
// }

// UnitSet
// CombineUnitSets(const UnitSet *a, const UnitSet *b)
// {
//  UnitSet combined = {};
//  for(int i=0; i<a->size; i++)
//  {
//      AddUnitToUnitSet(a->ids[i], &combined);
//  }

//  for(int i=0; i<b->size; i++)
//  {
//      if(!UnitInUnitSet(b->ids[i], *a))
//      {
//          AddUnitToUnitSet(b->ids[i], &combined);
//      }
//  }

//  return combined;
// }

bool
CheckValidTarget(Id<Unit> caster_id, Id<Unit> target_id, TargetClass tc)
{
    Unit *caster = GetUnitFromId(caster_id);
    Unit *target = GetUnitFromId(target_id);
    if(!ValidUnit(caster) or !ValidUnit(target) or UnitIsDead(caster_id) or UnitIsDead(target_id)) return false;

    if(tc == TargetClass::self)
    {
        return(caster_id == target_id);
    }
    else if(tc == TargetClass::single_ally)
    {
        return(caster->team == target->team);
    }
    else if(tc == TargetClass::single_ally_not_self)
    {
        return(caster->team == target->team and caster_id != target_id);
    }
    else if(tc == TargetClass::all_allies)
    {
        return(caster->team == target->team);
    }
    else if(tc == TargetClass::all_allies_not_self)
    {
        return(caster->team == target->team and caster_id != target_id);
    }
    else if(tc == TargetClass::single_enemy)
    {
        return(caster->team != target->team);
    }
    else if(tc == TargetClass::all_enemies)
    {
        return(caster->team != target->team);
    }
    else if(tc == TargetClass::single_unit)
    {
        return true;
    }
    else if(tc == TargetClass::single_unit_not_self)
    {
        return(caster_id != target_id);
    }
    else if(tc == TargetClass::all_units)
    {
        return true;
    }

    Log("Invalid TargetClass encountered (%d)", int(tc));
    return false;
}

void
GenerateInferredUnitSet(Id<Unit> caster_id,
                        Id<Unit> selected_target_id,
                        TargetClass tc,
                        Array<UnitId> all_targets,
                        Array<UnitId> *inferred_target_set)
{
    TIMED_BLOCK;

    Unit *caster = GetUnitFromId(caster_id);
    Unit *selected_target = GetUnitFromId(selected_target_id);
    if(!ValidUnit(caster) or !ValidUnit(selected_target)) return;

    // Return empty target set if the selected target is invalid or not a valid target for the given target_class
    if(!CheckValidTarget(caster_id, selected_target_id, tc)) return;

    ClearArray(inferred_target_set);

    if(tc == TargetClass::all_allies)
    {
        // All targets that are on the same team as the caster
        for(auto target_id : all_targets)
        {
            Unit *target = GetUnitFromId(target_id);
            if(!ValidUnit(target)) continue;

            if(target->team == caster->team and !UnitIsDead(target_id))
            {
                *inferred_target_set += target_id;
            }
        }
    }
    else if(tc == TargetClass::all_allies_not_self)
    {
        // All targets that are on the same team as the caster, excluding the caster.
        for(auto target_id : all_targets)
        {
            Unit *target = GetUnitFromId(target_id);
            if(!ValidUnit(target)) continue;

            if(target_id != caster_id and target->team == caster->team and !UnitIsDead(target_id))
            {
                *inferred_target_set += target_id;
            }
        }
    }
    else if(tc == TargetClass::single_ally_not_self)
    {
        // if(selected target is ally _AND_ selected target is not self)
        if(selected_target->team == caster->team and selected_target != caster and !UnitIsDead(selected_target_id))
        {
            *inferred_target_set += selected_target_id;
        }
    }
    else if(tc == TargetClass::single_unit_not_self)
    {
        // if(selected target is not self)
        if(selected_target != caster and !UnitIsDead(selected_target_id))
        {
            *inferred_target_set += selected_target_id;
        }
    }
    else if(tc == TargetClass::all_enemies)
    {
        // All targets that are not on the same team as the caster.
        for(auto target_id : all_targets)
        {
            Unit *target = GetUnitFromId(target_id);
            if(!ValidUnit(target)) continue;

            if(target->team != caster->team and !UnitIsDead(target_id))
            {
                *inferred_target_set += target_id;
            }
        }
    }
    else if(tc == TargetClass::all_units)
    {
        // All targets
        for(auto target_id : all_targets)
        {
            if(UnitIsDead(target_id)) continue;
            Unit *target = GetUnitFromId(target_id);
            if(!ValidUnit(target)) continue;

            *inferred_target_set += target_id;
        }
    }
    else
    {
        // Single-target abilities need no inference. The inferred target set is
        // always equal to the selected target if it's a valid target.
        // At the time of writing this comment, this includes:
        //      self, single_ally, single_enemy, single_unit

        Unit *selected_target = GetUnitFromId(selected_target_id);
        if(ValidUnit(selected_target) and !UnitIsDead(selected_target_id))
        {
            *inferred_target_set += selected_target_id;
        }
    }
}

bool
UnitInUnitSet(Id<Unit> unit_id, Array<UnitId> target_set, int *index)
{
    Unit *unit = GetUnitFromId(unit_id);
    if(!ValidUnit(unit)) return false;

    for(int i=0; i<target_set.count; i++)
    {
        if(unit_id == target_set[i])
        {
            if(index) *index = i;
            return true;
        }
    }

    if(index) *index = -1;
    return false;
}

// // @note: AOWTC this doesn't check that the unit_id is valid.
// //        I figured this wouldn't be a problem, since it's not
// //        necessarilly a bug for a unit id corresponding to an
// //        invalid unit to be inside a unit set -- it's up to the
// //        methods interpreting the unit set to check for that.
// void
// AddUnitIdToUnitSet(Id<Unit> unit_id, UnitSet *target_set)
// {
//  // Do nothing if target set is already at max size
//  if(target_set->size >= ArrayCount(target_set->ids)) return;

//  // Do nothing if [*unit] is already part of the set
//  if(UnitInUnitSet(unit_id, *target_set)) return;

//  // Add the unit to the set
//  target_set->ids[target_set->size++] = unit_id;
// }

Vec2f
DrawTraitBarWithPreview(Vec2f pos, int current, int max, int preview, Color color, float flash_timer)
{
    TIMED_BLOCK;

    const Rect bar_rect = {pos, c::trait_bar_size};

    // If the max value is 0, don't draw the trait bar.
    if(max <= 0)
    {
        //ErrorDrawText(pos, "max <= 0");
        return Vec2f{};
    }
    else if(current < 0)
    {
        ErrorDrawText(pos, "current < 0");
        return Vec2f{};
    }
    else if(preview < 0)
    {
        ErrorDrawText(pos, "preview < 0");
        return Vec2f{};
    }

    // Cases:
    //
    // NO CHANGE
    //      1) Trait is not being changed (preview == current)
    //          => Color bar proportionally to current/max
    //          => If there's overheal, draw it as "X", where X=current-max
    //
    // INCREASING
    //      2) normal range => normal range
    //          => Color bar proportionally to current/max
    //          => Flash N pips, where N=preview-current
    //      3) normal range => overheal
    //          => Color bar proportionally to current/max
    //          => Flash N pips, where N=max-current
    //          => Draw "0+X" as overheal, where X=preview-max
    //      4) overheal => overheal
    //          => Fill bar
    //          => Draw "X+Y" as overheal, where X=current_overheal, Y=preview-current
    //
    // DECREASING
    //      5) normal range => normal range
    //          => Color bar proportionally to preview/max
    //          => Flash N pips, where N=current-preview
    //      6) overheal => normal range
    //          => Color bar proportionally to preview/max
    //          => Flash N pips, where N=max-preview
    //          => Draw "X-Y", where X=current_overheal, Y=current_overheal
    //      7) overheal => overheal
    //          => Fill bar
    //          => Draw "X-Y" as overheal, where X=current_overheal, Y=current-preview

    int solid_pip_count = 0;
    int flash_pip_count = 0;
    int overheal_change_sign = 0; // <0 if negative, 0 if unchanged, >0 if positive
    if(preview == current)
    {
        // Case 1 (No change)
        solid_pip_count = m::Max(0, m::Min(current, max));
        flash_pip_count = 0;
        overheal_change_sign = 0;
    }
    else if(preview > current and current <= max and preview <= max)
    {
        // Case 2 (increase; normal => normal)
        solid_pip_count = current;
        flash_pip_count = preview-current;
        overheal_change_sign = 0;
    }
    else if(preview > current and current <= max and preview > max)
    {
        // Case 3 (increase; normal => overheal)
        solid_pip_count = current;
        flash_pip_count = max-current;
        overheal_change_sign = 1;
    }
    else if(preview > current and current > max and preview > max)
    {
        // Case 4 (increase; overheal => overheal)
        solid_pip_count = max;
        flash_pip_count = 0.f;
        overheal_change_sign = 1;
    }
    else if(preview < current and current <= max and preview <= max)
    {
        // Case 5 (decrease; normal => normal)
        solid_pip_count = preview;
        flash_pip_count = current-preview;
        overheal_change_sign = 0;
    }
    else if(preview < current and current > max and preview <= max)
    {
        // Case 6 (decrease; overheal => normal)
        solid_pip_count = preview;
        flash_pip_count = max-preview;
        overheal_change_sign = -1;
    }
    else if(preview < current and current > max and preview > max)
    {
        // Case 7 (decrease; overheal => overheal)
        solid_pip_count = max;
        flash_pip_count = 0;
        overheal_change_sign = -1;
    }
    else if(c::verbose_error_logging)
    {
        Log("Invalid case encountered in function " __FUNCTION__ " (current:%d, max:%d, preview:%d)",
            current, max, preview);

        return Vec2f{};
    }

    float pip_width = c::trait_bar_size.x / max;
    float solid_bar_width = solid_pip_count * pip_width;
    float flash_bar_width = flash_pip_count * pip_width;

    Rect solid_rect = {bar_rect.pos, {solid_bar_width, c::trait_bar_size.y}};
    Rect flash_rect = {RectTopRight(solid_rect), {flash_bar_width, c::trait_bar_size.y}};

    DrawFilledRect(solid_rect, color);
    if(preview < current)
    {
        // Flash red if trait is being decreased
        Color flashing_red = {1.f, 0.f, 0.f, flash_timer};
        DrawFilledRect(flash_rect, flashing_red);
    }
    if(preview > current)
    {
        // Flash green if trait is being increased
        Color flashing_green = {0.f, 1.f, 0.f, flash_timer};
        DrawFilledRect(flash_rect, flashing_green);
    }

    DrawUnfilledRect(bar_rect, c::white);


    // for(int i=0; i<max; i++)
    // {
    //  // double-width line
    //  DrawLine(bar_rect.pos + Vec2f{i*pip_width, 0.f},
    //           bar_rect.pos + Vec2f{i*pip_width, bar_rect.size.y},
    //           c::white);
    //  DrawLine(bar_rect.pos + Vec2f{i*pip_width+1, 0.f},
    //           bar_rect.pos + Vec2f{i*pip_width+1, bar_rect.size.y},
    //           c::white);
    // }

    //DrawText(c::trait_bar_value_text_layout, RectCenter(bar_rect), "%d/%d", m::Min(preview, max), max);
    DrawText(c::trait_bar_value_text_layout, RectCenter(bar_rect), "%d/%d", preview, max);

    // Draw overheal box and number value if there is any overheal (previewed overheal > 0)
    // if(preview > max or current > max)
    // {
    //  TextLayout overheal_layout = c::def_text_layout;
    //  overheal_layout.font_size = 16;
    //  overheal_layout.align = c::align_leftcenter;
    //  Vec2f overheal_text_pos = RectTopRight(bar_rect) + Vec2f{c::overheal_text_h_offset, 0.5f*c::trait_bar_size.y};

    //  if(overheal_change_sign == 0)
    //  {
    //      DrawText(overheal_layout, overheal_text_pos, "%d", current-max);
    //  }
    //  else if(overheal_change_sign > 0)
    //  {
    //      overheal_text_pos.x += DrawText(overheal_layout, overheal_text_pos, "%d", m::Max(0, current-max)).x;

    //      Color flashing_color = {0.f, 1.f, 0.f, flash_timer};
    //      overheal_layout.color = flashing_color;
    //      DrawText(overheal_layout, overheal_text_pos, " +%d", preview-max);
    //  }
    //  else if(overheal_change_sign < 0)
    //  {
    //      overheal_text_pos.x += DrawText(overheal_layout, overheal_text_pos, "%d", current-max).x;

    //      Color flashing_color = {1.f, 0.f, 0.f, flash_timer};
    //      overheal_layout.color = flashing_color;
    //      DrawText(overheal_layout, overheal_text_pos, " -%d", current-max);
    //  }
    // }

     Vec2f trait_change_text_pos = RectTopRight(bar_rect) + Vec2f{c::trait_change_preview_h_offset, 0.5f*bar_rect.size.y};

    // Draw change to trait to the right of the trait bar
    if(preview == current)
    {
        // Draw nothing if no change.
    }
    else if(preview > current)
    {
        // Draw green "+X" if trait is increasing

        TextLayout layout = c::trait_change_preview_text_layout;
        layout.color = {0.f, 1.f, 0.f, flash_timer};
        DrawText(layout, trait_change_text_pos, "+%d", preview-current);
    }
    else if(preview < current)
    {
        // Draw red "-X" if trait is decreasing

        TextLayout layout = c::trait_change_preview_text_layout;
        layout.color = {1.f, 0.f, 0.f, flash_timer};
        DrawText(layout, trait_change_text_pos, "-%d", current-preview);
    }

    return(Vec2f{0.f, c::trait_bar_size.y});
}

// void
// DrawTraitSet(Vec2f pos, TraitSet cur_traits, TraitSet max_traits)
// {
//  pos += DrawTraitBarWithPreview(pos, cur_traits.vigor, max_traits.vigor,
//                                 cur_traits.vigor, c::red); // Vigor
//  pos += DrawTraitBarWithPreview(pos, cur_traits.focus, max_traits.focus,
//                                 cur_traits.focus, c::lt_blue); // Focus
//  pos += DrawTraitBarWithPreview(pos, cur_traits.armor, max_traits.armor,
//                                 cur_traits.armor, c::gold); // Armor
// }

void
DrawTraitSetWithPreview(Vec2f pos, TraitSet cur_traits, TraitSet max_traits, TraitSet preview_traits, float flash_timer)
{
    pos += DrawTraitBarWithPreview(pos, cur_traits.vigor, max_traits.vigor,
                                   preview_traits.vigor, c::red, flash_timer); // Vigor
    pos += DrawTraitBarWithPreview(pos, cur_traits.focus, max_traits.focus,
                                   preview_traits.focus, c::lt_blue, flash_timer); // Focus
    pos += DrawTraitBarWithPreview(pos, cur_traits.armor, max_traits.armor,
                                   preview_traits.armor, c::gold, flash_timer); // Armor
}

char *
TraitSetString(TraitSet traits)
{
    char *traitset_string = ScratchString(c::max_traitset_string_size);

    bool prepend_space = false; // Track whether a trait has been written yet, so we know to prepend a space.
    char *p = traitset_string;

    const int trait_count = 3;
    char trait_chars[trait_count+1] = "VFA";
    for(int i=0; i<trait_count; i++)
    {
        if(traits[i] == 0) continue;

        char format_string[5];
        char *f = format_string;
        if(prepend_space) *f++ = ' ';
        *f++ = '%';
        *f++ = 'd';
        *f++ = trait_chars[i];
        *f = '\0';

        int substring_length = snprintf(p, c::max_traitset_string_size, format_string, traits[i]);
        if(substring_length < 0 or substring_length > c::max_traitset_string_size)
        {
            // snprintf had an encoding error or traitset_string wasn't large enough.
            traitset_string[0] = '\0';
            return traitset_string;
        }
        p += substring_length;

        prepend_space = true;
    }

    *p = '\0';
    return traitset_string;
}

int
DetermineAbilityTier(Id<Unit> caster_id, Id<Ability> ability_id)
{
    TIMED_BLOCK;

    Unit *caster = GetUnitFromId(caster_id);
    Ability *ability = GetAbilityFromId(ability_id);
    if(!ValidUnit(caster) or !ValidAbility(ability)) return -1;

    for(int i=ability->tiers.count-1; i>=0; --i)
    {
        if(caster->cur_traits >= ability->tiers[i].required_traits) return i;
    }

    // The caster doesn't have the required traits to use the ability at all.
    // We return a negative value to indicate this case.
    return -1;
}

// Assumes outer context has already checked that *caster and *ability are valid.
int
DetermineAbilityTier(Unit *caster, Ability *ability)
{
    for(int i=ability->tiers.count-1; i>=0; --i)
    {
        if(caster->cur_traits >= ability->tiers[i].required_traits) return i;
    }

    // The caster doesn't have the required traits to use the ability at all.
    // We return a negative value to indicate this case.
    return -1;
}

Breed *
GetBreedFromId(Id<Breed> id)
{
    return GetEntryFromId(g::breed_table, id);
}

Unit *
GetUnitFromId(Id<Unit> id, Table<Unit> unit_table)
{
    TIMED_BLOCK;
    return GetEntryFromId(unit_table, id);
}

void
FullHealUnit(Id<Unit> id)
{
    Unit *unit = GetUnitFromId(id);
    if(!unit) return;

    unit->cur_traits = unit->max_traits;
}

Id<Breed>
RandomBreedIdByTier(int tier)
{
    Array<Id<Breed>> valid_ids = CreateTempArray<Id<Breed>>(g::breed_table.entry_count);
    for(int i=0; i<g::breed_table.entry_count; ++i)
    {
        auto &entry = g::breed_table.entries[i]; // alias
        Breed &breed = entry.data; // alias
        if(!breed.init) continue;

        if(breed.tier == tier) valid_ids += entry.id;
    }

    int chosen_index = RandomU32(0, valid_ids.count-1);
    return valid_ids[chosen_index];
}