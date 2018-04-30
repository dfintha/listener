#if !defined(LISTENER_DEBUGTHREAD)
#define LISTENER_DEBUGTHREAD
#pragma once

#include <Windows.h>
#include <thread>

class DebugThread {
public:
    DebugThread();
    DebugThread(DWORD processId, HWND output);
    ~DebugThread() noexcept;

    DebugThread(const DebugThread&) = delete;
    DebugThread(DebugThread&&) = delete;
    DebugThread& operator=(const DebugThread&) = delete;
    DebugThread& operator=(DebugThread&&) = delete;

    void Attach();
    void Detach();
    void Print(std::wstring output) const;

    inline DWORD GetPID() const {
        return pid;
    }

private:
    DWORD pid;
    HWND textOutput;
    bool isDetaching;
    std::thread workThread;
    std::wstring fileName;
};

#endif
