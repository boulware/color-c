#include "draw.h"

void
ActivateColorShader(Color color)
{
	gl->UseProgram(game->color_shader);
	gl->ProgramUniform4f(game->color_shader, 1, color.r, color.g, color.b, 1.f);
	gl->BindVertexArray(game->color_vao);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->color_vbo);
}

void ActivateUvShader(GLuint texture)
{
	gl->UseProgram(game->uv_shader);
	gl->BindVertexArray(game->uv_vao);
	gl->BindBuffer(GL_ARRAY_BUFFER, game->uv_vbo);
	gl->ActiveTexture(GL_TEXTURE0);
	gl->BindTexture(GL_TEXTURE_2D, texture);
}

void DrawUnfilledRect(Rect rect, Color color)
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

void DrawFilledRect(Rect rect, Color color)
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

void DrawLine(Vec2f start, Vec2f end, Color color)
{
	ActivateColorShader(color);

	GLfloat verts[] = {
		start.x, start.y,
		end.x, end.y
	};

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_LINES, 0, 2);
}