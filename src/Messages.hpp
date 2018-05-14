#if !defined(LISTENER_MESSAGES)
#define LISTENER_MESSAGES
#pragma once

constexpr const wchar_t UNKNOWN_PATH[] =
    L"(Unknown)";

constexpr const wchar_t FATAL_EXCEPTION[] =
    L" (Fatal)";

constexpr const wchar_t MESSAGE_ATTACHED[] =
    L"Attached to Process";

constexpr const wchar_t MESSAGE_DETACHED[] =
    L"Detached from Process";

constexpr const wchar_t MESSAGE_EXITED[] =
    L"Process Exited";

#if defined(LISTENER_WINDOWED)

constexpr const char BUTTON_ATTACH[] = 
    "Attach";

constexpr const char BUTTON_DETACH[] = 
    "Detach";

constexpr const char PID_LABEL[] = 
    "PID: ";

#endif

#endif
