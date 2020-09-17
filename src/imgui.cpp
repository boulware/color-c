#include "imgui.h"

#include "draw.h"

ButtonResponse
ButtonColor(Color color, Color hover_color, const char *label, ...)
{
	ButtonResponse response = {};

	if(imgui::active_container == nullptr) return response;

	bool button_pressed = false;

	va_list args;
	va_start(args, label);

	char formatted_string[c::max_text_render_length];
	int formatted_length = vsprintf(formatted_string, label, args);
	if(formatted_length > c::max_text_render_length)
	{
		log("Button() received a label longer than c::max_text_render_length (%d). "
			"The label is still rendered, but clipped to max render length.",
			c::max_text_render_length);

		formatted_string[c::max_text_render_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	Vec2f label_size = SizeUtf8Line(imgui::active_container->font_size, label);
	//Vec2f padding_multiplier = {0.2f,0.2f};
	//Vec2f padding = padding_multiplier*label_size;
	Vec2f padded_size = label_size+2*imgui::button_padding;

	Rect padded_rect = {imgui::active_container->pos+imgui::active_container->pen, padded_size};
	// Check if there's enough horizontal room left for this button.
	if(padded_size.x > imgui::active_container->max_size.x - imgui::active_container->pen.x)
	{
		imgui::active_container->pen.x = 0.f;
		imgui::active_container->pen.y += padded_size.y+1; // +1 so rects don't overlap on edges
	}
	else
	{
		imgui::active_container->pen.x += padded_rect.size.x+1; // +1 so rects don't overlap on edges
	}

	Color outline_color;
	Color label_color;
	if(PointInRect(padded_rect, game->mouse_pos))
	{
		response.hovered = true;

		outline_color = hover_color;
		label_color = hover_color;

		if(Pressed(vk::lmb))
		{
			response.pressed = true;
		}
	}
	else
	{
		outline_color = color;
		label_color = color;
	}

	DrawFilledRect(padded_rect, c::black);
	DrawUnfilledRect(padded_rect, outline_color);
	DrawTextColor(imgui::active_container->font_size, padded_rect.pos+imgui::button_padding, label_color, label);

	return response;
}

ButtonResponse
Button(const char *label, ...)
{
	va_list args;
	va_start(args, label);

	char formatted_string[c::max_text_render_length];
	int formatted_length = vsprintf(formatted_string, label, args);
	if(formatted_length > c::max_text_render_length)
	{
		log("Button() received a label longer than c::max_text_render_length (%d). "
			"The label is still rendered, but clipped to max render length.",
			c::max_text_render_length);

		formatted_string[c::max_text_render_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	return ButtonColor(c::grey, c::white, formatted_string);
}

void
ResetImguiContainer(ImguiContainer *container)
{
	container->pen = {0.f,0.f};
}

void
SetActiveContainer(ImguiContainer *container)
{
	imgui::active_container = container;
}

float
GetButtonHeight(ImguiContainer *container = imgui::active_container)
{
	return LineSize(container->font_size) + 2*imgui::button_padding.y;
}