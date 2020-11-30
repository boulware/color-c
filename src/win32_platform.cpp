#include "win32_platform.h"

#include "meta.h"
#include "macros.h"

#include <windows.h>
#include <GL/GL.h>
#include <cstdint>

#include "types.h"
#include "math.h"

#include "platform.h"
#include "opengl.h"
#include "game.h"

// #include "input.h"
// #include "debug.h"

#include "macros.h"
#include "const.h"
#include "global.h"
#include "debug.h"
#include "win32_work_queue.h"

Platform *platform = nullptr;
OpenGL *gl = nullptr;
Game *game = nullptr;

WorkQueueSystem g_work_queue_system = {};

// #include "log.cpp"
// #include "input.cpp"
// #include "vec.cpp"
// #include "math.cpp"
//#include "game_code_include.cpp"
//#include "text_render.cpp"

#include "platform_include.cpp"
#include "work_entry.h"
#include "ring_buffer.h"

static bool QUIT_GAME = false;
static bool FRAMESTEP = false;
static bool ANYKEY_ACTIVE = false;
static WindowContext wc;
static InputState platform_input;

// A utf-8 encoded bytestream which is generated by the platform with translated characters
// and sent to the game just before frame update begins. Number of meaningful bytes is variable-length,
// and its length is determined by how many translated characters were translated by the platform (in the case of
// Windows, in the WM_CHAR event) since the beginning of the last frame.
// The platform should null terminate it.

// We use two buffers here and just point the game at the one that we'll freeze during the course of frame rendering,
// so we can continue to catch characters and change the stream even while the frame is rendering without affecting
// it.
static u32 utf32_translated_stream0[256] = {};
static u32 utf32_translated_stream1[256] = {};
static u32 *current_translated_stream = utf32_translated_stream0;
static size_t pos_in_translated_stream = 0;

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

            //Log("0x%0*x", 2, (u8)wParam);

            if(lParam & (1<<30))
            { // WM_KEYDOWN caused by key autorepeat
                game->input.repeated_keys[(u8)wParam] = 1;
                platform_input.repeated_keys[(u8)wParam] = 1;
            }
            else
            {
                if(ANYKEY_ACTIVE == true)
                {
                    game->input.any_key = (u8)wParam;
                    ANYKEY_ACTIVE = false;
                }

                // if(lParam & KF_ALTDOWN)
                // {
                //  game->input.pressed_keys[]
                // }

                game->input.pressed_keys[(u8)wParam] = 1;
                platform_input.pressed_keys[(u8)wParam] = 1;
            }

            return 0;
        } break;
        case WM_SYSKEYDOWN:
        {
            if(game == nullptr) return 0;

            //Log("0x%0*x", 2, (u8)wParam);

            if(lParam & (1<<30))
            { // WM_KEYDOWN caused by key autorepeat
                game->input.repeated_keys[(u8)wParam] = 1;
                platform_input.repeated_keys[(u8)wParam] = 1;
            }
            else
            {
                if(ANYKEY_ACTIVE == true)
                {
                    game->input.any_key = (u8)wParam;
                    ANYKEY_ACTIVE = false;
                }

                game->input.pressed_keys[(u8)wParam] = 1;
                platform_input.pressed_keys[(u8)wParam] = 1;
            }

            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
        case WM_KEYUP: {
            if(game == nullptr) return 0;

            game->input.released_keys[(u8)wParam] = 1;
            platform_input.released_keys[(u8)wParam] = 1;
            return 0;
        } break;
        case WM_LBUTTONDOWN: {
            if(game == nullptr) return 0;

            game->input.pressed_keys[vk::LMB] = 1;
            platform_input.pressed_keys[vk::LMB] = 1;
            return 0;
        } break;
        case WM_LBUTTONUP: {
            if(game == nullptr) return 0;

            game->input.released_keys[vk::LMB] = 1;
            platform_input.released_keys[vk::LMB] = 1;
            return 0;
        } break;
        case WM_RBUTTONDOWN: {
            if(game == nullptr) return 0;

            game->input.pressed_keys[vk::RMB] = 1;
            platform_input.pressed_keys[vk::RMB] = 1;
            return 0;
        } break;
        case WM_RBUTTONUP: {
            if(game == nullptr) return 0;

            game->input.released_keys[vk::RMB] = 1;
            platform_input.released_keys[vk::RMB] = 1;
            return 0;
        } break;
        case WM_MBUTTONDOWN: {
            if(game == nullptr) return 0;

            game->input.pressed_keys[vk::MMB] = 1;
            platform_input.pressed_keys[vk::MMB] = 1;
            return 0;
        } break;
        case WM_MBUTTONUP: {
            if(game == nullptr) return 0;

            game->input.released_keys[vk::MMB] = 1;
            platform_input.released_keys[vk::MMB] = 1;
            return 0;
        } break;
        case WM_XBUTTONDOWN: {
            if(game == nullptr) return 0;

            if(HIWORD(wParam) & 0x1)
            {
                game->input.pressed_keys[vk::Mouse4] = 1;
                platform_input.pressed_keys[vk::Mouse4] = 1;
            }
            else if(HIWORD(wParam) & 0x2)
            {
                game->input.pressed_keys[vk::Mouse5] = 1;
                platform_input.pressed_keys[vk::Mouse5] = 1;
            }
            return 0;
        } break;
        case WM_XBUTTONUP: {
            if(game == nullptr) return 0;

            if(HIWORD(wParam) & 0x1)
            {
                game->input.released_keys[vk::Mouse4] = 1;
                platform_input.released_keys[vk::Mouse4] = 1;
            }
            else if(HIWORD(wParam) & 0x2)
            {
                game->input.released_keys[vk::Mouse5] = 1;
                platform_input.released_keys[vk::Mouse5] = 1;
            }
            return 0;
        } break;
        case WM_MOUSEWHEEL: {
            game->input.mouse_scroll += GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
            platform_input.mouse_scroll += GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
            return 0;
        };
        case WM_CHAR: {
            //Log("%u", wParam);
            if(pos_in_translated_stream < ArrayCount(utf32_translated_stream0))
            {
                current_translated_stream[pos_in_translated_stream++] = wParam;
            }
            else
            {
                Log("utf32-translated stream in win32 platform layer reached maximum size.");
            }
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


s64 performance_frequency;

s64
win32_PerformanceFrequencyAsS64()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

s64
win32_PerformanceCounterFrequency()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

s64
win32_CurrentTime()
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return count.QuadPart;
}

u32
win32_GetCallingThreadId()
{
    return GetCurrentThreadId();
}

float
win32_TimeElapsedMs(s64 start, s64 end)
{
    return (float)(end-start)/performance_frequency*1000.f;
}

void *
win32_AllocateMemory(size_t size)
{
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

// Returns true on success, false otherwise.
bool
win32_FreeMemory(void *p)
{
    return VirtualFree(p, 0, MEM_RELEASE);
}

void
win32_BlockAndTakeMutex(void *mutex_handle)
{
    HANDLE win32_mutex_handle = (HANDLE)mutex_handle;
    WaitForSingleObjectEx(win32_mutex_handle, INFINITE, FALSE);
}

void
win32_ReleaseMutex(void *mutex_handle)
{
    HANDLE win32_mutex_handle = (HANDLE)mutex_handle;
    ReleaseMutex(win32_mutex_handle);
}

void
win32_ReadWriteBarrier()
{
    _ReadWriteBarrier();
    _mm_mfence();
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

bool
win32_GetFileSize(const char *filename, size_t *size_in_bytes)
{
    HANDLE file = CreateFile(filename,
                             FILE_READ_DATA,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    if(file == INVALID_HANDLE_VALUE)
    {
        Log("Failed to open file to find its size: %s", filename);
        return false;
    }

    DWORD filesize_high;
    DWORD filesize_low = GetFileSize(file, &filesize_high);
    if(filesize_high != 0) Log("FileSizeHigh != 0 (%u)", filesize_high);
    CloseHandle(file);

    *size_in_bytes = filesize_low;
    return true;
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
        win32_WriteLineToFile("logs/critical.log", "Failed to open text file for writing line.");
        return;
    }

    DWORD byte_count_written;
    WriteFile(file, line, strlen(line), &byte_count_written, NULL);
    char newline = '\n';
    WriteFile(file, &newline, 1, nullptr, NULL);

    CloseHandle(file);
}

bool
win32_LoadFileIntoSizedBufferAndNullTerminate(const char *filename, Buffer *buffer)
{
    if(!win32_GetFileSize(filename, &buffer->byte_count))
    {
        return false;
    }

    HANDLE file = CreateFile(filename,
                             GENERIC_READ,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    if(file == INVALID_HANDLE_VALUE)
    {
        Log("Failed to open file for reading: %s", filename);
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
                             FILE_READ_DATA,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    if(file == INVALID_HANDLE_VALUE)
    {
        Log("Failed to open file for reading: %s", filename);
        return false;
    }

    DWORD number_of_bytes_read;
    ReadFile(file, buffer, buffer_length, &number_of_bytes_read, NULL);

    if(number_of_bytes_read > buffer_length-1)
    {
        //Log("win32_LoadFileIntoMemoryAndNullTerminate(): Fixed buffer not long enough to recieve null-terminated data");
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



void
win32_AnyKey()
{
    game->input.any_key = 0;
    ANYKEY_ACTIVE = true;
}

void
WIN32_DELETE_OPENGL_WINDOW(HWND hwnd)
{
    //HGLRC hRc = wglGetCurrentContext();
    wglDeleteContext(wc.rc);
    DestroyWindow(wc.hwnd);
    QUIT_GAME = true;
}

void
win32_ExitGame()
{
    WIN32_DELETE_OPENGL_WINDOW(wc.hwnd);
}

int
win32_MyInterlockedIncrement(int volatile *v)
{
    return InterlockedIncrement((LONG volatile *)v);
}

void CreateWindowAndOpenGlContext(HINSTANCE hInstance, int nCmdShow)
{
    const char *window_class_name = "Trait WC";
    WNDCLASSEX window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_OWNDC;
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = window_class_name;
    window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);

    RegisterClassEx(&window_class);

    RECT window_rect = {};
    DWORD window_style = 0;
    if(c::fullscreen)
    {
        window_style = WS_POPUP;
        game->window_size = {(float)c::fullscreen_window_width, (float)c::fullscreen_window_height};
        window_rect = {0,0,c::fullscreen_window_width,c::fullscreen_window_height};
    }
    else
    {
        window_style = WS_OVERLAPPEDWINDOW;
        game->window_size = {(float)c::windowed_window_width, (float)c::windowed_window_height};
        window_rect = {0,0,c::windowed_window_width,c::windowed_window_height};
    }

    AdjustWindowRectEx(&window_rect, window_style, FALSE, NULL);
    wc.hwnd = CreateWindowEx(0,
                             window_class_name,
                             "Trait",
                             window_style,
                             0,
                             0,
                             window_rect.right - window_rect.left,
                             window_rect.bottom - window_rect.top,
                             NULL, NULL, hInstance, NULL);

    RECT test;
    GetWindowRect(wc.hwnd, &test);

    wc.hdc = GetDC(wc.hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    int pfd_index = ChoosePixelFormat(wc.hdc, &pfd);
    if(pfd_index == 0) Log("ChoosePixelFormat() failed.");
    SetPixelFormat(wc.hdc, pfd_index, &pfd);
    DescribePixelFormat(wc.hdc, pfd_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
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
    //mBindPlatformFunction(ExitGame);
    mBindPlatformFunction(AllocateMemory);
    mBindPlatformFunction(PerformanceCounterFrequency);
    mBindPlatformFunction(AnyKey);
    //mBindPlatformFunction(StartJob);
    mBindPlatformFunction(GetCallingThreadId);
    mBindPlatformFunction(MyInterlockedIncrement);
    mBindPlatformFunction(CreateWorkQueue);
    mBindPlatformFunction(AddWorkEntry);
    mBindPlatformFunction(QueueReleaseSemaphore);
    mBindPlatformFunction(FreeMemory);
    mBindPlatformFunction(WorkQueuePendingJobCount);

    mBindPlatformFunction(BlockAndTakeMutex);
    mBindPlatformFunction(ReleaseMutex);
    mBindPlatformFunction(ReadWriteBarrier);

    platform->SwapIntervalEXT = (fnsig_SwapIntervalEXT*)wglGetProcAddress("wglSwapIntervalEXT");

    #undef mBindPlatformFunction
}

void WIN32_BIND_OPENGL_EXTENSIONS(OpenGL *opengl) {
    #define mBindBaseOpenGLFunction(name) opengl->name = gl##name
    #define mBindExtendedOpenGLFunction(name) opengl->name = (fnsig_gl##name*)wglGetProcAddress("gl"#name)

    mBindExtendedOpenGLFunction(GetInteger64v);
    mBindExtendedOpenGLFunction(GetInteger64i_v);

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
    mBindBaseOpenGLFunction(GetString);
    mBindExtendedOpenGLFunction(ReadnPixels);
    mBindExtendedOpenGLFunction(MemoryBarrier);
    mBindExtendedOpenGLFunction(DebugMessageCallback);

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

    mBindExtendedOpenGLFunction(BlendEquation);
    mBindBaseOpenGLFunction(BlendFunc);

    // Compute shader
    mBindExtendedOpenGLFunction(DispatchCompute);
    mBindExtendedOpenGLFunction(DispatchComputeIndirect);
    mBindExtendedOpenGLFunction(BindImageTexture);

    mBindExtendedOpenGLFunction(GetTextureImage);
    mBindExtendedOpenGLFunction(GetIntegeri_v);
    mBindExtendedOpenGLFunction(NamedFramebufferTexture);
    mBindExtendedOpenGLFunction(FramebufferTexture);
    mBindExtendedOpenGLFunction(BindTextureUnit);

    // Query
    mBindExtendedOpenGLFunction(GenQueries);
    mBindExtendedOpenGLFunction(DeleteQueries);
    mBindExtendedOpenGLFunction(IsQuery);
    mBindExtendedOpenGLFunction(BeginQuery);
    mBindExtendedOpenGLFunction(EndQuery);
    mBindExtendedOpenGLFunction(GetQueryiv);
    mBindExtendedOpenGLFunction(GetQueryObjectiv);
    mBindExtendedOpenGLFunction(GetQueryObjectuiv);
    mBindExtendedOpenGLFunction(CreateQueries);
    mBindExtendedOpenGLFunction(GetQueryBufferObjecti64v);
    mBindExtendedOpenGLFunction(GetQueryBufferObjectiv);
    mBindExtendedOpenGLFunction(GetQueryBufferObjectui64v);
    mBindExtendedOpenGLFunction(GetQueryBufferObjectuiv);
    mBindExtendedOpenGLFunction(QueryCounter);
    mBindExtendedOpenGLFunction(GetQueryObjecti64v);
    mBindExtendedOpenGLFunction(GetQueryObjectui64v);

    // Stencil buffer
    mBindBaseOpenGLFunction(StencilMask);
    mBindBaseOpenGLFunction(StencilFunc);
    mBindBaseOpenGLFunction(StencilOp);

    //
    mBindBaseOpenGLFunction(DepthFunc);

    #undef mBindBaseOpenGLFunction
    #undef mBindExtendedOpenGLFunction

    CheckOpenGLBinds(opengl);
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    //TIMED_BLOCK;
    debug::timed_block_array_size = __COUNTER__;

    performance_frequency = win32_PerformanceFrequencyAsS64();

    platform = new Platform{};
    gl = new OpenGL{};
    game = new Game{};

    WIN32_BIND_PLATFORM_FUNCTIONS(platform);

    volatile Pool<Arena> arena_pool = AllocPool<Arena>(100);
    game->arena_pool = &arena_pool;

    HANDLE arena_pool_mutex = CreateMutex(NULL, FALSE, NULL);
    memory::default_arena_id   = AllocArena("Default");
    memory::per_frame_arena_id = AllocArena("Per Frame");
    memory::permanent_arena_id = AllocArena("Permanent");

    GameInitData game_init_data = {
        .per_frame_arena_id = memory::per_frame_arena_id,
        .permanent_arena_id = memory::permanent_arena_id,
        .arena_pool_mutex_handle  = (void *)arena_pool_mutex,
    };

    g_work_queue_system.queues = CreatePermanentArray<WorkQueue>(8);
    g_work_queue_system.init   = true;

    // ----------------------------------------------------------------------------
    // Barrier for creating work queues. Below this point, it should be good to go.
    // ----------------------------------------------------------------------------

    InitLog(&game->log_state);
    LogToFile("logs/critical.log", "-----------------");

    CreateWindowAndOpenGlContext(hInstance, nCmdShow);
    WIN32_BIND_OPENGL_EXTENSIONS(gl);

    void (*GameHook)(Platform*, OpenGL*, Game*);
    void (*GameInit)(GameInitData);
    void (*GameUpdateAndRender)();
    HMODULE game_module = LoadLibraryA("game.dll");
    if(game_module == NULL)
    {
        Log("Failed to load library game.dll");
    }
    else
    {
        GameHook = (void(*)(Platform*, OpenGL*, Game*))GetProcAddress(game_module, "GameHook");
        GameInit = (void(*)(GameInitData))GetProcAddress(game_module, "GameInit");
        GameUpdateAndRender = (void(*)())GetProcAddress(game_module, "GameUpdateAndRender");
    }

    GameHook(platform, gl, game);

    //s64 start_time = win32_CurrentTime();
    GameInit(game_init_data);
    //Log("GameInit() time: %fms", win32_TimeElapsedMs(start_time, win32_CurrentTime()));

    MSG msg;
    gl->Enable(GL_DEPTH_TEST);
    gl->DepthFunc(GL_LEQUAL);
    gl->ClearColor(0.f, 0.f, 0.f, 1.f);

    //platform->SwapIntervalEXT(0);
    input::global_input = &game->input;

    FT_Library ft_lib;
    if(InitFreetype(&ft_lib))
    {
        FT_Face roboto_face = LoadFontFaceFromFile("resource/Roboto-Bold.ttf", ft_lib);
        text_render::default_font = LoadFontData(roboto_face, 64);
        CloseFreetype(&ft_lib);
    }

    FrametimeGraphState cpu_frametime_graph_state;
    InitFrametimeGraphState(&cpu_frametime_graph_state, c::number_of_frametimes);
    game->cpu_frametime_graph_state = &cpu_frametime_graph_state;

    FrametimeGraphState gpu_frametime_graph_state;
    InitFrametimeGraphState(&gpu_frametime_graph_state, c::number_of_frametimes);
    game->gpu_frametime_graph_state = &gpu_frametime_graph_state;

    GLuint gpu_start_counter, gpu_end_counter;
    gl->CreateQueries(GL_TIMESTAMP, 1, &gpu_start_counter);
    gl->CreateQueries(GL_TIMESTAMP, 1, &gpu_end_counter);

    while(!QUIT_GAME)
    {
        while(PeekMessage(&msg, wc.hwnd, NULL, NULL, PM_REMOVE) != 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(Pressed(vk::F9))
        {
            FRAMESTEP = !FRAMESTEP;
        }


        if(!FRAMESTEP or Pressed(vk::F5) or Repeated(vk::F5))
        {
            s64 before_update = win32_CurrentTime();

            // START update
            gl->QueryCounter(gpu_start_counter, GL_TIMESTAMP);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 //    HANDLE mutex_handle;

    // volatile u32 next_entry_to_read;
    // volatile u32 next_entry_to_write;
    // HANDLE semaphore_handle;

    // volatile RingBuffer entry_data_buffer;
    // WorkEntry entries[1024];

            #if 0
            if(g_work_queue_system.init)
            {
                Vec2f pen = {};
                for(auto &queue : g_work_queue_system.queues)
                {
                    Rect text_rect = DrawText(c::def_text_layout, pen, "%s: %zu running, %zu completed",
                                              queue.name,
                                              win32_WorkQueuePendingJobCount(&queue),
                                              queue.job_completion_count);

                    pen.y += text_rect.size.y;
                }
            }
            #endif
            //DrawText(c::def_text_layout, {0.f, 100.f}, "next_entry_to_read: %u", game->log_state.queue->next_entry_to_read);

            GetKeyboardState((BYTE*)&game->input.down_keys);
            game->input.prev_mouse_pos = game->input.mouse_pos;
            win32_GetMousePosF(&game->input.mouse_pos.x, &game->input.mouse_pos.y);
            game->input.mouse_focus_taken = false;

            // Null-terminate the translated stream and give the game input its address.
            current_translated_stream[pos_in_translated_stream] = 0;
            game->input.utf32_translated_stream = current_translated_stream;
            pos_in_translated_stream = 0;

            // Swap to the other stream in the platform layer, so we don't change the
            // translated stream while the frame is being updated.
            if(current_translated_stream == utf32_translated_stream0)
            {
                current_translated_stream = utf32_translated_stream1;
            }
            else if(current_translated_stream == utf32_translated_stream1)
            {
                current_translated_stream = utf32_translated_stream0;
            }
            else
            {
                Assert(false);
            }

            GameUpdateAndRender();

            // Clear pressed/released for next frame.
            // memset(&game->input.pressed_keys, 0, 256);
            // memset(&game->input.released_keys, 0, 256);
            // memset(&game->input.repeated_keys, 0, 256);
            // game->input.mouse_scroll = 0;

            // END update

            s64 after_update = win32_CurrentTime();
            float this_frame_time_ms = win32_TimeElapsedMs(before_update, after_update);
            game->frame_time_ms = 0.5f*(this_frame_time_ms + game->frame_time_ms);
            AddFrametimeToGraph(&cpu_frametime_graph_state, this_frame_time_ms);
            ResetInputState(&game->input);

            // Process GPU frametime query
            gl->QueryCounter(gpu_end_counter, GL_TIMESTAMP);
            u64 gpu_start_time, gpu_end_time;
            gl->GetQueryObjectui64v(gpu_start_counter, GL_QUERY_RESULT, &gpu_start_time);
            gl->GetQueryObjectui64v(gpu_end_counter, GL_QUERY_RESULT, &gpu_end_time);
            float gpu_time_ms = (float)(gpu_end_time - gpu_start_time) / 1000000.f;
            AddFrametimeToGraph(&gpu_frametime_graph_state, gpu_time_ms);

            Vec2f pos = {game->window_size.x,0.f};
            TextLayout frametime_layout = c::def_text_layout;
            frametime_layout.align = c::align_topright;
            auto cam = PushUiCamera();
            pos.y += DrawText(frametime_layout, pos, "cpu: %.3fms",
                              game->frame_time_ms).rect.size.y;
            DrawText(frametime_layout, pos, "gpu: %.3fms", gpu_time_ms);
            PopUiCamera(cam);

            SwapBuffers(wc.hdc);
            RedrawWindow(wc.hwnd, NULL, NULL, RDW_INVALIDATE);
        }

        if(game->exit_requested) QUIT_GAME = true;

        ResetInputState(&platform_input);
    }

    win32_ExitGame();
    FreeLibrary(game_module);

    ExitProcess(0);
}

TimedBlockEntry TIMED_BLOCK_ARRAY[__COUNTER__-1];