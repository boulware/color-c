#include "ability.h"

void StartEditor(Editor *editor)
{
	editor->arena = AllocateArena();

	editor->left_panel_layout = {
		.rect = {
			.pos = {0.f,100.f},
			.size = {300.f,game->window_size.y-100}
		}
	};

	// editor->field_container = {};
	// editor->field_container.pos = {350.0f, 0.0f};
	// editor->field_container.button_layout = c::def_button_layout;
	// editor->field_container.text_entry_layout = c::def_text_entry_layout;

	// Fetch ability names from ability data table

	// Allocate field_labels and fill them with field names
	const char *field_names[] = {"Search", "Name"};
	editor->field_positions[0] = {10.f,10.f};
	editor->field_positions[1] = {350.0f, 10.f};
	int field_label_length = c::max_field_label_length;
	for(int i=0; i<ArrayCount(editor->field_labels); i++)
	{
		// @note: this +1 is for an implicit null terminator (not tracked by the string) just
		// to be safe in case this string's data pointer ever gets passed to a function that'a assuming
		// null-terminated c-strings. It shouldn't be necessary, but a lot of my codebase still uses
		// c-strings, so it's probably better just to be a bit safe here and allocate an extra value.
		StringFromCString(field_names[i], &editor->arena);
		// char *data = (char*)AllocFromArena(&editor->arena, sizeof(char)*(field_label_length+1));
		// editor->field_labels[i] = {0, field_label_length, data};
		// CopyFromCString(&editor->field_labels[i], field_names[i]);
	}

	// Search box strings
	//editor->search_label = Utf32StringFromCString(&editor->arena, "Search");

	// Allocate field_strings
	int field_string_length = c::max_field_text_length;
	for(int i=0; i<ArrayCount(editor->field_strings); i++)
	{
		char *data = (char*)AllocFromArena(&editor->arena, sizeof(char)*(field_string_length+1));
		editor->field_strings[i] = {0, field_string_length, data};
	}

	editor->active_index = -1;
	editor->init = true;
}

void
UpdateAndDrawEditor(Editor *editor)
{
	if(!editor->init)
	{
		log("Editor was not initialized. Skipping " __FUNCTION__);
		return;
	}

	editor->left_panel_layout.rect.pos.y += 40*MouseScroll();

	// Generate list panel entry strings
	size_t ability_count = g::ability_table.entry_count;
	size_t unit_schematic_count = g::unit_schematic_table.entry_count;
	size_t filtered_entry_count = 0;
	String *panel_entries = (String*)AllocTemp(sizeof(String) * (ability_count + unit_schematic_count));

	// Add abilities to panel
	for(int i=0; i<ability_count; i++)
	{
		Ability *ability = (Ability*)g::ability_table[i];
		String ability_name = StringFromCString(ability->name);

		if(SubstringInString(LowerCase(editor->field_strings[0]), LowerCase(ability_name)))
		{
			panel_entries[filtered_entry_count++] = ability_name;
		}
	}

	// Add unit schematics to panel
	for(int i=0; i<unit_schematic_count; i++)
	{
		UnitSchematic *unit_schematic = (UnitSchematic*)g::unit_schematic_table[i];
		String unit_name = StringFromCString(unit_schematic->name);

		if(SubstringInString(LowerCase(editor->field_strings[0]), LowerCase(unit_name)))
		{
			panel_entries[filtered_entry_count++] = unit_name;
		}
	}

	//sTextEntry(c::def_text_entry_layout, {50.f,50.f}, editor->search_label, {}, false);

	// Draw list panel
	auto panel_response = ListPanel(editor->left_panel_layout, panel_entries, filtered_entry_count);
	if(panel_response.pressed_index >= 0)
	{ // A panel entry was clicked
		Ability *ability = (Ability*)g::ability_table[panel_response.pressed_index];
		if(ValidAbility(ability))
		{
			// Clear field string and write ability name to it.
			editor->field_strings[1].length = 0;
			CopyFromCString(&editor->field_strings[1], ability->name);
			editor->active_index = -1;
		}
	}

	// Draw text entry
	ResetImguiContainer(&editor->field_container);
	SetActiveContainer(&editor->field_container);

	for(int i=0; i<g::ability_table.entry_count; i++)
	{
		Ability *ability = (Ability*)g::ability_table[i];
		//ButtonResponse response = Button(ability->name);
	}


	// Get translated input stream from global input
	String *active_string = nullptr;
	if(editor->active_index >= 0)
	{
		active_string = &editor->field_strings[editor->active_index];
	}
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
						editor->active_index = (editor->active_index+1) % ArrayCount(editor->field_strings);
						editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
					}
					else
					{ // No active text entry, so set the 0th text entry as the active one.
						editor->active_index = 0;
						editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
					}
				}
				else
				{ // Reverse tab direction
					if(editor->active_index == 0)
					{
						// Special case for shift+tabbing when the 0th element is selected,
						// because the modulo a%b doesn't work the same way when a is negative.
						editor->active_index = ArrayCount(editor->field_strings)-1;
						editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
					}
					else if(editor->active_index > 0)
					{
						editor->active_index = (editor->active_index-1) % ArrayCount(editor->field_strings);
						editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
					}
					else
					{ // No active text entry, so set the 0th text entry as the active one.
						editor->active_index = 0;
						editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
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
			}
		}

		++p;
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
		editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
	}

	for(int i=0; i<ArrayCount(editor->field_strings); i++)
	{
		bool is_active = false;
		if(i == editor->active_index) is_active = true;

		// TextEntryLayout fields_text_entry_layout = c::def_text_entry_layout;
		// fields_text_entry_layout.rect.pos = {350.f, 0.f};

		auto response = TextEntry(c::def_text_entry_layout,
								  editor->field_positions[i],
								  editor->field_labels[i], editor->field_strings[i],
								  is_active, editor->text_cursor_pos);

		if(response.pressed)
		{
			editor->active_index = i;
			editor->text_cursor_pos = editor->field_strings[editor->active_index].length;
		}
	}
}