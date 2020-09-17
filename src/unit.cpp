#include "unit.h"

#include "util.h"
#include "text_parsing.h"

bool
ParseNextAsTraitSet(Buffer *buffer, TraitSet *trait_set)
{
	char *initial = buffer->p;
	TraitSet temp;

	// true && true = true
	// true && false = false

	bool tokens_valid = true;
	tokens_valid = tokens_valid && ParseNextAsS32(buffer, &temp.vigor);
	tokens_valid = tokens_valid && ParseNextAsS32(buffer, &temp.focus);
	tokens_valid = tokens_valid && ParseNextAsS32(buffer, &temp.armor);

	if(tokens_valid)
	{
		*trait_set = temp;
		return true;
	}
	else
	{
		buffer->p = initial;
		return false;
	}
}

bool
ParseNextAsAbilityData(Buffer *buffer, Ability *ability)
{
	bool valid_ability_data = true;
	char *initial = buffer->p;
	Ability temp_ability = {};

	bool header_valid = ConfirmNextToken(buffer, "ability");
	if(!header_valid) valid_ability_data = false;

	Token name_token;
	bool is_valid_name = NextTokenAsDoubleQuotedString(buffer, &name_token);
	if(!is_valid_name)
	{
		valid_ability_data = false;
	}

	bool end_of_ability_data = false;
	while(valid_ability_data and !end_of_ability_data)
	{
		char *before_token = buffer->p;
		Token token = NextToken(buffer);

		if(BufferBytesRemaining(*buffer) == 0 or CompareStrings(token.start, "ability"))
		{
			// reset buffer to before the previous token was fetched, since it's not part of the current
			// ability data
			buffer->p = before_token;
			end_of_ability_data = true;
		}
		else if(CompareStrings(token.start, "self_required"))
		{
			valid_ability_data = ParseNextAsTraitSet(buffer, &temp_ability.self_required);
		}
		else if(CompareStrings(token.start, "target_required"))
		{
			valid_ability_data = ParseNextAsTraitSet(buffer, &temp_ability.target_required);
		}
		else if(CompareStrings(token.start, "change_to_target"))
		{
			valid_ability_data = ParseNextAsTraitSet(buffer, &temp_ability.change_to_target);
		}
		else if(CompareStrings(token.start, "change_to_self"))
		{
			 valid_ability_data = ParseNextAsTraitSet(buffer, &temp_ability.change_to_self);
		}
		else if(CompareStrings(token.start, "target_class"))
		{
			// target_class string (e.g., "single_unit", "self", "all_allies", etc.)
			Token target_class_token;
			valid_ability_data = NextTokenAsDoubleQuotedString(buffer, &target_class_token);

			if(valid_ability_data)
			{
				for(int i=0; i<sizeof(TargetClass_strings)/sizeof(char*); i++)
				{
					if(CompareBytesN(target_class_token.start, TargetClass_strings[i], target_class_token.length))
					{
						temp_ability.target_class = (TargetClass)i;
						break;
					}
				}
			}
		}
		else
		{
			valid_ability_data = false;
		}
	}

	if(valid_ability_data)
	{
		*ability = temp_ability;
		ability->name = (char*)calloc(name_token.length+1, 1);
		CopyStringN_unsafe(ability->name, name_token.start, name_token.length);

		return true;
	}
	else
	{
		buffer->p = initial;
		size_t number_of_bytes_to_print = m::Min(BufferBytesRemaining(*buffer), size_t(32));
		log("Encountered invalid ability data in buffer at address: %p (\"%.*s\")",
			buffer->p, number_of_bytes_to_print, buffer->p);

		return false;
	}
}

bool
ParseNextAsUnitData(Buffer *buffer, UnitSchematic *unit_schematic, DataTable ability_table)
{
	bool valid_unit_data = true;
	char *initial = buffer->p;
	UnitSchematic temp_unit_schematic = {};
	for(int i=0; i<c::moveset_max_size; i++)
	{
		temp_unit_schematic.ability_indices[i] = -1; // -1 is considered an empty ability slot
	}

	bool header_valid = ConfirmNextToken(buffer, "unit");
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

		if(BufferBytesRemaining(*buffer) == 0 or CompareStrings(token.start, "unit"))
		{
			// reset buffer to before the previous token was fetched, since it's not part of the current
			// ability data
			buffer->p = before_token;
			end_of_unit_data = true;
		}
		else if(CompareStrings(token.start, "traits"))
		{
			valid_unit_data = ParseNextAsTraitSet(buffer, &temp_unit_schematic.max_traits);
		}
		else if(CompareStrings(token.start, "moveset"))
		{
			// Look for *up to* moveset_max_size strings of ability names
			for(int i=0; i<c::moveset_max_size; i++)
			{
				Token ability_name_token;
				if(NextTokenAsDoubleQuotedString(buffer, &ability_name_token))
				{
					if(!GetEntryIndexByStringMember(ability_table, MemberOffset(Ability, name),
												   ability_name_token.start, &temp_unit_schematic.ability_indices[i]))
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
		else
		{
			// Encountered invalid identifier token in unit data.
			valid_unit_data = false;
		}
	}

	if(valid_unit_data)
	{
		*unit_schematic = temp_unit_schematic;
		unit_schematic->name = (char*)calloc(name_token.length+1, 1);
		CopyStringN_unsafe(unit_schematic->name, name_token.start, name_token.length);

		return true;
	}
	else
	{
		buffer->p = initial;
		size_t number_of_bytes_to_print = m::Min(BufferBytesRemaining(*buffer), size_t(32));
		log("Encountered invalid unit data in buffer at address: %p (\"%.*s\")",
			buffer->p, number_of_bytes_to_print, buffer->p);

		return false;
	}
}

bool
LoadAbilityFile(const char *filename, DataTable *table)
{
	Buffer file;
	bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
	if(!load_success) return false;

	size_t ability_count_loaded = 0;

	while(BufferBytesRemaining(file) > 0)
	{
		bool found_ability = SeekNextLineThatBeginsWith(&file, "ability");
		if(!found_ability) break;

		if(DataTableEntriesRemaining(*table) >= 1)
		{
			Ability temp_ability;
			if(ParseNextAsAbilityData(&file, &temp_ability))
			{
				Ability *ability = (Ability*)CreateEntry(table);
				*ability = temp_ability;
				ability_count_loaded += 1;
			}
			else
			{
				break;
			}
		}
	}

	//log("Loaded %zu abilities from file: %s", ability_count_loaded, filename);
	FreeBuffer(&file);
	return true;
}

bool
LoadUnitFile(const char *filename, DataTable *table, DataTable ability_table)
{
	Buffer file;
	bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
	if(!load_success) return false;

	size_t unit_count_loaded = 0;

	while(BufferBytesRemaining(file) > 0)
	{
		bool found_unit = SeekNextLineThatBeginsWith(&file, "unit");
		if(!found_unit) break;

		if(DataTableEntriesRemaining(*table) >= 1)
		{
			UnitSchematic temp_unit_schematic;
			if(ParseNextAsUnitData(&file, &temp_unit_schematic, ability_table))
			{
				UnitSchematic *unit_schematic = (UnitSchematic*)CreateEntry(table);
				*unit_schematic = temp_unit_schematic;
				unit_count_loaded += 1;
			}
			else
			{
				break;
			}
		}
	}

	//log("Loaded %zu unit schematics from file: %s", unit_count_loaded, filename);
	FreeBuffer(&file);
	return true;
}

void
DrawUnitInfo(Unit unit, Vec2f pos)
{
	UnitSchematic *schematic = (UnitSchematic*)GetEntryByIndex(game->unit_table, unit.schematic_index);
	if(schematic == nullptr) return;

	pos.y += DrawText(c::medium_text, pos, schematic->name).y;
	pos.y += DrawText(c::medium_text, pos, "Vigor: %d/%d", unit.cur_traits.vigor, schematic->max_traits.vigor).y;
	pos.y += DrawText(c::medium_text, pos, "Focus: %d/%d", unit.cur_traits.focus, schematic->max_traits.focus).y;
	pos.y += DrawText(c::medium_text, pos, "Armor: %d/%d", unit.cur_traits.armor, schematic->max_traits.armor).y;

	pos.y += DrawText(c::medium_text, pos, "Abilities:").y;

	ImguiContainer layout = {};
	layout.pos = pos;
	layout.max_size = {0.f, 0.f};
	layout.font_size = 24;

	SetActiveContainer(&layout);
	for(int i=0; i<c::moveset_max_size; i++)
	{
		Ability *ability = (Ability*)GetEntryByIndex(game->ability_table, schematic->ability_indices[i]);
		if(ability == nullptr) break;

		Button(ability->name);
	}
}

// Return false if failed to find unit with given name
bool
CreateUnit(const char *name, Team team, Unit *unit)
{
	int index;
	bool found_entry = GetEntryIndexByStringMember(game->unit_table, MemberOffset(UnitSchematic, name), name, &index);
	if(!found_entry) return false;

	UnitSchematic *schematic = (UnitSchematic*)GetEntryByIndex(game->unit_table, index);

	*unit = {};
	unit->schematic_index = index;
	unit->cur_traits = schematic->max_traits;
	unit->team = team;
	unit->active = true;
	return true;
}

const char *
GetUnitName(Unit unit)
{
	UnitSchematic *schematic = (UnitSchematic*)GetEntryByIndex(game->unit_table, unit.schematic_index);
	if(schematic == nullptr) return nullptr;

	return schematic->name;
}

UnitSchematic *
GetSchematicFromUnit(Unit unit)
{
	return (UnitSchematic*)GetEntryByIndex(game->unit_table, unit.schematic_index);
}

const char *
AbilityNameFromIndex(int index)
{
	Ability *ability = (Ability*)GetEntryByIndex(game->ability_table, index);
	if(ability == nullptr) return nullptr;

	return ability->name;
}

void
DrawUnitHudData(Unit unit)
{
	UnitSchematic *schematic = GetSchematicFromUnit(unit);
	if(schematic == nullptr)
	{
		log("Tried to draw HUD data for unit with invalid schematic_index.");
		return;
	}

	float bottom_offset = 150.f;
	Vec2f pen = {0.f, (float)c::window_height-bottom_offset};
	DrawFilledRect({pen, {c::window_width,bottom_offset}}, {0.1f,0.1f,0.1f});
	DrawLine(pen, pen+Vec2f{c::window_width,0.f}, c::white);

	float left_padding = 20.f;
	float top_padding = 10.f;
	pen.x += left_padding;
	pen.y += top_padding;

	// Draw name and traits
	pen.y += DrawText(32, pen, schematic->name).y;

	float name_trait_padding = 20.f;
	pen.y += name_trait_padding;
	pen.y += DrawText(16, pen, "Vigor: %d/%d",
							unit.cur_traits.vigor, schematic->max_traits.vigor).y;
	pen.y += DrawText(16, pen, "Focus: %d/%d",
							unit.cur_traits.focus, schematic->max_traits.focus).y;
	pen.y += DrawText(16, pen, "Armor: %d/%d",
							unit.cur_traits.armor, schematic->max_traits.armor).y;

	// Ability buttons
	float ability_left_offset = 200.f;
	float ability_top_offset = 10.f;
	ImguiContainer layout = {};
	layout.pos = {ability_left_offset, (float)c::window_height-bottom_offset+ability_top_offset};
	layout.font_size = 16;
	SetActiveContainer(&layout);

	for(int i : schematic->ability_indices)
	{
		Ability *ability = (Ability*)GetEntryByIndex(game->ability_table, i);
		if(ability == nullptr) continue;

		const char *ability_name = ability->name;
		Vec2f button_start_pos = layout.pen;
		Color button_color = c::white;
		Color hover_color = c::yellow;

		if(game->_selected_ability == ability)
		{
			button_color = c::orange;
			hover_color = c::orange;
		}

		auto response = ButtonColor(button_color, hover_color, ability_name);
		if(response.pressed)
		{
			SetSelectedAbility(ability);
		}

		if(game->_selected_ability == ability)
		{
			float button_height = layout.pen.y - button_start_pos.y;
			float active_box_size = game->target_cursor.size.x;
			float offset = -0.5f*button_height;
			Vec2f pos = layout.pos + layout.pen + Vec2f{offset,offset};
			pos = {pos.x, pos.y};

			DrawSprite(game->target_cursor, Round(pos));
		}
	}
}

bool
CheckValidAbilityTarget(Unit *source, Unit *target, Ability *ability)
{
	TargetClass tc = ability->target_class;
	if(tc == TargetClass::self)
	{
		return(source == target);
	}
	else if(tc == TargetClass::single_ally)
	{
		return(source->team == target->team);
	}
	else if(tc == TargetClass::single_ally_not_self)
	{
		return(source->team == target->team and source != target);
	}
	else if(tc == TargetClass::all_allies)
	{
		return(source->team == target->team);
	}
	else if(tc == TargetClass::all_allies_not_self)
	{
		return(source->team == target->team and source != target);
	}
	else if(tc == TargetClass::single_enemy)
	{
		return(source->team != target->team);
	}
	else if(tc == TargetClass::all_enemies)
	{
		return(source->team != target->team);
	}
	else if(tc == TargetClass::single_unit)
	{
		return true;
	}
	else if(tc == TargetClass::single_unit_not_self)
	{
		return(source != target);
	}
	else if(tc == TargetClass::all_units)
	{
		return true;
	}

	log("Invalid TargetClass encountered (%d)", int(tc));
	return false;
}

TargetSet
GenerateValidTargetSet(Unit *source, Ability *ability, TargetSet all_targets)
{
	TargetSet valid_targets = {};
	if(source == nullptr or ability == nullptr) return valid_targets;

	for(Unit *target : all_targets.units)
	{
		if(CheckValidAbilityTarget(source, target, ability))
		{
			valid_targets.units[valid_targets.size++] = target;
		}
	}

	return valid_targets;
}

TargetSet
GenerateInferredTargetSet(Unit *source, Unit *selected_target, Ability *ability, TargetSet all_targets)
{
	TargetSet inferred_target_set = {};

	// Return empty set if the target is invalid.
	if(!CheckValidAbilityTarget(source, selected_target, ability))
	{
		return inferred_target_set;
	}

	TargetClass tc = ability->target_class;
	if(tc == TargetClass::all_allies)
	{
		// All targets that are on the same team as the source
		for(Unit *unit : all_targets.units)
		{
			if(unit->team == source->team)
			{
				AddUnitToTargetSet(unit, &inferred_target_set);
			}
		}
	}
	else if(tc == TargetClass::all_allies_not_self)
	{
		// All targets that are on the same team as the source, excluding the source.
		for(Unit *unit : all_targets.units)
		{
			if(unit != source and unit->team == source->team)
			{
				AddUnitToTargetSet(unit, &inferred_target_set);
			}
		}
	}
	else if(tc == TargetClass::all_enemies)
	{
		// All targets that are not on the same team as the source.
		for(Unit *unit : all_targets.units)
		{
			if(unit->team != source->team)
			{
				AddUnitToTargetSet(unit, &inferred_target_set);
			}
		}
	}
	else if(tc == TargetClass::all_units)
	{
		// All targets
		inferred_target_set = all_targets;
	}
	else
	{
		// Single-target abilities need no inference. The inferred target set is
		// always equal to the selected target.
		// At the time of writing this comment, this includes:
		// 		self, single_ally, single_ally_not_self,
		//		single_enemy, single_unit, single_unit_not_self

		AddUnitToTargetSet(selected_target, &inferred_target_set);
	}

	return inferred_target_set;
}

bool
AbilityIsSelected()
{
	if(game->_selected_ability == nullptr) return false;
	else return true;
}

bool
IsSelectedAbility(Ability *ability)
{
	return(ability == game->_selected_ability);
}

void
SetSelectedAbility(Ability *ability)
{
	game->_selected_ability = ability;
	game->_selected_ability_valid_targets = GenerateValidTargetSet(game->selected_unit, ability, game->all_targets);
}

bool
UnitInTargetSet(Unit *unit, TargetSet target_set)
{
	for(Unit *unit_in_set : target_set.units)
	{
		if(unit == unit_in_set) return true;
	}

	return false;
}

void
AddUnitToTargetSet(Unit *unit, TargetSet *target_set)
{
	// Do nothing if target set is already at max size
	if(target_set->size >= c::max_target_count) return;

	// Do nothing if [*unit] is already part of the set
	if(UnitInTargetSet(unit, *target_set)) return;

	// Add the unit to the set
	target_set->units[target_set->size++] = unit;
}

