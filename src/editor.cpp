#include "ability.h"

void StartEditor(Editor *editor)
{
	editor->arena = AllocateArena();
	editor->mode = EditorMode::None;

	const char *field_names[] = {"Search", "Name", "Vigor", "Focus", "Armor", "Vigor", "Focus", "Armor", "Vigor", "Focus", "Armor"};
	for(int i=0; i<ArrayCount(field_names); i++)
	{
		editor->input_elements[i] = {
			.type = InputElementType::None,
			.label = StringFromCString(field_names[i], &editor->arena),
			.pos = ability_field_positions[i]
		};
	}

	// Allocate field_labels and fill them with field names

	// editor->input_elements[0].pos = {10.0f, 10.0f,};
	// editor->input_elements[1].pos = {450.0f, 10.0f};
	// int field_label_length = c::max_field_label_length;
	// for(int i=0; i<ArrayCount(editor->input_elements); i++)
	// {
	// 	editor->input_elements[i].label = StringFromCString(field_names[i], &editor->arena);
	// }

	//editor->search_label = StringFromCString("Search");
	//editor->search_string = AllocStringDataFromArena(c::max_field_label_length, &editor->arena);
	//editor->search_pos = {10.f,10.f};

	editor->active_index = -1;

	editor->panel_scroll_pos = 0.f;
	editor->panel_scroll_vel = 0.f;
	editor->panel_scroll_acc = 10.f;
	editor->panel_scroll_friction = -0.2f;
	editor->panel_scroll_velocity_minimum = 2.f;

	editor->init = true;
}

// String
// InputElementTextAsString(InputElement element)
// {
// 	if(element.type == InputElementType::String)
// 	{
// 		return *((String*)element.text);
// 	}
// 	else if(element.type == InputElementType::Integer)
// 	{
// 		return AsString((int*)element.value_address);
// 	}
// 	else
// 	{
// 		return String{};
// 	}
// }

void
GenerateInputElementString(InputElement *input_element, void *value_ptr)
{
	if(input_element->type == InputElementType::String)
	{
		input_element->text = AsString((String*)value_ptr);
		input_element->value_ptr = value_ptr;
	}
	else if(input_element->type == InputElementType::Integer)
	{
		input_element->text = AsString((int*)value_ptr);
		input_element->value_ptr = value_ptr;
	}
}

void
UpdateAndDrawEditor(Editor *editor)
{
	if(!editor->init)
	{
		log("Editor was not initialized. Skipping " __FUNCTION__);
		return;
	}

	// Generate and display list panel entries, by pulling data from global data tables
	// of editable objects (abilities, unit schematics, etc.)
	{

		size_t ability_count = g::ability_table.entry_count;
		size_t unit_schematic_count = g::unit_schematic_table.entry_count;
		size_t filtered_entry_count = 0;

		// @BUG: I'm currently allocating this panel_entries thing for temp strings, but I'm pointing
		// to the data member of the original string when I "copy" them. We probably don't even need to make temp
		// strings here, but this is something I need to be aware of. It's bug prone.
		String *panel_entries = (String*)AllocTemp(sizeof(String) * (ability_count + unit_schematic_count));
		// IDs into the relevant data table that correspond with the string panel entry strings.
		int *panel_entry_ids = (int*)AllocTemp(sizeof(int) * (ability_count + unit_schematic_count));

		// Add abilities to panel
		for(int i=0; i<ability_count; i++)
		{
			Ability *ability = (Ability*)g::ability_table[i];

			if(SubstringInString(LowerCase(editor->input_elements[(int)AbilityPropertyIndex::search].text), LowerCase(ability->name)))
			{
				panel_entries[filtered_entry_count] = ability->name;
				panel_entry_ids[filtered_entry_count] = i;
				++filtered_entry_count;
			}
		}

		// Add unit schematics to panel
		for(int i=0; i<unit_schematic_count; i++)
		{
			UnitSchematic *unit_schematic = (UnitSchematic*)g::unit_schematic_table[i];

			if(SubstringInString(LowerCase(editor->input_elements[(int)AbilityPropertyIndex::search].text), LowerCase(unit_schematic->name)))
			{
				panel_entries[filtered_entry_count] = unit_schematic->name;
				panel_entry_ids[filtered_entry_count] = i;
				++filtered_entry_count;
			}
		}

		// List panel scroll smoothing.
		{
			if(PointInRect(editor->search_panel_layout.rect, MousePos()))
			{
				editor->panel_scroll_vel -= editor->panel_scroll_acc*MouseScroll();
			}

			float frictional_force = editor->panel_scroll_vel*editor->panel_scroll_friction;
			if(m::Abs(frictional_force) <= m::Abs(editor->panel_scroll_vel))
			{
				editor->panel_scroll_vel += frictional_force;
			}
			else
			{
				// Frictional force is greater than the velocity, so applying it would
				// cause the velocity to flip sign. Instead, we'll just set the velocity to 0

				editor->panel_scroll_vel = 0.f;
			}
			editor->panel_scroll_pos = m::Max(0.0f, editor->panel_scroll_pos+editor->panel_scroll_vel);
			if(m::Abs(editor->panel_scroll_vel) < editor->panel_scroll_velocity_minimum) editor->panel_scroll_vel = 0.f;
			if(editor->panel_scroll_pos <= 0.f) editor->panel_scroll_vel = 0.f;
		}

		// Draw list panel
		auto panel_response = ListPanel(editor->search_panel_layout, panel_entries, filtered_entry_count, editor->panel_scroll_pos);
		if(panel_response.pressed_index >= 0)
		{ // A panel entry was clicked
			int first_ability_index = 0;
			int last_ability_index = ability_count - 1;
			int first_unit_index = ability_count;
			int last_unit_index = ability_count + unit_schematic_count - 1;

			int index_into_datatable = panel_entry_ids[panel_response.pressed_index];

			if(InRange(panel_response.pressed_index, first_ability_index, last_ability_index))
			{ // An ability button was clicked in the search panel
				// Load ability from data table.
				Ability *ability = (Ability*)g::ability_table[index_into_datatable];
				if(ValidAbility(ability))
				{
					// Copy the ability into a temp ability for editing.
					editor->temp_ability = *ability;
					editor->mode = EditorMode::Ability;
					for(int i=0; i<ArrayCount(ability_property_types); i++)
					{
						editor->input_elements[i].type = ability_property_types[i];
					}
				}
			}
			else if(InRange(panel_response.pressed_index, first_unit_index, last_unit_index))
			{
				UnitSchematic *unit_schematic = (UnitSchematic*)g::unit_schematic_table[index_into_datatable];
				if(ValidUnitSchematic(unit_schematic))
				{
					// Clear field string and write unit schematic name to it.
					editor->temp_unit_schematic = *unit_schematic;
					editor->mode = EditorMode::UnitSchematic;
				}
			}
		}
	}

	// Using our editor mode and temp object, generate temp strings for our text entry UI.
	{

		if(editor->mode == EditorMode::Ability)
		{
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::name], &editor->temp_ability.name);

			// Tier 0 required
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier0required_vigor], &editor->temp_ability.tiers[0].required_traits.vigor);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier0required_focus], &editor->temp_ability.tiers[0].required_traits.focus);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier0required_armor], &editor->temp_ability.tiers[0].required_traits.armor);

			// Tier 1 required
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier1required_vigor], &editor->temp_ability.tiers[1].required_traits.vigor);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier1required_focus], &editor->temp_ability.tiers[1].required_traits.focus);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier1required_armor], &editor->temp_ability.tiers[1].required_traits.armor);

			// Tier 2 required
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier2required_vigor], &editor->temp_ability.tiers[2].required_traits.vigor);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier2required_focus], &editor->temp_ability.tiers[2].required_traits.focus);
			GenerateInputElementString(&editor->input_elements[(int)AbilityPropertyIndex::tier2required_armor], &editor->temp_ability.tiers[2].required_traits.armor);

			DrawTextMultiline(c::def_text_layout, MousePos(), MetaString(&editor->temp_ability.tiers[0]));
		}
	}


	// // Prepare temporary strings to the global translated input stream to.
	// String editor->input_elements[ArrayCount(editor->input_elements)];
	// for(int i=0; i<ArrayCount(editor->input_elements); i++)
	// {
	// 	editor->input_elements[i] = InputElementTextAsString(editor->input_elements[editor->active_index]);
	// }

	String *active_string = nullptr;
	if(editor->active_index >= 0)
	{
		active_string = &editor->input_elements[editor->active_index].text;
	}

	// Get translated input stream from global input
	u32 *p = input::global_input->utf32_translated_stream;

	// Iterate through translated stream and interpret.
	while(*p != 0)
	{
		if(*p == 0)
		{ // Break on reaching null terminator in stream.
			break;
		}
		else if(IsUtf32ControlChar(*p))
		{ // Handle control codes
			if(*p == c::control_code_backspace and editor->text_cursor_pos > 0 and active_string)
			{ // Backspace removes char to the left of text cursor
				if(DeleteChar(active_string, editor->text_cursor_pos-1))
				{
					--editor->text_cursor_pos;
				}
			}
			else if(*p == c::control_code_tab)
			{ // Tab changes the active text entry box.
				if(!Down(vk::shift))
				{ // Forward tab direction
					if(editor->active_index >= 0)
					{ // There is an active text entry, so set the next text entry as the active one.
						editor->active_index = (editor->active_index+1) % ArrayCount(editor->input_elements);
						editor->text_cursor_pos = editor->input_elements[editor->active_index].text.length;
					}
					else
					{ // No active text entry, so set the 0th text entry as the active one.
						editor->active_index = 0;
						editor->text_cursor_pos = editor->input_elements[editor->active_index].text.length;
					}
				}
				else
				{ // Reverse tab direction
					if(editor->active_index == 0)
					{
						// Special case for shift+tabbing when the 0th element is selected,
						// because the modulo a%b trick doesn't work the same way when a is negative.
						editor->active_index = ArrayCount(editor->input_elements)-1;
						editor->text_cursor_pos = editor->input_elements[editor->active_index].text.length;
					}
					else if(editor->active_index > 0)
					{
						editor->active_index = (editor->active_index-1) % ArrayCount(editor->input_elements);
						editor->text_cursor_pos = editor->input_elements[editor->active_index].text.length;
					}
					else
					{ // No active text entry, so set the 0th text entry as the active one.
						editor->active_index = 0;
						editor->text_cursor_pos = editor->input_elements[editor->active_index].text.length;
					}
				}
			}
			else if(*p == c::control_code_return)
			{ // Currently no functionality for pressing enter.
			}
			else if(*p == c::control_code_DEL and active_string)
			{ // Spawned by ctrl+backspace in windows. Deletes all chars to the left of the text cursor.
				DeleteRange(active_string, 0, editor->text_cursor_pos);
				editor->text_cursor_pos = 0;
			}
		}
		else if(active_string)
		{
			if(InsertChar(active_string, *p, editor->text_cursor_pos))
			{
				++editor->text_cursor_pos;
				if(editor->active_index == 0)
				{
					editor->panel_scroll_pos = 0.f;
				}
			}
		}

		if(editor->active_index >= 0)
		{
			active_string = &editor->input_elements[editor->active_index].text;
		}
		++p;
	}

	// Fully sanitize temp strings (?)
	{

	}

	//DrawText(c::def_text_layout, {500.f,500.f}, "%d", editor->active_index);

	// Delete removes char to the right of text cursor
	if((Pressed(vk::del) or Repeated(vk::del)) and editor->text_cursor_pos < active_string->length)
	{
		if(DeleteChar(active_string, editor->text_cursor_pos))
		{
			//editor->text_cursor_pos;
		}

	}

	// Arrow keys move the text cursor
	if(Pressed(vk::left) or Repeated(vk::left))
	{
		editor->text_cursor_pos = m::Max(0, editor->text_cursor_pos-1);
	}

	if(Pressed(vk::right) or Repeated(vk::right))
	{
		editor->text_cursor_pos = m::Min((int)active_string->length, editor->text_cursor_pos+1);
	}

	// ctrl+F activates search textbox
	if(Down(vk::ctrl) and Pressed(vk::f))
	{
		editor->active_index = 0;
		editor->text_cursor_pos = editor->input_elements[0].text.length;
	}

	for(int i=0; i<ArrayCount(editor->input_elements); i++)
	{
		switch(editor->input_elements[i].type)
		{
			case InputElementType::String: {
				auto response = TextEntry(c::def_text_entry_layout,
										  editor->input_elements[i].pos,
										  editor->input_elements[i].label,
										  editor->input_elements[i].text,
										  (i == editor->active_index) ? true : false,
										  editor->text_cursor_pos);
			} break;
			case InputElementType::Integer: {
				auto response = IntegerBox(c::editor_vigor_integer_box_layout,
										   editor->input_elements[i].pos,
										   editor->input_elements[i].label,
										   editor->input_elements[i].text);

				if(response.value_change)
				{
					*(int*)editor->input_elements[i].value_ptr += response.value_change;
				}
			} break;
		}
	}


	// for(int i=0; i<ArrayCount(editor->field_strings); i++)
	// {
	// 	bool is_active = false;
	// 	if(i == editor->active_index) is_active = true;

	// 	//editor->temp_ability.name

	// 	auto response = TextEntry(c::def_text_entry_layout,
	// 							  editor->field_positions[i],
	// 							  editor->field_labels[i], editor->field_strings[i],
	// 							  is_active, editor->text_cursor_pos);

	// 	if(response.pressed)
	// 	{
	// 		editor->active_index = i;
	// 		editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
	// 	}
	// }

	// Number boxes
	// for(int i=0; i<1; i++)
	// {
	// 	IntegerBoxResponse response;

	// 	response = IntegerBox(c::editor_vigor_integer_box_layout, {500.f,500.f}, StringFromCString("Vigor"), editor->test_int);
	// 	editor->test_int = m::Max(0, editor->test_int+response.value_change);
	// 	response = IntegerBox(c::editor_armor_integer_box_layout, {555.f,500.f}, StringFromCString("Focus"), editor->test_int);
	// 	editor->test_int = m::Max(0, editor->test_int+response.value_change);
	// 	response = IntegerBox(c::editor_focus_integer_box_layout, {610.f,500.f}, StringFromCString("Armor"), editor->test_int);
	// 	editor->test_int = m::Max(0, editor->test_int+response.value_change);
	// }
}