#include "imgui.h"

#include "draw.h"

bool ActiveContainerIsValid()
{
	bool is_valid = true;

	// Check that the active_container isn't nullptr
	if(imgui::active_container == nullptr)
	{
		log("imgui::active_container not set when it was expected to be.");
		is_valid = false;
	}

	// Check that the label font for the button layout is init'd.
	if(!ValidFont(imgui::active_container->button_layout.label_layout.font))
	{
		log("imgui::active_container->button_layout has no font loaded when it was expected to have one.");
		is_valid = false;
	}

	// Check that the text font for the text entry layout is init'd.
	if(!ValidFont(imgui::active_container->text_entry_layout.text_layout.font))
	{
		log("imgui::active_container->text_layout has no font loaded when it was expected to have one.");
		is_valid = false;
	}

	return is_valid;
}

ButtonResponse
Button(const char *label, ...)
{
	if(!ActiveContainerIsValid()) return ButtonResponse{};

	ButtonLayout layout = imgui::active_container->button_layout;

	ButtonResponse response = {};

	bool button_pressed = false;

	char *formatted_string;
	mFormatString(formatted_string, label);

	Vec2f label_size = SizeUtf8Line(layout.label_layout, label);
	//Vec2f padding_multiplier = {0.2f,0.2f};
	//Vec2f padding = padding_multiplier*label_size;
	Vec2f padded_size = label_size+2*imgui::button_padding;

	Rect padded_rect = {imgui::active_container->pos+imgui::active_container->pen, padded_size};
	padded_rect = AlignRect(padded_rect, layout.align);
	response.rect = padded_rect;
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
	if(PointInRect(padded_rect, input::global_input->mouse_pos))
	{
		response.hovered = true;

		outline_color = layout.button_hover_color;
		label_color = layout.label_hover_color;

		if(Pressed(vk::lmb))
		{
			response.pressed = true;
		}
	}
	else
	{
		outline_color = layout.button_color;
		label_color = layout.label_layout.color;
	}
	layout.label_layout.color = label_color;

	DrawFilledRect(padded_rect, c::black);
	DrawUnfilledRect(padded_rect, outline_color);
	DrawText(layout.label_layout, padded_rect.pos+0.5f*padded_rect.size, label);

	return response;
}

TextEntryResponse TextEntry(TextEntryLayout layout, Vec2f origin, Utf32String label, Utf32String text, bool active=false, int cursor_index=-1)
{
	TextEntryResponse response = {};

	Vec2f label_size = SizeTextUtf32(layout.label_layout, label);
	Rect full_rect = {origin, layout.size + Vec2f{0.f, label_size.y}};
	Vec2f label_pos = full_rect.pos;
	Rect text_rect = {full_rect.pos + Vec2f{0.f, label_size.y}, layout.size};

	// Draw label
	DrawTextUtf32(layout.label_layout, label_pos, label);

	// Draw
	DrawFilledRect(text_rect, c::dk_grey);

	// Draw border: green if active, grey otherwise
	if(active) DrawUnfilledRect(text_rect, c::green);
	else DrawUnfilledRect(text_rect, c::grey);

	// Draw text
	DrawTextUtf32(layout.text_layout, text_rect.pos + Vec2f{0.f, 0.5f*text_rect.size.y}, text);

	// Draw text cursor
	if(active and cursor_index >= 0)
	{
		Vec2f cursor_pos = text_rect.pos + Vec2f{SizeTextUtf32(layout.text_layout, text, cursor_index).x, 0.f};
		//DrawText(c::def_text_layout, {500.f,500.f}, "%f,%f", cursor_pos.x, cursor_pos.y);
		DrawLine(cursor_pos, cursor_pos + Vec2f{0.f, text_rect.size.y}, c::red);
	}

	if(Pressed(vk::LMB) and PointInRect(text_rect, MousePos()))
	{
		response.pressed = true;
	}

	return response;
}

TextEntryResponse TextEntry(TextEntryLayout layout, Vec2f origin, String label, String text, bool active=false, int cursor_index=-1)
{
	TextEntryResponse response = {};

	Vec2f label_size = SizeText(layout.label_layout, label);
	Rect full_rect = {origin, layout.size + Vec2f{0.f, label_size.y}};
	Vec2f label_pos = full_rect.pos;
	Rect text_rect = {full_rect.pos + Vec2f{0.f, label_size.y}, layout.size};

	// Draw label
	DrawText(layout.label_layout, label_pos, label);

	// Draw
	DrawFilledRect(text_rect, c::dk_grey);

	// Draw border: green if active, grey otherwise
	if(active) DrawUnfilledRect(text_rect, c::green);
	else DrawUnfilledRect(text_rect, c::grey);

	// Draw text
	DrawText(layout.text_layout, text_rect.pos + Vec2f{0.f, 0.5f*text_rect.size.y}, text);

	// Draw text cursor
	if(active and cursor_index >= 0)
	{
		Vec2f cursor_pos = text_rect.pos + Vec2f{SizeText(layout.text_layout, text, cursor_index).x, 0.f};
		//DrawText(c::def_text_layout, {500.f,500.f}, "%f,%f", cursor_pos.x, cursor_pos.y);
		DrawLine(cursor_pos, cursor_pos + Vec2f{0.f, text_rect.size.y}, c::red);
	}

	if(Pressed(vk::LMB) and PointInRect(text_rect, MousePos()))
	{
		response.pressed = true;
	}

	return response;
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
GetButtonHeight(ImguiContainer container)
{
	return LineSize(container.button_layout.label_layout) + 2*imgui::button_padding.y;
}

ListPanelResponse
ListPanel(ListPanelLayout layout, String *entry_names, size_t entry_count, float scroll_offset)
{
	ListPanelResponse response = {
		.hovered_index = -1,
		.pressed_index = -1
	};

	DrawFilledRect(layout.rect, Color{0.05f,0.05f,0.05f,1.f});
	DrawUnfilledRect(layout.rect, c::lt_grey);
	//DrawLine(RectTopRight(layout.rect), RectBottomRight(layout.rect), c::lt_grey);

	TextLayout text_layout = c::def_text_layout;
	text_layout.font_size = 16;
	text_layout.draw_debug = false;

	Vec2f outer_padding = {20.f,2.f};
	Vec2f inner_padding = {5.f,5.f};
	Vec2f pen = layout.rect.pos + Vec2f{0.f,20.f};

	// Calculate which entries should be drawn based on scroll_offset
	float v_distance_between_entries = 2.0f*outer_padding.y + 2.0f*inner_padding.y + LineSize(text_layout);
	int index_of_first_drawn_entry = (int)(scroll_offset / v_distance_between_entries);
	float remainder = v_distance_between_entries*((scroll_offset / v_distance_between_entries) - index_of_first_drawn_entry);
	//pen.y += remainder;

	// DrawText(c::def_text_layout, MousePos(), "%d", index_of_first_drawn_entry);

	for(int i=0; i<entry_count; i++)
	{
		if(i < index_of_first_drawn_entry) continue;
		Rect entry_rect = {pen+outer_padding-Vec2f{0.0f, remainder}, Vec2f{layout.rect.size.x-2.f*outer_padding.x, LineSize(text_layout)+2.f*inner_padding.y}};

		if(PointInRect(entry_rect, MousePos()))
		{ // Entry hovered
			DrawFilledRect(entry_rect, c::grey);
			DrawUnfilledRect(entry_rect, c::green);
			response.hovered_index = i;
			if(Pressed(vk::LMB)) response.pressed_index = i;
		}
		else
		{ // Entry NOT hovered
			DrawFilledRect(entry_rect, c::dk_grey);
			DrawUnfilledRect(entry_rect, c::grey);
		}

		// @note: spooky! We should write a real implementation of DrawText that takes a String.
		Vec2f text_size = DrawText(text_layout, entry_rect.pos + inner_padding, entry_names[i]);
		//SetDrawDepth(120.0f);


		pen.y += text_size.y + 2.f*(outer_padding+inner_padding).y;
	}

	return response;
}

IntegerBoxResponse
IntegerBox(IntegerBoxLayout layout, Vec2f pos, String label, String text)
{
	IntegerBoxResponse response = {};

	Rect box_rect = {pos, layout.size};
	DrawFilledRect(box_rect, c::dk_grey);
	DrawUnfilledRect(box_rect, layout.border_color);
	DrawText(layout.label_layout, RectTopLeft(box_rect), label);
	DrawText(layout.text_layout, RectCenter(box_rect), text);

	if(MouseInRect(box_rect))
	{
		int scroll = MouseScroll();

		response.value_change = scroll;
		if(Down(vk::shift)) response.value_change *= 10;
	}

	return response;
}