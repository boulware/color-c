#include "win32_platform.h"

#include <windows.h>
#include <GL/GL.h>
#include <cstdint>

#include "types.h"
#include "math.h"

#include "platform.h"
#include "opengl.h"
#include "game.h"

#include "util.h"
#include "strings.h"
#include "input.h"

Platform *platform = nullptr;
OpenGL *gl = nullptr;
Game *game = nullptr;

#include "log.cpp"

static bool QUIT_GAME = false;
static WindowContext wc;

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE: {
			DestroyWindow(hwnd);
		} break;
		case WM_DESTROY: {
			QUIT_GAME = true;
			PostQuitMessage(0);
		} break;
		case WM_KEYDOWN: {
			if(game == nullptr) return 0;
			if(lParam & (1<<30)) return 0; // WM_KEYDOWN caused by key autorepeat

			game->input.pressed_keys[(u8)wParam] = 1;
			return 0;
		} break;
		case WM_KEYUP: {
			if(game == nullptr) return 0;

			game->input.released_keys[(u8)wParam] = 1;
			return 0;
		} break;
		case WM_LBUTTONDOWN: {
			if(game == nullptr) return 0;

			game->input.pressed_keys[vk::LMB] = 1;
			return 0;
		} break;
		case WM_LBUTTONUP: {
			if(game == nullptr) return 0;

			game->input.released_keys[vk::LMB] = 1;
			return 0;
		} break;
		case WM_RBUTTONDOWN: {
			if(game == nullptr) return 0;

			game->input.pressed_keys[vk::RMB] = 1;
			return 0;
		} break;
		case WM_RBUTTONUP: {
			if(game == nullptr) return 0;

			game->input.released_keys[vk::RMB] = 1;
			return 0;
		} break;
		case WM_MBUTTONDOWN: {
			if(game == nullptr) return 0;

			game->input.pressed_keys[vk::MMB] = 1;
			return 0;
		} break;
		case WM_MBUTTONUP: {
			if(game == nullptr) return 0;

			game->input.released_keys[vk::MMB] = 1;
			return 0;
		} break;
		case WM_XBUTTONDOWN: {
			if(game == nullptr) return 0;

			if(HIWORD(wParam) & 0x1) game->input.pressed_keys[vk::Mouse4] = 1;
			else if(HIWORD(wParam) & 0x2) game->input.pressed_keys[vk::Mouse5] = 1;
			return 0;
		} break;
		case WM_XBUTTONUP: {
			if(game == nullptr) return 0;

			if(HIWORD(wParam) & 0x1) game->input.released_keys[vk::Mouse4] = 1;
			else if(HIWORD(wParam) & 0x2) game->input.released_keys[vk::Mouse5] = 1;
			return 0;
		} break;
		case WM_CHAR: {
			u32 utf32_code = wParam;
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		} break;
		case WM_INPUTLANGCHANGE: {
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		}
		case WM_INPUTLANGCHANGEREQUEST: {
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		}
		case WM_SETCURSOR: {
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		}
		default: {
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		} break;
	}

	return 0;
}

void
win32_ShowCursor()
{
	ShowCursor(TRUE);
}

void
win32_HideCursor()
{
	ShowCursor(FALSE);
}

void
win32_GetMousePos(int *x, int *y)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(wc.hwnd, &point);

	*x = point.x;
	*y = point.y;
}

void
win32_GetMousePosF(float *x, float *y)
{
	Vec2i mouse_pos_i;
	win32_GetMousePos(&mouse_pos_i.x, &mouse_pos_i.y);
	*x = (float)mouse_pos_i.x;
	*y = (float)mouse_pos_i.y;
}

u32
win32_GetFileSize(const char *filename)
{
	HANDLE file = CreateFile(filename,
							 FILE_READ_DATA, FILE_SHARE_READ,
							 NULL,
							 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
							 NULL);
	if(file == INVALID_HANDLE_VALUE)
	{
		log("Failed to open file to find its size: %s", filename);
		return 0;
	}

	DWORD filesize_high;
	DWORD filesize_low = GetFileSize(file, &filesize_high);
	if(filesize_high != 0) log("FileSizeHigh != 0 (%u)", filesize_high);
	CloseHandle(file);

	return filesize_low;
}

void
win32_WriteLineToFile(char *filename, char *line)
{
	HANDLE file = CreateFile(filename,
							 FILE_APPEND_DATA, FILE_SHARE_READ,
							 NULL,
							 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if(file == INVALID_HANDLE_VALUE) {
		log("Failed to open text file for writing line: %s", filename);
		return;
	}

	WriteFile(file, line, strlen(line), nullptr, NULL);
	char newline = '\n';
	WriteFile(file, &newline, 1, nullptr, NULL);

	CloseHandle(file);
}

bool
win32_LoadFileIntoSizedBufferAndNullTerminate(const char *filename, Buffer *buffer)
{
	buffer->byte_count = win32_GetFileSize(filename);

	HANDLE file = CreateFile(filename,
							 FILE_READ_DATA, FILE_SHARE_READ,
							 NULL,
							 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if(file == INVALID_HANDLE_VALUE)
	{
		log("Failed to open file for reading: %s", filename);
		return false;
	}

	buffer->data = (char*)malloc(buffer->byte_count+1); // +1 for null terminator
	buffer->p = buffer->data;
	ReadFile(file, buffer->data, buffer->byte_count, nullptr, nullptr);
	CloseHandle(file);

	buffer->data[buffer->byte_count] = 0; // null-terminate

	return true;
}

bool
win32_LoadFileIntoFixedBufferAndNullTerminate(const char *filename, u8 *buffer, u32 buffer_length)
{
	HANDLE file = CreateFile(filename,
							 FILE_READ_DATA, FILE_SHARE_READ,
							 NULL,
							 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
							 NULL);

	if(file == INVALID_HANDLE_VALUE)
	{
		log("Failed to open file for reading: %s", filename);
		return false;
	}

	DWORD number_of_bytes_read;
	ReadFile(file, buffer, buffer_length, &number_of_bytes_read, NULL);

	if(number_of_bytes_read > buffer_length-1)
	{
		//log("win32_LoadFileIntoMemoryAndNullTerminate(): Fixed buffer not long enough to recieve null-terminated data");
		buffer[buffer_length-1] = '\0';
		CloseHandle(file);
		return false;
	}
	else
	{
		buffer[number_of_bytes_read] = '\0';
		CloseHandle(file);
		return true;
	}
}

// Utf32String
// win32_LoadUtf8FileAsUtf32String(const char *filename)
// {
// 	u8 *data;
// 	size_t byte_count;
// 	bool success = win32_LoadFileIntoSizedBuffer(filename, &data, &byte_count);

// 	if(!success) return {};

// 	u32 *utf32_chars = (u32*)malloc(4*byte_count);
// 	size_t c = 0; // current index into data[]
// 	size_t c_utf = 0; // current index into utf32_chars

// 	while(c < byte_count)
// 	{
// 		u8 first_byte = data[c];
// 		u32 char_code = 0;

// 		if(u8((first_byte >> 7) | utf8_mask_1byte) == utf8_mask_1byte)
// 		{
// 			char_code = first_byte;
// 			c += 1;
// 		}
// 		else if (u8((first_byte >> 5) | utf8_mask_2byte) == utf8_mask_2byte)
// 		{
// 			char_code += data[c+1] & 0b00111111;
// 			char_code += (data[c+0] & 0b00011111) << 6;
// 			c += 2;
// 		}
// 		else if (u8((first_byte >> 4) | utf8_mask_3byte) == utf8_mask_3byte)
// 		{
// 			char_code += data[c+2] & 0b00111111;
// 			char_code += (data[c+1] & 0b00111111) << 6;
// 			char_code += (data[c+0] & 0b00001111) << 12;
// 			c += 3;
// 		}
// 		else if(u8((first_byte >> 3) | utf8_mask_4byte) == utf8_mask_4byte)
// 		{
// 			char_code += data[c+3] & 0b00111111;
// 			char_code += (data[c+2] & 0b00111111) << 6;
// 			char_code += (data[c+1] & 0b00111111) << 12;
// 			char_code += (data[c+0] & 0b00000111) << 18;
// 			c += 4;
// 		}
// 		else
// 		{
// 			log("Encountered invalid utf-8 starting byte (decimal: %u)", char_code);
// 		}

// 		utf32_chars[c_utf++] = char_code;
// 	}

// 	Utf32String string = {};
// 	string.chars = (u32*)malloc(4*c_utf);
// 	string.char_count = c_utf;
// 	for(int i=0; i<c_utf; i++) {
// 		string.chars[i] = utf32_chars[i];
// 	}

// 	free(data);
// 	free(utf32_chars);

// 	return string;
// }

void CreateWindowAndOpenGlContext(HINSTANCE hInstance, int nCmdShow)
{
	const char *window_class_name = "Lang WC";
	WNDCLASSEX window_class = {};
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_OWNDC;
	window_class.lpfnWndProc = WindowProc;
	window_class.hInstance = hInstance;
	window_class.lpszClassName = window_class_name;
	window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);

	RegisterClassEx(&window_class);

	RECT window_rect = {0,0,c::window_width,c::window_height};
	AdjustWindowRectEx(&window_rect	, WS_OVERLAPPEDWINDOW, FALSE, NULL);
	wc.hwnd = CreateWindowEx(0, window_class_name, "LangL", WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT,
							 CW_USEDEFAULT,
							 window_rect.right - window_rect.left,
							 window_rect.bottom - window_rect.top,
							 NULL, NULL, hInstance, NULL);

	wc.hdc = GetDC(wc.hwnd);
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	int pfd_index = ChoosePixelFormat(wc.hdc, &pfd);
	if(pfd_index == 0) log("ChoosePixelFormat() failed.");
	SetPixelFormat(wc.hdc, pfd_index, &pfd);
	wc.rc = wglCreateContext(wc.hdc);
	wglMakeCurrent(wc.hdc, wc.rc);

	ShowWindow(wc.hwnd, nCmdShow);
	UpdateWindow(wc.hwnd);
}

void WIN32_BIND_PLATFORM_FUNCTIONS(Platform *platform)
{
	#define mBindPlatformFunction(name) platform->name = win32_##name

	mBindPlatformFunction(GetMousePos);
	mBindPlatformFunction(GetFileSize);
	mBindPlatformFunction(WriteLineToFile);
	mBindPlatformFunction(LoadFileIntoFixedBufferAndNullTerminate);
	mBindPlatformFunction(LoadFileIntoSizedBufferAndNullTerminate);
	mBindPlatformFunction(ShowCursor);
	mBindPlatformFunction(HideCursor);

	#undef mBindPlatformFunction
	//platform->LoadUtf8FileAsUtf32String = win32_LoadUtf8FileAsUtf32String;
}

void WIN32_BIND_OPENGL_EXTENSIONS(OpenGL *opengl) {
	#define mBindBaseOpenGLFunction(name) opengl->name = gl##name
	#define mBindExtendedOpenGLFunction(name) opengl->name = (fnsig_gl##name*)wglGetProcAddress("gl"#name)

	mBindBaseOpenGLFunction(GetError);
	mBindExtendedOpenGLFunction(BindBuffer);
	mBindExtendedOpenGLFunction(GenBuffers);
	mBindExtendedOpenGLFunction(DeleteBuffers);
	mBindExtendedOpenGLFunction(BufferData);
	mBindExtendedOpenGLFunction(NamedBufferData);
	mBindExtendedOpenGLFunction(CreateShader);
	mBindExtendedOpenGLFunction(ShaderSource);
	mBindExtendedOpenGLFunction(CompileShader);
	mBindExtendedOpenGLFunction(GetShaderInfoLog);
	mBindExtendedOpenGLFunction(GetShaderiv);
	mBindExtendedOpenGLFunction(CreateProgram);
	mBindExtendedOpenGLFunction(LinkProgram);
	mBindExtendedOpenGLFunction(GetProgramiv);
	mBindExtendedOpenGLFunction(GetProgramInfoLog);
	mBindExtendedOpenGLFunction(AttachShader);
	mBindExtendedOpenGLFunction(UseProgram);
	mBindExtendedOpenGLFunction(GetAttribLocation);
	mBindExtendedOpenGLFunction(VertexAttribPointer);
	mBindExtendedOpenGLFunction(DrawArrays);
	mBindExtendedOpenGLFunction(EnableVertexAttribArray);
	mBindExtendedOpenGLFunction(DisableVertexAttribArray);
	mBindExtendedOpenGLFunction(EnableVertexArrayAttrib);
	mBindExtendedOpenGLFunction(BindFragDataLocation);
	mBindExtendedOpenGLFunction(GetUniformLocation);
	mBindExtendedOpenGLFunction(Uniform1f);
	mBindExtendedOpenGLFunction(Uniform2f);
	mBindExtendedOpenGLFunction(Uniform3f);
	mBindExtendedOpenGLFunction(Uniform4f);
	mBindExtendedOpenGLFunction(Uniform3fv);
	mBindExtendedOpenGLFunction(Uniform4fv);
	mBindExtendedOpenGLFunction(Uniform1ui);
 	mBindExtendedOpenGLFunction(GenTextures);
 	mBindExtendedOpenGLFunction(BindTexture);
 	mBindBaseOpenGLFunction(TexParameteri);
 	mBindBaseOpenGLFunction(TexImage2D);
 	mBindExtendedOpenGLFunction(ActiveTexture);
 	mBindExtendedOpenGLFunction(Uniform1i);
 	mBindBaseOpenGLFunction(GetIntegerv);
 	mBindExtendedOpenGLFunction(IsBuffer);
 	mBindExtendedOpenGLFunction(GenVertexArrays);
 	mBindExtendedOpenGLFunction(BindVertexArray);
 	mBindExtendedOpenGLFunction(DeleteVertexArrays);
 	mBindExtendedOpenGLFunction(DeleteTextures);
	mBindBaseOpenGLFunction(ClearColor);
	mBindBaseOpenGLFunction(Clear);
	mBindBaseOpenGLFunction(Enable);
	mBindBaseOpenGLFunction(Disable);
	mBindExtendedOpenGLFunction(DrawElements);
	mBindExtendedOpenGLFunction(GenFramebuffers);
	mBindExtendedOpenGLFunction(BindFramebuffer);
	mBindExtendedOpenGLFunction(GenRenderbuffers);
	mBindExtendedOpenGLFunction(BindRenderbuffer);
	mBindExtendedOpenGLFunction(RenderbufferStorage);
	mBindExtendedOpenGLFunction(FramebufferRenderbuffer);
	mBindExtendedOpenGLFunction(CheckFramebufferStatus);
	mBindExtendedOpenGLFunction(DeleteShader);
	mBindExtendedOpenGLFunction(BlitFramebuffer);
	mBindExtendedOpenGLFunction(FramebufferTexture2D);
	mBindExtendedOpenGLFunction(DrawBuffers);
	mBindExtendedOpenGLFunction(GetTextureImage);
	mBindExtendedOpenGLFunction(GetTextureSubImage);
	mBindBaseOpenGLFunction(Finish);
	mBindBaseOpenGLFunction(Flush);
	mBindBaseOpenGLFunction(PixelStorei);
	mBindBaseOpenGLFunction(PolygonMode);
	mBindExtendedOpenGLFunction(MapNamedBuffer);
	mBindExtendedOpenGLFunction(UnmapNamedBuffer);

	mBindExtendedOpenGLFunction(ProgramUniform1i);
	mBindExtendedOpenGLFunction(ProgramUniform1iv);
	mBindExtendedOpenGLFunction(ProgramUniform1f);
	mBindExtendedOpenGLFunction(ProgramUniform1fv);
	mBindExtendedOpenGLFunction(ProgramUniform1d);
	mBindExtendedOpenGLFunction(ProgramUniform1dv);
	mBindExtendedOpenGLFunction(ProgramUniform1ui);
	mBindExtendedOpenGLFunction(ProgramUniform1uiv);
	mBindExtendedOpenGLFunction(ProgramUniform2i);
	mBindExtendedOpenGLFunction(ProgramUniform2iv);
	mBindExtendedOpenGLFunction(ProgramUniform2f);
	mBindExtendedOpenGLFunction(ProgramUniform2fv);
	mBindExtendedOpenGLFunction(ProgramUniform2d);
	mBindExtendedOpenGLFunction(ProgramUniform2dv);
	mBindExtendedOpenGLFunction(ProgramUniform2ui);
	mBindExtendedOpenGLFunction(ProgramUniform2uiv);
	mBindExtendedOpenGLFunction(ProgramUniform3i);
	mBindExtendedOpenGLFunction(ProgramUniform3iv);
	mBindExtendedOpenGLFunction(ProgramUniform3f);
	mBindExtendedOpenGLFunction(ProgramUniform3fv);
	mBindExtendedOpenGLFunction(ProgramUniform3d);
	mBindExtendedOpenGLFunction(ProgramUniform3dv);
	mBindExtendedOpenGLFunction(ProgramUniform3ui);
	mBindExtendedOpenGLFunction(ProgramUniform3uiv);
	mBindExtendedOpenGLFunction(ProgramUniform4i);
	mBindExtendedOpenGLFunction(ProgramUniform4iv);
	mBindExtendedOpenGLFunction(ProgramUniform4f);
	mBindExtendedOpenGLFunction(ProgramUniform4fv);
	mBindExtendedOpenGLFunction(ProgramUniform4d);
	mBindExtendedOpenGLFunction(ProgramUniform4dv);
	mBindExtendedOpenGLFunction(ProgramUniform4ui);
	mBindExtendedOpenGLFunction(ProgramUniform4uiv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2x3fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3x2fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2x4fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4x2fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3x4fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4x3fv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2x3dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3x2dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix2x4dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4x2dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix3x4dv);
	mBindExtendedOpenGLFunction(ProgramUniformMatrix4x3dv);

	#undef mBindBaseOpenGLFunction
	#undef mBindExtendedOpenGLFunction

	CheckOpenGLBinds(opengl);
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	platform = new Platform{};
	gl = new OpenGL{};
	game = new Game{};

	WIN32_BIND_PLATFORM_FUNCTIONS(platform);
	start_log();

	CreateWindowAndOpenGlContext(hInstance, nCmdShow);
	WIN32_BIND_OPENGL_EXTENSIONS(gl);

	void (*GameHook)(Platform*, OpenGL*, Game*);
	void (*GameInit)();
	void (*GameUpdateAndRender)();
	HMODULE game_module = LoadLibraryA("game.dll");
	if(game_module == NULL)
	{
		log("Failed to load library game.dll");
	}
	else
	{
		GameHook = (void(*)(Platform*, OpenGL*, Game*))GetProcAddress(game_module, "GameHook");
		GameInit = (void(*)())GetProcAddress(game_module, "GameInit");
		GameUpdateAndRender = (void(*)())GetProcAddress(game_module, "GameUpdateAndRender");
	}

	GameHook(platform, gl, game);
	GameInit();

	MSG msg;
	glClearColor(0.f, 0.f, 0.f, 1.f);

	while(!QUIT_GAME)
	{
		while(PeekMessage(&msg, wc.hwnd, NULL, NULL, PM_REMOVE) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		GetKeyboardState((BYTE*)&game->input.down_keys);
		win32_GetMousePosF(&game->mouse_pos.x, &game->mouse_pos.y);

		GameUpdateAndRender();

		// Clear pressed/released for next frame.
		memset(&game->input.pressed_keys, 0, 256);
		memset(&game->input.released_keys, 0, 256);

		SwapBuffers(wc.hdc);
	}

	FreeLibrary(game_module);

	return 0;
}