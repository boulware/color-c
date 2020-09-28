#ifndef EDITOR_H
#define EDITOR_H

#include "memory.h"
#include "utf32string.h"

struct Editor
{
	bool init;
	Arena arena;

	ImguiContainer field_container;
	ListPanelLayout left_panel_layout;

	//String *ability_names;

	int active_index; // Index of the active element (for things like text box input)
	int text_cursor_pos;
	String field_labels[2];
	String field_strings[2];
	Vec2f field_positions[2];
	//Utf32String search_label;
};

void StartEditor(Editor *editor);

#endif