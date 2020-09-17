#ifndef SPRITE_H
#define SPRITE_H

#include "vec.h"

struct Sprite
{
	GLuint texture;
	Vec2f size;
	Vec2f origin;
};

GLuint GenerateAndBindTexture();

#endif