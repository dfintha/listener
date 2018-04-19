#if !defined(LISTENER_MESSAGES)
#define LISTENER_MESSAGES
#pragma once

constexpr const wchar_t UNKNOWN_PATH[] =
    L"(Unknown)";

constexpr const wchar_t FATAL_EXCEPTION[] =
    L"(Fatal)";

constexpr const wchar_t MESSAGE_ATTACHED[] =
    L"Attached to Process";

constexpr const wchar_t MESSAGE_EXITED[] =
    L"Process Exited";

constexpr const wchar_t *EXCEPTION_ACCESS_VIOLATION_DETAIL =
    L"EXCEPTION_ACCESS_VIOLATION: "
    L"The thread tried to read from or write to a "
    L"virtual address for which it does not have the "
    L"appropriate access.";

constexpr const wchar_t *EXCEPTION_ARRAY_BOUNDS_EXCEEDED_DETAIL =
    L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED: "
    L"The thread tried to access an array element that "
    L"is out of bounds and the underlying hardware "
    L"supports bounds checking.";

constexpr const wchar_t *EXCEPTION_BREAKPOINT_DETAIL =
    L"EXCEPTION_BREAKPOINT: "
    L"A breakpoint was encountered.";

constexpr const wchar_t *EXCEPTION_DATATYPE_MISALIGNMENT_DETAIL =
    L"EXCEPTION_DATATYPE_MISALIGNMENT: "
    L"The thread tried to read or write data that is "
    L"misaligned on hardware that does not provide "
    L"alignment.";

constexpr const wchar_t *EXCEPTION_FLT_DENORMAL_OPERAND_DETAIL =
    L"EXCEPTION_FLT_DENORMAL_OPERAND: "
    L"One of the operands in a floating-point operation "
    L"is denormal.";

constexpr const wchar_t *EXCEPTION_FLT_DIVIDE_BY_ZERO_DETAIL =
    L"EXCEPTION_FLT_DIVIDE_BY_ZERO: "
    L"The thread tried to divide a floating-point "
    L"value by a floating-point divisor of zero.";

constexpr const wchar_t *EXCEPTION_FLT_INEXACT_RESULT_DETAIL =
    L"EXCEPTION_FLT_INEXACT_RESULT: "
    L"The result of a floating-point operation cannot "
    L"be represented exactly as a decimal fraction.";

constexpr const wchar_t *EXCEPTION_FLT_INVALID_OPERATION_DETAIL =
    L"EXCEPTION_FLT_INVALID_OPERATION: "
    L"This exception represents any floating-point "
    L"exception not included in this list.";

constexpr const wchar_t *EXCEPTION_FLT_OVERFLOW_DETAIL =
    L"EXCEPTION_FLT_OVERFLOW: "
    L"The exponent of a floating-point operation is "
    L"greater than the magnitude allowed by the "
    L"corresponding type.";

constexpr const wchar_t *EXCEPTION_FLT_STACK_CHECK_DETAIL =
    L"EXCEPTION_FLT_STACK_CHECK: "
    L"The stack overflowed or underflowed as the result "
    L"of a floating-point operation.";

constexpr const wchar_t *EXCEPTION_FLT_UNDERFLOW_DETAIL =
    L"EXCEPTION_FLT_UNDERFLOW: "
    L"The exponent of a floating-point operation is "
    L"less than the magnitude allowed by the "
    L"corresponding type.";

constexpr const wchar_t *EXCEPTION_ILLEGAL_INSTRUCTION_DETAIL =
    L"EXCEPTION_ILLEGAL_INSTRUCTION: "
    L"The thread tried to execute an invalid "
    L"instruction.";

constexpr const wchar_t *EXCEPTION_IN_PAGE_ERROR_DETAIL =
    L"EXCEPTION_IN_PAGE_ERROR: "
    L"The thread tried to access a page that was not "
    L"present, and the system was unable to load the "
    L"page.";

constexpr const wchar_t *EXCEPTION_INT_DIVIDE_BY_ZERO_DETAIL =
    L"EXCEPTION_INT_DIVIDE_BY_ZERO: "
    L"The thread tried to divide an integer value by "
    L"an integer divisor of zero.";

constexpr const wchar_t *EXCEPTION_INT_OVERFLOW_DETAIL =
    L"EXCEPTION_INT_OVERFLOW: "
    L"The result of an integer operation caused a "
    L"carry out of the most significant bit of the "
    L"result.";

constexpr const wchar_t *EXCEPTION_INVALID_DISPOSITION_DETAIL =
    L"EXCEPTION_INVALID_DISPOSITION: "
    L"An exception handler returned an invalid "
    L"disposition to the exception dispatcher. ";

constexpr const wchar_t *EXCEPTION_NONCONTINUABLE_EXCEPTION_DETAIL =
    L"EXCEPTION_NONCONTINUABLE_EXCEPTION: "
    L"The thread tried to continue execution after a "
    L"noncontinuable exception occurred.";

constexpr const wchar_t *EXCEPTION_PRIV_INSTRUCTION_DETAIL =
    L"EXCEPTION_PRIV_INSTRUCTION: "
    L"The thread tried to execute an instruction whose "
    L"operation is not allowed in the current machine "
    L"mode.";

constexpr const wchar_t *EXCEPTION_SINGLE_STEP_DETAIL =
    L"EXCEPTION_SINGLE_STEP: "
    L"A trace trap or other single-instruction mechanism "
    L"signaled that one instruction has been executed.";

constexpr const wchar_t *EXCEPTION_STACK_OVERFLOW_DETAIL =
    L"EXCEPTION_STACK_OVERFLOW: "
    L"The thread used up its stack.";

constexpr const wchar_t *EXCEPTION_UNKNOWN_DETAIL =
    L"EXCEPTION_UNKNOWN: "
    L"An unknown exception occurred.";

#endif
