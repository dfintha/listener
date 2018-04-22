#include "Window.hpp"
#include "DebugThread.hpp"
#include <map>

constexpr const int EVENT_ATTACH = 10001;
constexpr const int EVENT_DETACH = 10002;

static HWND windowHandle = nullptr;
static HWND labelHandle = nullptr;
static HWND listHandle = nullptr;
static HWND inputHandle = nullptr;
static HWND attachButtonHandle = nullptr;
static HWND detachButtonHandle = nullptr;
static std::map<DWORD, DebugThread *> threads;

static void ResizeEvent(int width, int height) {
    const DWORD listWidth = width - 40;
    const DWORD listHeight = height - 80;
    MoveWindow(listHandle, 20, 60, listWidth, listHeight, TRUE);
}

#include <iostream>

static LRESULT CALLBACK ProcEvent(HWND window, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(windowHandle);
            for (auto& pair : threads)
                delete pair.second;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_GETMINMAXINFO:
            LPMINMAXINFO(l)->ptMinTrackSize.x = 640;
            LPMINMAXINFO(l)->ptMinTrackSize.y = 480;
            break;
        case WM_SIZE:
            ResizeEvent(LOWORD(l), HIWORD(l));
            break;
        case WM_COMMAND:
            {
                const size_t length = GetWindowTextLength(inputHandle);
                CHAR *buffer = new CHAR[length + 1];
                GetWindowTextA(inputHandle, buffer, length + 1);
                DWORD pid = std::atoi(buffer);
                delete[] buffer;

                if (LOWORD(w) == EVENT_ATTACH) {
                    threads[pid] = new DebugThread(pid, listHandle);
                    threads[pid]->Attach();
                } else if (LOWORD(w) == EVENT_DETACH) {
                    threads[pid]->Detach();
                    delete threads[pid];
                    threads.erase(pid);
                }
            }
            break;
        default:
            break;
    }
    return DefWindowProc(window, msg, w, l);
}

bool InitWindow(HINSTANCE instance) {
    if (windowHandle != nullptr)
        return true;

    static const char className[] = "ListenerWindow";
    static const char windowName[] = "Listener";

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = ProcEvent;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = HBRUSH(COLOR_WINDOW);
    windowClass.lpszClassName = className;
    windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (RegisterClassEx(&windowClass) == 0)
        return false;

    const DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    const DWORD windowExStyle = WS_EX_CLIENTEDGE;
    const DWORD windowWidth = 800;
    const DWORD windowHeight = 600;

    windowHandle = CreateWindowEx(windowExStyle, 
                                  className, windowName, 
                                  windowStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  windowWidth, windowHeight,
                                  nullptr, nullptr, instance, nullptr);

    if (windowHandle == nullptr)
        return false;

    const DWORD labelStyle = WS_VISIBLE | WS_CHILD;
    const DWORD labelLeft = 20;
    const DWORD labelTop = 22;
    const DWORD labelWidth = 40;
    const DWORD labelHeight = 20;
    labelHandle = CreateWindow("STATIC", "PID: ", 
                               labelStyle,
                               labelLeft, labelTop,
                               labelWidth, labelHeight,
                               windowHandle, nullptr, instance, nullptr);

    const DWORD listStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
    const DWORD listLeft = 20;
    const DWORD listTop = 60;
    const DWORD listWidth = 740;
    const DWORD listHeight = 470;
    listHandle = CreateWindow("LISTBOX", windowName, 
                              listStyle,
                              listLeft, listTop,
                              listWidth, listHeight,
                              windowHandle, nullptr, instance, nullptr);

    const DWORD inputStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
    const DWORD inputLeft = 60;
    const DWORD inputTop = 20;
    const DWORD inputWidth = 200;
    const DWORD inputHeight = 20;
    inputHandle = CreateWindow("EDIT", "", 
                               inputStyle,
                               inputLeft, inputTop,
                               inputWidth, inputHeight,
                               windowHandle, nullptr, instance, nullptr);

    const DWORD attachButtonStyle = WS_VISIBLE | WS_CHILD;
    const DWORD attachButtonLeft = 270;
    const DWORD attachButtonTop = 20;
    const DWORD attachButtonWidth = 60;
    const DWORD attachButtonHeight = 20;
    attachButtonHandle = CreateWindow("BUTTON", "Attach", 
                                      attachButtonStyle,
                                      attachButtonLeft, attachButtonTop,
                                      attachButtonWidth, attachButtonHeight,
                                      windowHandle, HMENU(EVENT_ATTACH),
                                      instance, nullptr);

    const DWORD detachButtonStyle = WS_VISIBLE | WS_CHILD;
    const DWORD detachButtonLeft = 335;
    const DWORD detachButtonTop = 20;
    const DWORD detachButtonWidth = 60;
    const DWORD detachButtonHeight = 20;
    detachButtonHandle = CreateWindow("BUTTON", "Detach", 
                                      detachButtonStyle,
                                      detachButtonLeft, detachButtonTop,
                                      detachButtonWidth, detachButtonHeight,
                                      windowHandle, HMENU(EVENT_DETACH),
                                      instance, nullptr);

    return true;
}

bool InvokeWindow(int mode) {
    ShowWindow(windowHandle, mode);
    UpdateWindow(windowHandle);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return true;
}
