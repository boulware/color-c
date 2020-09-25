#ifndef IMGUI_H
#define IMGUI_H

#include "text_render.h"

struct ButtonLayout
{
	TextLayout label_layout;
	Color button_color;
	Color button_hover_color, label_hover_color;
	Align align;
};

struct ImguiContainer
{
	Vec2f pos; // Top left corner of container in window coordinates
	Vec2f max_size; // Maximum size the container should stretch to
	Vec2f pen; // Offset relative to [pos] where the end of the last element occured. (i.e., where the next element should be placed)
	//int font_size;

	ButtonLayout button_layout;
};

struct ButtonResponse
{
	bool pressed;
	bool hovered;
	Rect rect;
};

namespace imgui
{
	ImguiContainer *active_container;
	Vec2f button_padding = {5.f,5.f};
};

ButtonResponse Button(const char *label, ...);
float GetButtonHeight(ImguiContainer container);

void ResetImguiContainer(ImguiContainer *container);
void SetActiveContainer(ImguiContainer *container);

#endif