#include "DebugThread.hpp"
#include "Messages.hpp"
#include <psapi.h>
#include <iostream>
#include <mutex>
#include <sstream>

#if !defined(LISTENER_WINDOWED)
static std::mutex streamMutex;
#endif

static void ProcessDebugStringEvent(HANDLE processHandle,
                                    OUTPUT_DEBUG_STRING_INFO& info,
                                    std::wstringstream& sstream) {
    const BOOL unicode = info.fUnicode != 0;
    const SIZE_T unit = unicode ? sizeof(WCHAR) : sizeof(CHAR);
    const SIZE_T len = info.nDebugStringLength * unit;
    SIZE_T r;

    if (unicode) {
        WCHAR *buf = new WCHAR[info.nDebugStringLength];
        ReadProcessMemory(processHandle, info.lpDebugStringData, buf, len, &r);
        sstream << buf;
        delete[] buf;
    } else {
        CHAR *buf = new CHAR[info.nDebugStringLength];
        ReadProcessMemory(processHandle, info.lpDebugStringData, buf, len, &r);
        sstream << buf;
        delete[] buf;
    }
}

static void ProcessExceptionEvent(EXCEPTION_DEBUG_INFO& info,
                                  std::wstringstream& sstream) {
    const size_t addr = size_t(info.ExceptionRecord.ExceptionAddress);

    switch (info.ExceptionRecord.ExceptionCode) {
        #define HandleExceptionCode(CODE) \
            case CODE: sstream << CODE##_DETAIL; break
        HandleExceptionCode(EXCEPTION_ACCESS_VIOLATION);
        HandleExceptionCode(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
        HandleExceptionCode(EXCEPTION_BREAKPOINT);
        HandleExceptionCode(EXCEPTION_DATATYPE_MISALIGNMENT);
        HandleExceptionCode(EXCEPTION_FLT_DENORMAL_OPERAND);
        HandleExceptionCode(EXCEPTION_FLT_DIVIDE_BY_ZERO);
        HandleExceptionCode(EXCEPTION_FLT_INEXACT_RESULT);
        HandleExceptionCode(EXCEPTION_FLT_INVALID_OPERATION);
        HandleExceptionCode(EXCEPTION_FLT_OVERFLOW);
        HandleExceptionCode(EXCEPTION_FLT_STACK_CHECK);
        HandleExceptionCode(EXCEPTION_FLT_UNDERFLOW);
        HandleExceptionCode(EXCEPTION_ILLEGAL_INSTRUCTION);
        HandleExceptionCode(EXCEPTION_IN_PAGE_ERROR);
        HandleExceptionCode(EXCEPTION_INT_DIVIDE_BY_ZERO);
        HandleExceptionCode(EXCEPTION_INT_OVERFLOW);
        HandleExceptionCode(EXCEPTION_INVALID_DISPOSITION);
        HandleExceptionCode(EXCEPTION_NONCONTINUABLE_EXCEPTION);
        HandleExceptionCode(EXCEPTION_PRIV_INSTRUCTION);
        HandleExceptionCode(EXCEPTION_SINGLE_STEP);
        HandleExceptionCode(EXCEPTION_STACK_OVERFLOW);
        #undef HandleExceptionCode

        default:
            sstream << EXCEPTION_UNKNOWN_DETAIL;
            break;
    }
    sstream << " (0x" << std::hex << addr << std::dec << "). ";
}

constexpr WORD colFatal = FOREGROUND_INTENSITY | FOREGROUND_RED;
constexpr WORD colStartStop = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
constexpr WORD colException = colStartStop | colFatal;
constexpr WORD colString = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

static void SetColor(WORD colorAttrs) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorAttrs);
}

DebugThread::DebugThread() :
    pid(0),
    textOutput(nullptr),
    isDetaching(true),
    workThread()
{ }

DebugThread::DebugThread(DWORD processId, HWND output) :
    pid(processId),
    textOutput(output),
    isDetaching(false),
    workThread()
{ }

DebugThread::~DebugThread() noexcept {
    if (!isDetaching)
        Detach();
}

void DebugThread::Attach() {
    workThread = std::thread{[this]() {
        HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
        if (process == nullptr)
            return;

        if (!DebugActiveProcess(pid)) {
            CloseHandle(process);
            return;
        }

        {
            WCHAR path[MAX_PATH];
            if (!GetModuleFileNameExW(process, nullptr, path, MAX_PATH)) {
                fileName = UNKNOWN_PATH;
            } else {
                fileName = path;
                const size_t where = fileName.rfind(L'\\');
                if (where != std::wstring::npos)
                    fileName = fileName.substr(where + 1);
            }
        }

        std::wstringstream ss;
        ss << fileName << " [" << pid << "]: " << MESSAGE_ATTACHED;
        SetColor(colStartStop);
        Print(ss.str());

        DEBUG_EVENT event;
        DWORD contFlag = DBG_CONTINUE;
        while (!isDetaching) {
            if (!WaitForDebugEvent(&event, 500))
                continue;

            contFlag = DBG_CONTINUE;
            switch (event.dwDebugEventCode) {
                case OUTPUT_DEBUG_STRING_EVENT: 
                {
                    ss.str(L"");
                    ss << fileName << " [" << pid << "]: ";
                    ProcessDebugStringEvent(process, event.u.DebugString, ss);
                    SetColor(colString);
                    Print(ss.str());
                    break;
                }

                case EXCEPTION_DEBUG_EVENT:
                {
                    ss.str(L"");
                    ss << fileName << " [" << pid << "]: ";
                    ProcessExceptionEvent(event.u.Exception, ss);
                    SetColor(colException);
                    const auto& record = event.u.Exception.ExceptionRecord;
                    const auto& flags = record.ExceptionFlags;
                    if ((flags & EXCEPTION_NONCONTINUABLE) != 0) {
                        SetColor(colFatal);
                        ss << FATAL_EXCEPTION;
                        contFlag = DBG_EXCEPTION_NOT_HANDLED;
                    }
                    Print(ss.str());
                    break;
                }

                case EXIT_PROCESS_DEBUG_EVENT:
                {
                    ss.str(L"");
                    ss << fileName << " [" << pid << "]: " << MESSAGE_EXITED;
                    SetColor(colStartStop);
                    Print(ss.str());
                    this->isDetaching = true;
                }

                default:
                    break;
            }

            ContinueDebugEvent(event.dwProcessId, event.dwThreadId, contFlag);
        }

        CloseHandle(process);
        DebugActiveProcessStop(pid);
    }};
}

void DebugThread::Detach() {
    if (isDetaching)
        return;
    
    isDetaching = true;
    workThread.join();

    std::wstringstream ss;
    ss << fileName << " [" << pid << "]: " << MESSAGE_DETACHED;
    SetColor(colStartStop);
    Print(ss.str());
}

void DebugThread::Print(std::wstring output) const {
#if !defined(LISTENER_WINDOWED)
    std::lock_guard<std::mutex> streamGuard(streamMutex);
    std::wcout << output;
    if (output[output.length() - 1] != L'\n')
        std::wcout << L'\n';
#else
    SendMessageW(textOutput, LB_ADDSTRING, 0, LPARAM(output.data()));
    const DWORD count = SendMessageW(textOutput, LB_GETCOUNT, 0, 0);
    SendMessageW(textOutput, LB_SETCURSEL, count - 1, 0);
#endif
}
