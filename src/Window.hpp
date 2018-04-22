#if defined(LISTENER_WINDOWED) && !defined(LISTENER_WINDOW)
#define LISTENER_WINDOW
#pragma once

#include <Windows.h>

bool InitWindow(HINSTANCE instance);
bool InvokeWindow(int mode);

#endif
