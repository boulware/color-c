#include "draw.h"

void
ActivateColorShader(Color color)
{
	gl->UseProgram(game->color_shader);
	gl->ProgramUniform4f(game->color_shader, 1, color.r, color.g, color.b, color.a);
	gl->BindVertexArray(game->color_vao);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->color_vbo);
}

void
ActivateUvShader(Color color={1.f,1.f,1.f,0.f})
{
	gl->UseProgram(game->uv_shader);
	gl->BindVertexArray(game->uv_vao);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);
	gl->ActiveTexture(GL_TEXTURE0);
	gl->ProgramUniform4f(game->uv_shader, 2, color.r, color.g, color.b, color.a);
}

void
SetDrawDepth(float depth)
{
	float clip_space_depth = c::clip_A*depth+c::clip_B;
	gl->ProgramUniform1f(game->uv_shader, 3, clip_space_depth);
	gl->ProgramUniform1f(game->color_shader, 2, clip_space_depth);
}

void
DrawUnfilledRect(Rect rect, Color color)
{
	ActivateColorShader(color);

	GLfloat verts[] = {
		rect.pos.x, rect.pos.y,
		rect.pos.x + rect.size.x, rect.pos.y,
		rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
		rect.pos.x, rect.pos.y + rect.size.y
	};

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_LINE_LOOP, 0, 4);
}

void
DrawUnfilledRect(Vec2f pos, Vec2f size, Color color)
{
	return DrawUnfilledRect({pos,size}, color);
}

void
DrawFilledRect(Rect rect, Color color)
{
	ActivateColorShader(color);

	GLfloat verts[] = {
		rect.pos.x, rect.pos.y,
		rect.pos.x + rect.size.x, rect.pos.y,
		rect.pos.x, rect.pos.y + rect.size.y,
		rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
	};

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
DrawFilledRect(Vec2f pos, Vec2f size, Color color)
{
	return DrawFilledRect({pos,size}, color);
}

void
DrawLine(Vec2f start, Vec2f end, Color color=c::white)
{
	ActivateColorShader(color);

	GLfloat verts[] = {
		start.x, start.y,
		end.x, end.y
	};

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_LINES, 0, 2);
}

// A healthbar in this context is a rectangle, which scales in size proportionally to a value
// relative to a maximum value. i.e., a canonical healthbar
void
DrawHealthbar()
{

}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, String label)
{
	ButtonResponse response = {};

	Rect aligned_button_rect = AlignRect(rect, layout.align);
	response.rect = aligned_button_rect;

	if(PointInRect(aligned_button_rect, MousePos()))
	{
		response.hovered = true;
		if(!PointInRect(aligned_button_rect, PrevMousePos()))
		{
			response.just_now_hovered = true;
		}
	}

	if(response.hovered)
	{
		// Button is being hovered
		DrawUnfilledRect(aligned_button_rect, layout.button_hover_color);
		TextLayout hovered_layout = layout.label_layout;
		hovered_layout.color = layout.label_hover_color;
		DrawText(hovered_layout, RectCenter(aligned_button_rect), label);

		response.hovered = true;
		if(Pressed(vk::LMB)) response.pressed = true;
	}
	else
	{
		// Button is NOT being hovered
		DrawUnfilledRect(aligned_button_rect, layout.button_color);
		DrawText(layout.label_layout, RectCenter(aligned_button_rect), label);
	}

	return response;
}

ButtonResponse
DrawButton(ButtonLayout layout, Rect rect, const char *c_string)
{
	return DrawButton(layout, rect, StringFromCString(c_string));
}