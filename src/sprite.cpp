#include "sprite.h"

GLuint
GenerateAndBindTexture()
{
	gl->ActiveTexture(GL_TEXTURE0);

	GLuint texture;
	gl->GenTextures(1, &texture);

	gl->BindTexture(GL_TEXTURE_2D, texture);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}

void
DrawSprite(Sprite sprite, Vec2f pos, Vec2f size={-1.f,-1.f})
{
	ActivateUvShader(sprite.texture);

	if(size == Vec2f{-1.f,-1.f})
	{
		// Use exact sprite size
		size = sprite.size;
	}

	pos -= Round(sprite.origin);

	GLfloat verts[] = {
		pos.x, pos.y, 0.f, 0.f,
		pos.x+size.x, pos.y, 1.f, 0.f,
		pos.x, pos.y+size.y, 0.f, 1.f,
		pos.x+size.x, pos.y+size.y, 1.f, 1.f
	};

	gl->BufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	gl->DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}