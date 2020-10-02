#ifndef IMGUI_H
#define IMGUI_H

#include "text_render.h"
#include "string.h"

struct ListPanelLayout
{
	Rect rect;
};

struct ListPanelResponse
{
	int hovered_index;
	int pressed_index;
};

struct ListPanel_
{
	ListPanelLayout layout;
	int cur_entry_count;
	float scroll_offset;
};

struct ButtonLayout
{
	TextLayout label_layout;
	Color button_color;
	Color button_hover_color, label_hover_color;
	Align align;
};

struct TextEntryLayout
{
	Vec2f size;
	TextLayout label_layout;
	TextLayout text_layout;
	Align align;
};

struct IntegerBoxLayout
{
	Vec2f size;
	Color border_color;
	TextLayout label_layout;
	TextLayout text_layout;
};

struct IntegerBoxResponse
{
	int value_change;
};


struct ImguiContainer
{
	Vec2f pos; // Top left corner of container in window coordinates
	Vec2f max_size; // Maximum size the container should stretch to
	Vec2f pen; // Offset relative to [pos] where the end of the last element occured. (i.e., where the next element should be placed)
	//int font_size;

	ButtonLayout button_layout;
	TextEntryLayout text_entry_layout;
};

struct ButtonResponse
{
	bool pressed;
	bool hovered;
	Rect rect;
};

struct TextEntryResponse
{
	bool pressed;
	bool hovered;

	// -1 if not clicked. >=0 if the element was clicked, representing what text cursor pos the click corresponded to
	int clicked_cursor_pos;
};

namespace imgui
{
	ImguiContainer *active_container;
	Vec2f button_padding = {5.f,5.f};
};

ButtonResponse Button(const char *label, ...);
float GetButtonHeight(ImguiContainer container);

ListPanelResponse ListPanel(ListPanelLayout layout, String *entry_names, size_t entry_count);

//TextEntryResponse TextEntry(const char *text, int cursor_pos);

void ResetImguiContainer(ImguiContainer *container);
void SetActiveContainer(ImguiContainer *container);


void DrawListPanel_(ListPanel_ panel);

#endif