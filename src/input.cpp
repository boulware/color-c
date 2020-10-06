#include "input.h"

void
ResetInputState(InputState *input)
{
	memset(&input->pressed_keys, 0, 256);
	memset(&input->released_keys, 0, 256);
	memset(&input->repeated_keys, 0, 256);
	input->mouse_scroll = 0;
}

bool
Down(const InputState *input, u8 key)
{
	// win32 GetKeyboardState() sets the high bit to 1 if the key is down.
	return(input->down_keys[key] & 0x80);
}

bool
Pressed(const InputState *input, u8 key)
{
	return(input->pressed_keys[key] == 1);
}

bool
Released(const InputState *input, u8 key)
{
	return(input->released_keys[key] == 1);
}

bool
Down(u8 key)
{
	if(!input::global_input) return false;

	// win32 GetKeyboardState() sets the high bit to 1 if the key is down.
	return(input::global_input->down_keys[key] & 0x80);
}

bool
Pressed(u8 key)
{
	if(!input::global_input) return false;

	return(input::global_input->pressed_keys[key] == 1);
}

bool
Released(u8 key)
{
	if(!input::global_input) return false;

	return(input::global_input->released_keys[key] == 1);
}

bool
Repeated(u8 key)
{
	if(!input::global_input) return false;

	return(input::global_input->repeated_keys[key] == 1);
}

Vec2f
PrevMousePos()
{
	if(!input::global_input) return Vec2f{};

	return(input::global_input->prev_mouse_pos);
}

Vec2f
MousePos()
{
	if(!input::global_input) return Vec2f{};

	return(input::global_input->mouse_pos);
}

bool
MouseInRect(Rect rect)
{
	return(PointInRect(rect, MousePos()));
}

int
MouseScroll()
{
	if(!input::global_input) return 0;

	return(input::global_input->mouse_scroll);
}