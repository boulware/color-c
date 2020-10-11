#ifndef INPUT_H
#define INPUT_H

#include "types.h"

namespace vk
{
    const u8 a = 'A'; const u8 A = 'A';
    const u8 b = 'B'; const u8 B = 'B';
    const u8 c = 'C'; const u8 C = 'C';
    const u8 d = 'D'; const u8 D = 'D';
    const u8 e = 'E'; const u8 E = 'E';
    const u8 f = 'F'; const u8 F = 'F';
    const u8 g = 'G'; const u8 G = 'G';
    const u8 h = 'H'; const u8 H = 'H';
    const u8 i = 'I'; const u8 I = 'I';
    const u8 j = 'J'; const u8 J = 'J';
    const u8 k = 'K'; const u8 K = 'K';
    const u8 l = 'L'; const u8 L = 'L';
    const u8 m = 'M'; const u8 M = 'M';
    const u8 n = 'N'; const u8 N = 'N';
    const u8 o = 'O'; const u8 O = 'O';
    const u8 p = 'P'; const u8 P = 'P';
    const u8 q = 'Q'; const u8 Q = 'Q';
    const u8 r = 'R'; const u8 R = 'R';
    const u8 s = 'S'; const u8 S = 'S';
    const u8 t = 'T'; const u8 T = 'T';
    const u8 u = 'U'; const u8 U = 'U';
    const u8 v = 'V'; const u8 V = 'V';
    const u8 w = 'W'; const u8 W = 'W';
    const u8 x = 'X'; const u8 X = 'X';
    const u8 y = 'Y'; const u8 Y = 'Y';
    const u8 z = 'Z'; const u8 Z = 'Z';
    const u8 num0 = '0';
    const u8 num1 = '1';
    const u8 num2 = '2';
    const u8 num3 = '3';
    const u8 num4 = '4';
    const u8 num5 = '5';
    const u8 num6 = '6';
    const u8 num7 = '7';
    const u8 num8 = '8';
    const u8 num9 = '9';
    const u8 left  = 0x25;
    const u8 up    = 0x26;
    const u8 right = 0x27;
    const u8 down  = 0x28;
    const u8 f1  = 0x70; const u8 F1  = 0x70;
    const u8 f2  = 0x71; const u8 F2  = 0x71;
    const u8 f3  = 0x72; const u8 F3  = 0x72;
    const u8 f4  = 0x73; const u8 F4  = 0x73;
    const u8 f5  = 0x74; const u8 F5  = 0x74;
    const u8 f6  = 0x75; const u8 F6  = 0x75;
    const u8 f7  = 0x76; const u8 F7  = 0x76;
    const u8 f8  = 0x77; const u8 F8  = 0x77;
    const u8 f9  = 0x78; const u8 F9  = 0x78;
    const u8 f10 = 0x79; const u8 F10 = 0x79;
    const u8 f11 = 0x7A; const u8 F11 = 0x7A;
    const u8 f12 = 0x7B; const u8 F12 = 0x7B;
    const u8 space = 0x20;
    const u8 shift = 0x10;
    const u8 ctrl = 0x11;
    const u8 alt = 0x12;
    const u8 tab = 0x09;
    const u8 tilde = 0xC0;
    const u8 grave = 0xC0;
    const u8 RightBracket = 0xDD;
    const u8 NumpadMinus = 0x6D;
    const u8 esc = 0x1B; const u8 escape = 0x1B;
    const u8 backspace = 0x08;
    const u8 del = 0x2E;
    const u8 enter = 0x0D;

    // Mouse
    const u8 LMB = 0x01; const u8 lmb = 0x01;
    const u8 RMB = 0x02; const u8 rmb = 0x02;
    const u8 MMB = 0x04; const u8 mmb = 0x04;
    const u8 Mouse4 = 0x05; const u8 mouse4 = 0x05;
    const u8 Mouse5 = 0x06; const u8 mouse5 = 0x06;
};

enum class VirtualKey : u8
{
    None           = 0x00,
    LMB            = 0x01,
    RMB            = 0x02,
    Cancel         = 0x03,
    MMB            = 0x04,
    Mouse4         = 0x05,
    Mouse5         = 0x06,
    Undefined0     = 0x07,
    Backspace      = 0x08,
    Tab            = 0x09,
    Reserved0      = 0x0A,
    Reserved1      = 0x0B,
    Clear          = 0x0C,
    Enter          = 0x0D,
    Undefined1     = 0x0E,
    Undefined2     = 0x0F,
    Shift          = 0x10,
    Ctrl           = 0x11,
    Alt            = 0x12,
    Pause          = 0x13,
    CapsLock       = 0x14,
    IME_Kana       = 0x15,
    IME_On         = 0x16,
    IME_Junja      = 0x17,
    IME_Final      = 0x18,
    IME_Hanja      = 0x19,
    IME_Off        = 0x1A,
    Esc            = 0x1B,
    IME_Convert    = 0x1C,
    IME_NonConvert = 0x1D,
    IME_Accept     = 0x1E,
    IME_ModeChange = 0x1F,
    Space          = 0x20,
    PageUp         = 0x21,
    PageDown       = 0x22,
    End            = 0x23,
    Home           = 0x24,
    Left           = 0x25,
    Up             = 0x26,
    Right          = 0x27,
    Down           = 0x28,
    Select         = 0x29,
    Print          = 0x2A,
    Execute        = 0x2B,
    PrintScreen    = 0x2C,
    Insert         = 0x2D,
    Delete         = 0x2E,
    Help           = 0x2F,
    Digit0         = 0x30,
    Digit1         = 0x31,
    Digit2         = 0x32,
    Digit3         = 0x33,
    Digit4         = 0x34,
    Digit5         = 0x35,
    Digit6         = 0x36,
    Digit7         = 0x37,
    Digit8         = 0x38,
    Digit9         = 0x39,
    Undefined3     = 0x3A,
    Undefined4     = 0x3B,
    Undefined5     = 0x3C,
    Undefined6     = 0x3D,
    Undefined7     = 0x3E,
    Undefined8     = 0x3F,
    Undefined9     = 0x40,
    A              = 0x41,
    B              = 0x42,
    C              = 0x43,
    D              = 0x44,
    E              = 0x45,
    F              = 0x46,
    G              = 0x47,
    H              = 0x48,
    I              = 0x49,
    J              = 0x4A,
    K              = 0x4B,
    L              = 0x4C,
    M              = 0x4D,
    N              = 0x4E,
    O              = 0x4F,
    P              = 0x50,
    Q              = 0x51,
    R              = 0x52,
    S              = 0x53,
    T              = 0x54,
    U              = 0x55,
    V              = 0x56,
    W              = 0x57,
    X              = 0x58,
    Y              = 0x59,
    Z              = 0x5A,
    LeftWin        = 0x5B,
    RightWin       = 0x5C,
    Apps           = 0x5D,
    Reserved2      = 0x5E,
    Sleep          = 0x5F,
    Numpad0        = 0x60,
    Numpad1        = 0x61,
    Numpad2        = 0x62,
    Numpad3        = 0x63,
    Numpad4        = 0x64,
    Numpad5        = 0x65,
    Numpad6        = 0x66,
    Numpad7        = 0x67,
    Numpad8        = 0x68,
    Numpad9        = 0x69,
    Multiply       = 0x6A,
    Add            = 0x6B,
    Separator      = 0x6C,
    Subtract       = 0x6D,
    Decimal        = 0x6E,
    Divide         = 0x6F,
    F1             = 0x70,
    F2             = 0x71,
    F3             = 0x72,
    F4             = 0x73,
    F5             = 0x74,
    F6             = 0x75,
    F7             = 0x76,
    F8             = 0x77,
    F9             = 0x78,
    F10            = 0x79,
    F11            = 0x7A,
    F12            = 0x7B,
    F13            = 0x7C,
    F14            = 0x7D,
    F15            = 0x7E,
    F16            = 0x7F,
    F17            = 0x80,
    F18            = 0x81,
    F19            = 0x82,
    F20            = 0x83,
    F21            = 0x84,
    F22            = 0x85,
    F23            = 0x86,
    F24            = 0x87,
    Unassigned0    = 0x88,
    Unassigned1    = 0x89,
    Unassigned2    = 0x8A,
    Unassigned3    = 0x8B,
    Unassigned4    = 0x8C,
    Unassigned5    = 0x8D,
    Unassigned6    = 0x8E,
    Unassigned7    = 0x8F,
    NumLock        = 0x90,
    ScrollLock     = 0x91,
    OEM_Specific0  = 0x92,
    OEM_Specific1  = 0x93,
    OEM_Specific2  = 0x94,
    OEM_Specific3  = 0x95,
    OEM_Specific4  = 0x96,
    Unassigned8    = 0x97,
    Unassigned9    = 0x98,
    Unassigned10   = 0x99,
    Unassigned11   = 0x9A,
    Unassigned12   = 0x9B,
    Unassigned13   = 0x9C,
    Unassigned14   = 0x9D,
    Unassigned15   = 0x9E,
    Unassigned16   = 0x9F,
    LeftShift      = 0xA0,
    RightShift     = 0xA1,
    LeftCtrl       = 0xA2,
    RightCtrl      = 0xA3,
    LeftAlt        = 0xA4,
    RightAlt       = 0xA5,
    BrowserBack    = 0xA6,
    BrowserForward = 0xA7,
    BrowserRefresh = 0xA8,
    BrowserStop    = 0xA9,
    BrowserSearch  = 0xAA,
    BrowserFavs    = 0xAB,
    BrowserHome    = 0xAC,
    VolumeMute     = 0xAD,
    VolumeDown     = 0xAE,
    VolumeUp       = 0xAF,
    MediaNext      = 0xB0,
    MediaPrev      = 0xB1,
    MediaStop      = 0xB2,
    MediaPlayPause = 0xB3,
    LaunchMail     = 0xB4,
    LaunchMedia    = 0xB5,
    LaunchApp1     = 0xB6,
    LaunchApp2     = 0xB7,
    Reserved3      = 0xB8,
    Reserved4      = 0xB9,
    OEM_1          = 0xBA,
    OEM_Plus       = 0xBB,
    OEM_Comma      = 0xBC,
    OEM_Minus      = 0xBD,
    OEM_Period     = 0xBE,
    OEM_2          = 0xBF,
    OEM_3          = 0xC0,
    Reserved5      = 0XC1,
    Reserved6      = 0XC2,
    Reserved7      = 0XC3,
    Reserved8      = 0XC4,
    Reserved9      = 0XC5,
    Reserved10     = 0XC6,
    Reserved11     = 0XC7,
    Reserved12     = 0XC8,
    Reserved13     = 0XC9,
    Reserved14     = 0XCA,
    Reserved15     = 0XCB,
    Reserved16     = 0XCC,
    Reserved17     = 0XCD,
    Reserved18     = 0XCE,
    Reserved19     = 0XCF,
    Reserved20     = 0XD0,
    Reserved21     = 0XD1,
    Reserved22     = 0XD2,
    Reserved23     = 0XD3,
    Reserved24     = 0XD4,
    Reserved25     = 0XD5,
    Reserved26     = 0XD6,
    Reserved27     = 0XD7,
    Unassigned17   = 0XD8,
    Unassigned18   = 0XD9,
    Unassigned19   = 0XDA,
    OEM_4          = 0xDB,
    OEM_5          = 0xDC,
    OEM_6          = 0xDD,
    OEM_7          = 0xDE,
    OEM_8          = 0xDF,
    Reserved28     = 0xE0,
    OEM_Specific5  = 0xE1,
    OEM_102        = 0xE2,
    OEM_Specific6  = 0xE3,
    OEM_Specific7  = 0xE4,
    IME_Process    = 0xE5,
    OEM_Specific8  = 0xE6,
    Packet         = 0xE7,
    Unassigned20   = 0xE8,
    OEM_Specific9  = 0xE9,
    OEM_Specific10 = 0xEA,
    OEM_Specific11 = 0xEB,
    OEM_Specific12 = 0xEC,
    OEM_Specific13 = 0xED,
    OEM_Specific14 = 0xEE,
    OEM_Specific15 = 0xEF,
    OEM_Specific16 = 0xF0,
    OEM_Specific17 = 0xF1,
    OEM_Specific18 = 0xF2,
    OEM_Specific19 = 0xF3,
    OEM_Specific20 = 0xF4,
    OEM_Specific21 = 0xF5,
    Attn           = 0xF6,
    CrSel          = 0xF7,
    ExSel          = 0xF8,
    EraseEOF       = 0xF9,
    Play           = 0xFA,
    Zoom           = 0xFB,
    NoName         = 0xFC,
    PA1            = 0xFD,
    OEM_Clear      = 0xFE,
    COUNT          = 0xFF
 };

const char *VirtualKey_userstrings[VirtualKey::COUNT] = {
    "None",
    "LMB",
    "RMB",
    "Cancel",
    "MMB",
    "Mouse4",
    "Mouse5",
    "Undefined0",
    "Backspace",
    "Tab",
    "Reserved0",
    "Reserved1",
    "Clear",
    "Enter",
    "Undefined1",
    "Undefined2",
    "Shift",
    "Ctrl",
    "Alt",
    "Pause",
    "CapsLock",
    "IME_Kana",
    "IME_On",
    "IME_Junja",
    "IME_Final",
    "IME_Hanja",
    "IME_Off",
    "Esc",
    "IME_Convert",
    "IME_NonConvert",
    "IME_Accept",
    "IME_ModeChange",
    "Space",
    "PageUp",
    "PageDown",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Select",
    "Print",
    "Execute",
    "PrintScreen",
    "Insert",
    "Delete",
    "Help",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "Undefined3",
    "Undefined4",
    "Undefined5",
    "Undefined6",
    "Undefined7",
    "Undefined8",
    "Undefined9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "LeftWin",
    "RightWin",
    "Apps",
    "Reserved2",
    "Sleep",
    "Numpad0",
    "Numpad1",
    "Numpad2",
    "Numpad3",
    "Numpad4",
    "Numpad5",
    "Numpad6",
    "Numpad7",
    "Numpad8",
    "Numpad9",
    "Multiply",
    "Add",
    "Separator",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Unassigned0",
    "Unassigned1",
    "Unassigned2",
    "Unassigned3",
    "Unassigned4",
    "Unassigned5",
    "Unassigned6",
    "Unassigned7",
    "NumLock",
    "ScrollLock",
    "OEM_Specific0",
    "OEM_Specific1",
    "OEM_Specific2",
    "OEM_Specific3",
    "OEM_Specific4",
    "Unassigned8",
    "Unassigned9",
    "Unassigned10",
    "Unassigned11",
    "Unassigned12",
    "Unassigned13",
    "Unassigned14",
    "Unassigned15",
    "Unassigned16",
    "LeftShift",
    "RightShift",
    "LeftCtrl",
    "RightCtrl",
    "LeftAlt",
    "RightAlt",
    "BrowserBack",
    "BrowserForward",
    "BrowserRefresh",
    "BrowserStop",
    "BrowserSearch",
    "BrowserFavs",
    "BrowserHome",
    "VolumeMute",
    "VolumeDown",
    "VolumeUp",
    "MediaNext",
    "MediaPrev",
    "MediaStop",
    "MediaPlayPause",
    "LaunchMail",
    "LaunchMedia",
    "LaunchApp1",
    "LaunchApp2",
    "Reserved3",
    "Reserved4",
    "OEM_1",
    "OEM_Plus",
    "OEM_Comma",
    "OEM_Minus",
    "OEM_Period",
    "OEM_2",
    "OEM_3",
    "Reserved5",
    "Reserved6",
    "Reserved7",
    "Reserved8",
    "Reserved9",
    "Reserved10",
    "Reserved11",
    "Reserved12",
    "Reserved13",
    "Reserved14",
    "Reserved15",
    "Reserved16",
    "Reserved17",
    "Reserved18",
    "Reserved19",
    "Reserved20",
    "Reserved21",
    "Reserved22",
    "Reserved23",
    "Reserved24",
    "Reserved25",
    "Reserved26",
    "Reserved27",
    "Unassigned17",
    "Unassigned18",
    "Unassigned19",
    "OEM_4",
    "OEM_5",
    "OEM_6",
    "OEM_7",
    "OEM_8",
    "Reserved28",
    "OEM_Specific5",
    "OEM_102",
    "OEM_Specific6",
    "OEM_Specific7",
    "IME_Process",
    "OEM_Specific8",
    "Packet",
    "Unassigned20",
    "OEM_Specific9",
    "OEM_Specific10",
    "OEM_Specific11",
    "OEM_Specific12",
    "OEM_Specific13",
    "OEM_Specific14",
    "OEM_Specific15",
    "OEM_Specific16",
    "OEM_Specific17",
    "OEM_Specific18",
    "OEM_Specific19",
    "OEM_Specific20",
    "OEM_Specific21",
    "Attn",
    "CrSel",
    "ExSel",
    "EraseEOF",
    "Play",
    "Zoom",
    "NoName",
    "PA1",
    "Clear",
};

// "Deselect": rmb
// find "Deselect" at index 1 of keybind_strings[]
// set keybind_vks[1] equal to rmb converted to an integer through some kind of vk string to enum array.

// namespace keybind
// {
//  u8 select = vk::lmb;
//  u8 exit = vk::esc;
// }

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
Vec2f PrevMousePos();
Vec2f MousePos();
bool MouseMoved();
int MouseScroll();
bool MouseInRect(Rect rect);

bool Down(const InputState *input, u8 key);
bool Pressed(const InputState *input, u8 key);
bool Released(const InputState *input, u8 key);

#endif