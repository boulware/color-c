#ifndef IMGUI_H
#define IMGUI_H

struct ImguiContainer
{
	Vec2f pos; // Top left corner of container in window coordinates
	Vec2f max_size; // Maximum size the container should stretch to
	Vec2f pen; // Offset relative to [pos] where the end of the last element occured. (i.e., where the next element should be placed)
	int font_size;
};

struct ButtonResponse
{
	bool pressed;
	bool hovered;
};

namespace imgui
{
	ImguiContainer *active_container;
	Vec2f button_padding = {5.f,5.f};
};

ButtonResponse ButtonColor(Color color, Color hover_color, const char *label, ...);
ButtonResponse Button(const char *label, ...);

void ResetImguiContainer(ImguiContainer *container);
void SetActiveContainer(ImguiContainer *container);

#endif