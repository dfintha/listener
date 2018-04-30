#if defined(LISTENER_WINDOWED)
#include "Window.hpp"
#include "DebugThread.hpp"
#include "Messages.hpp"
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

static LRESULT CALLBACK ProcEvent(HWND window, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_CLOSE:
            for (auto& pair : threads)
                delete pair.second;
            DestroyWindow(windowHandle);
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
                if (pid == 0)
                    break;

                if (LOWORD(w) == EVENT_ATTACH) {
                    threads[pid] = new DebugThread(pid, listHandle);
                    threads[pid]->Attach();
                    SetWindowTextA(inputHandle, "");
                } else if (LOWORD(w) == EVENT_DETACH) {
                    threads[pid]->Detach();
                    delete threads[pid];
                    threads.erase(pid);
                    SetWindowTextA(inputHandle, "");
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

    constexpr DWORD windowStyle = WS_OVERLAPPEDWINDOW;
    constexpr DWORD windowExStyle = WS_EX_CLIENTEDGE;
    constexpr DWORD windowWidth = 800;
    constexpr DWORD windowHeight = 600;

    windowHandle = CreateWindowEx(windowExStyle, 
                                  className, windowName, 
                                  windowStyle,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  windowWidth, windowHeight,
                                  nullptr, nullptr, instance, nullptr);

    if (windowHandle == nullptr)
        return false;

    constexpr DWORD labelStyle = WS_VISIBLE | WS_CHILD;
    constexpr DWORD labelLeft = 20;
    constexpr DWORD labelTop = 22;
    constexpr DWORD labelWidth = 40;
    constexpr DWORD labelHeight = 20;
    labelHandle = CreateWindow("STATIC", PID_LABEL, 
                               labelStyle,
                               labelLeft, labelTop,
                               labelWidth, labelHeight,
                               windowHandle, nullptr, instance, nullptr);

    constexpr DWORD listStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | 
                            WS_HSCROLL | WS_VSCROLL;
    constexpr DWORD listLeft = 20;
    constexpr DWORD listTop = 60;
    constexpr DWORD listWidth = 740;
    constexpr DWORD listHeight = 470;
    listHandle = CreateWindow("LISTBOX", windowName, 
                              listStyle,
                              listLeft, listTop,
                              listWidth, listHeight,
                              windowHandle, nullptr, instance, nullptr);

    constexpr DWORD inputStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
    constexpr DWORD inputLeft = 60;
    constexpr DWORD inputTop = 20;
    constexpr DWORD inputWidth = 200;
    constexpr DWORD inputHeight = 20;
    inputHandle = CreateWindow("EDIT", "", 
                               inputStyle,
                               inputLeft, inputTop,
                               inputWidth, inputHeight,
                               windowHandle, nullptr, instance, nullptr);

    constexpr DWORD attachButtonStyle = WS_VISIBLE | WS_CHILD;
    constexpr DWORD attachButtonLeft = 270;
    constexpr DWORD attachButtonTop = 20;
    constexpr DWORD attachButtonWidth = 60;
    constexpr DWORD attachButtonHeight = 20;
    attachButtonHandle = CreateWindow("BUTTON", BUTTON_ATTACH, 
                                      attachButtonStyle,
                                      attachButtonLeft, attachButtonTop,
                                      attachButtonWidth, attachButtonHeight,
                                      windowHandle, HMENU(EVENT_ATTACH),
                                      instance, nullptr);

    constexpr DWORD detachButtonStyle = WS_VISIBLE | WS_CHILD;
    constexpr DWORD detachButtonLeft = 335;
    constexpr DWORD detachButtonTop = 20;
    constexpr DWORD detachButtonWidth = 60;
    constexpr DWORD detachButtonHeight = 20;
    detachButtonHandle = CreateWindow("BUTTON", BUTTON_DETACH, 
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
#endif
