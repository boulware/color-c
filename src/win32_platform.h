#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

#include <windows.h>

struct WindowContext
{
	HWND hwnd;
	HDC hdc;
	HGLRC rc;
};

#endif