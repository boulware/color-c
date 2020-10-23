#ifndef INPUT_H
#define INPUT_H

#include "types.h"
#include "input_vk_constants.h"

struct InputState
{
    u8 pressed_keys[256];
    u8 released_keys[256];
    u8 repeated_keys[256];
    u8 down_keys[256];

    u8 any_key; // Once the platform is signaled, it will place the next key press
                // into this member, and the game layer can access it as needed.

    Vec2f prev_mouse_pos;
    Vec2f mouse_pos;
    int mouse_scroll;

    bool mouse_focus_taken;

    // A utf-32 encoded bytestream which is updated by the platform with translated characters
    // just before the update and render is called. It is variable-length, and its length is
    // determined by how many translated characters were caught by the platform (in the case of
    // Windows, in the WM_CHAR event) since the beginning of the last frame. The platform should
    // null terminate it.
    u32 *utf32_translated_stream;
};

namespace input
{
    InputState *global_input;
}

void ResetInputState(InputState *input);

bool Down(u8 key);
bool Pressed(u8 key);
bool Released(u8 key);
bool Repeated(u8 key);
u8 GetAnyKey();
Vec2f PrevMousePos(bool absolute = false);
Vec2f MousePos(bool absolute = false);
bool MouseMoved();
int MouseScroll();
bool MouseInRect(Rect rect, bool absolute = false);
bool MouseFocusTaken();

bool Down(const InputState *input, u8 key);
bool Pressed(const InputState *input, u8 key);
bool Released(const InputState *input, u8 key);

#endif