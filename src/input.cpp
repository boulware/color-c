#include "input.h"

bool
Down(u8 key)
{
	// win32 GetKeyboardState() sets the high bit to 1 if the key is down.
	return(input::global_input->down_keys[key] & 0x80);
}

bool
Pressed(u8 key)
{
	return(input::global_input->pressed_keys[key] == 1);
}

bool
Released(u8 key)
{
	return(input::global_input->released_keys[key] == 1);
}

bool
Repeated(u8 key)
{
	return(input::global_input->repeated_keys[key] == 1);
}

Vec2f
MousePos()
{
	return(input::global_input->mouse_pos);
}

int
MouseScroll()
{
	return(input::global_input->mouse_scroll);
}