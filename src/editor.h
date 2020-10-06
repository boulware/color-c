#ifndef EDITOR_H
#define EDITOR_H

#include "memory.h"
#include "utf32string.h"

enum class EditorMode
{
	None,
	Ability,
	Breed,
};

enum class InputElementType
{
	None,
	String,
	Integer,
};

enum class AbilityPropertyIndex : int
{
	search,
	name,
	tier0required_vigor,
	tier0required_focus,
	tier0required_armor,
	tier1required_vigor,
	tier1required_focus,
	tier1required_armor,
	tier2required_vigor,
	tier2required_focus,
	tier2required_armor,
	COUNT
};

InputElementType ability_property_types[] = {
	InputElementType::String,
	InputElementType::String,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
	InputElementType::Integer,
}; static_assert(ArrayCount(ability_property_types) == (int)AbilityPropertyIndex::COUNT);

Vec2f ability_field_positions[] = {
	{10.f,10.f},
	{400.f,10.f},
	{400.f,100.f},
	{460.f,100.f},
	{520.f,100.f},
	{400.f,200.f},
	{460.f,200.f},
	{520.f,200.f},
	{400.f,300.f},
	{460.f,300.f},
	{520.f,300.f},
}; static_assert(ArrayCount(ability_field_positions) == (int)AbilityPropertyIndex::COUNT);

enum class BreedPropertyIndex : int
{
	COUNT
};

struct InputElement
{
	InputElementType type;

	String label;
	Vec2f pos;
	String text;
	void *value_ptr;
};

Introspect
struct Editor
{
	bool init;
	Arena arena;

	EditorMode mode;

	ListPanelLayout search_panel_layout = {
		.rect = {
			.pos = {0.f,100.f},
			.size = {300.f,700.f}
		}
	};

	float panel_scroll_acc;
	float panel_scroll_vel;
	float panel_scroll_pos;
	float panel_scroll_friction;
	float panel_scroll_velocity_minimum;

	int active_index; // Index of the active element (for things like text box input)
	int text_cursor_pos;

	// String search_label;
	//String search_string;
	// Vec2f search_pos;

	InputElement input_elements[11];
	static_assert(ArrayCount(input_elements) >= (int)AbilityPropertyIndex::COUNT);
	static_assert(ArrayCount(input_elements) >= (int)BreedPropertyIndex::COUNT);

	// String field_labels[10];
	// Vec2f field_positions[10];

	// Temp instances created while editing so we're not editing datatable entries directly,
	// so we can allow things like reverting changes, saving as copy,
	Ability temp_ability;
	Id<Ability> temp_ability_id;

	Breed temp_breed;
	Id<Breed> temp_breed_id;
};

void StartEditor(Editor *editor);
GameState TickEditor(Editor *editor);


#endif