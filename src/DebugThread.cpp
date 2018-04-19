#include "DebugThread.hpp"
#include <psapi.h>
#include <iostream>
#include <mutex>
#include <sstream>

static std::mutex streamMutex;

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
			case EXCEPTION_ACCESS_VIOLATION:
				sstream << "EXCEPTION_ACCESS_VIOLATION: ";
				sstream << "The thread tried to read from or write to a "
						   "virtual address for which it does not have the "
						   "appropriate access.";
				break;

			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				sstream << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED: ";
				sstream << "The thread tried to access an array element that "
						   "is out of bounds and the underlying hardware "
						   "supports bounds checking.";
				break;

			case EXCEPTION_BREAKPOINT:
				sstream << "EXCEPTION_BREAKPOINT: ";
				sstream << "A breakpoint was encountered.";
				break;

			case EXCEPTION_DATATYPE_MISALIGNMENT:
				sstream << "EXCEPTION_DATATYPE_MISALIGNMENT: ";
				sstream << "The thread tried to read or write data that is "
						   "misaligned on hardware that does not provide "
						   "alignment.";
				break;

			case EXCEPTION_FLT_DENORMAL_OPERAND:
				sstream << "EXCEPTION_FLT_DENORMAL_OPERAND: ";
				sstream << "One of the operands in a floating-point operation "
						   "is denormal.";
				break;

			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				sstream << "EXCEPTION_FLT_DIVIDE_BY_ZERO: ";
				sstream << "The thread tried to divide a floating-point "
						   "value by a floating-point divisor of zero.";
				break;

			case EXCEPTION_FLT_INEXACT_RESULT:
				sstream << "EXCEPTION_FLT_INEXACT_RESULT: ";
				sstream << "The result of a floating-point operation cannot "
						   "be represented exactly as a decimal fraction.";
				break;

			case EXCEPTION_FLT_INVALID_OPERATION:
				sstream << "EXCEPTION_FLT_INVALID_OPERATION: ";
				sstream << "This exception represents any floating-point "
						   "exception not included in this list.";
				break;

			case EXCEPTION_FLT_OVERFLOW:
				sstream << "EXCEPTION_FLT_OVERFLOW: ";
				sstream << "The exponent of a floating-point operation is "
						   "greater than the magnitude allowed by the "
						   "corresponding type.";
				break;

			case EXCEPTION_FLT_STACK_CHECK:
				sstream << "EXCEPTION_FLT_STACK_CHECK: ";
				sstream << "The stack overflowed or underflowed as the result "
						   "of a floating-point operation.";
				break;

			case EXCEPTION_FLT_UNDERFLOW:
				sstream << "EXCEPTION_FLT_UNDERFLOW: ";
				sstream << "The exponent of a floating-point operation is "
						   "less than the magnitude allowed by the "
						   "corresponding type.";
				break;

			case EXCEPTION_ILLEGAL_INSTRUCTION:
				sstream << "EXCEPTION_ILLEGAL_INSTRUCTION: ";
				sstream << "The thread tried to execute an invalid "
						   "instruction.";
				break;

			case EXCEPTION_IN_PAGE_ERROR:
				sstream << "EXCEPTION_IN_PAGE_ERROR: ";
				sstream << "The thread tried to access a page that was not "
						   "present, and the system was unable to load the "
						   "page.";
				break;

			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				sstream << "EXCEPTION_INT_DIVIDE_BY_ZERO: ";
				sstream << "The thread tried to divide an integer value by "
						   "an integer divisor of zero.";
				break;

			case EXCEPTION_INT_OVERFLOW:
				sstream << "EXCEPTION_INT_OVERFLOW: ";
				sstream << "The result of an integer operation caused a "
						   "carry out of the most significant bit of the "
						   "result.";
				break;

			case EXCEPTION_INVALID_DISPOSITION:
				sstream << "EXCEPTION_INVALID_DISPOSITION: ";
				sstream << "An exception handler returned an invalid "
						   "disposition to the exception dispatcher. ";
				break;

			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				sstream << "EXCEPTION_NONCONTINUABLE_EXCEPTION: ";
				sstream << "The thread tried to continue execution after a "
						   "noncontinuable exception occurred.";
				break;

			case EXCEPTION_PRIV_INSTRUCTION:
				sstream << "EXCEPTION_PRIV_INSTRUCTION: ";
				sstream << "The thread tried to execute an instruction whose "
						   "operation is not allowed in the current machine "
						   "mode.";
				break;

			case EXCEPTION_SINGLE_STEP:
				sstream << "EXCEPTION_SINGLE_STEP: ";
				sstream << "A trace trap or other single-instruction mechanism "
						   "signaled that one instruction has been executed.";
				break;

			case EXCEPTION_STACK_OVERFLOW:
				sstream << "EXCEPTION_STACK_OVERFLOW: ";
				sstream << "The thread used up its stack.";
				break;

			default:
				sstream << "EXCEPTION_UNKNOWN: ";
				sstream << "An unknown exception occurred.";
				break;
	}

	sstream << " (0x" << std::hex << addr << "). ";
}

constexpr WORD colFatal = FOREGROUND_INTENSITY | FOREGROUND_RED;
constexpr WORD colStartStop = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
constexpr WORD colException = colStartStop | colFatal;
constexpr WORD colString = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

static void SetAttribute(WORD colorAttrs) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorAttrs);
}

DebugThread::DebugThread() :
	processID(0),
	textOutput(nullptr),
	isDetaching(true),
	workThread()
{ }

DebugThread::DebugThread(DWORD pid, HWND output) :
	processID(pid),
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
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processID);
		if (process == nullptr)
			return;

		if (!DebugActiveProcess(processID)) {
			CloseHandle(process);
			return;
		}

		std::wstring fileName;
		{
			WCHAR path[MAX_PATH];
			if (!GetModuleFileNameExW(process, nullptr, path, MAX_PATH)) {
				fileName = L"(Unknown)";
			} else {
				fileName = path;
				const size_t where = fileName.rfind(L'\\');
				if (where != std::wstring::npos)
					fileName = fileName.substr(where + 1);
			}
		}

		std::wstringstream ss;
		ss << fileName << " [" << processID << "]: Attached to Process.";
		SetAttribute(colStartStop);
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
					std::wstringstream ss;
					ss << fileName << " [" << processID << "]: ";
					ProcessDebugStringEvent(process, event.u.DebugString, ss);
					SetAttribute(colString);
					Print(ss.str());
					break;
				}

				case EXCEPTION_DEBUG_EVENT:
				{
					std::wstringstream ss;
					ss << fileName << " [" << processID << "]: ";
					ProcessExceptionEvent(event.u.Exception, ss);

					SetAttribute(colException);
					if ((event.u.Exception.ExceptionRecord.ExceptionFlags & 
						 EXCEPTION_NONCONTINUABLE) != 0) {
						SetAttribute(colFatal);
						ss << "[FATAL]";
						contFlag = DBG_EXCEPTION_NOT_HANDLED;
					}
					Print(ss.str());
					break;
				}

				case EXIT_PROCESS_DEBUG_EVENT:
				{
					std::wstringstream ss;
					ss << fileName << " [" << processID << "]: Process Exited.";
					SetAttribute(colStartStop);
					Print(ss.str());
					this->isDetaching = true;
				}

				default:
					break;
			}

			ContinueDebugEvent(event.dwProcessId, event.dwThreadId, contFlag);
		}

		CloseHandle(process);
		DebugActiveProcessStop(processID);
	}};
}

void DebugThread::Detach() {
	isDetaching = true;
	workThread.join();
}

void DebugThread::Print(std::wstring output) const {
	std::lock_guard<std::mutex> streamGuard(streamMutex);
	std::wcout << output;
	if (output[output.length() - 1] != L'\n')
		std::wcout << L'\n';
}
