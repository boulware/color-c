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
	const u8 left = 0x25;
	const u8 up = 0x26;
	const u8 right = 0x27;
	const u8 down = 0x28;
	const u8 f1  = 	0x70; const u8 F1  = 0x70;
	const u8 f2  = 	0x71; const u8 F2  = 0x71;
	const u8 f3  = 	0x72; const u8 F3  = 0x72;
	const u8 f4  = 	0x73; const u8 F4  = 0x73;
	const u8 f5  = 	0x74; const u8 F5  = 0x74;
	const u8 f6  = 	0x75; const u8 F6  = 0x75;
	const u8 f7  = 	0x76; const u8 F7  = 0x76;
	const u8 f8  = 	0x77; const u8 F8  = 0x77;
	const u8 f9  = 	0x78; const u8 F9  = 0x78;
	const u8 f10 =  0x79; const u8 F10 = 0x79;
	const u8 f11 =  0x7A; const u8 F11 = 0x7A;
	const u8 f12 =  0x7B; const u8 F12 = 0x7B;
	const u8 space = 0x20;
	const u8 shift = 0x10;
	const u8 ctrl = 0x11;
	const u8 tilde = 0xC0;
	const u8 grave = 0xC0;
	const u8 RightBracket = 0xDD;
	const u8 NumpadMinus = 0x6D;

	// Mouse
	const u8 LMB = 0x01; const u8 lmb = 0x01;
	const u8 RMB = 0x02; const u8 rmb = 0x02;
	const u8 MMB = 0x04; const u8 mmb = 0x04;
	const u8 Mouse4 = 0x05;
	const u8 Mouse5 = 0x06;
};

struct InputState
{
	u8 pressed_keys[256];
	u8 released_keys[256];
	s8 down_keys[256];
};

namespace input
{
	InputState *global_input;
}

bool Down(u8 key);
bool Pressed(u8 key);
bool Released(u8 key);

#endif