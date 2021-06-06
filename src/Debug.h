#pragma once

typedef enum _DEBUG_MESSAGE_TYPE
{
    MessageTypeError,
    MessageTypeWarning,
    MessageTypeInfo
} DEBUG_MESSAGE_TYPE;

#define _DEBUG

#define MAX_DEBUG_STRING 1024
LPTSTR
_cdecl StringFormat(
    _In_z_ LPTSTR MessageFormat,
    ...);

#ifdef _DEBUG

VOID _cdecl DbgPrintEx(
    _In_z_ LPTSTR MessageFormat,
    ...);

#    define dbgPrint(format, ...) \
        DbgPrintEx(format, __VA_ARGS__)

#    define ENTER_FUNCTION \
        dbgPrint(_T("[Enter] %s\n"), __FUNCTIONW__);

#    define LEAVE_FUNCTION \
        dbgPrint(_T("[LEAVE] %s\n"), __FUNCTIONW__);

#else
#    define ENTER_FUNCTION
#    define LEAVE_FUNCTION
#    define dbgPrint(format, ...)
#endif // _DEBUG
