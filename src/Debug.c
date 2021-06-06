
#include "pch.h"
#include "Debug.h"

LPTSTR
_cdecl VStringFormat(
    _In_z_ _Printf_format_string_ LPTSTR MessageFormat,
    va_list                              vArgs)
{
    TCHAR lpDebugMessage[(MAX_DEBUG_STRING + 2) * sizeof(TCHAR)];
    DWORD dwDebugMessageLength = 0;

    dwDebugMessageLength = _vsctprintf(MessageFormat, vArgs) // _vscprintf doesn't count
                           + 1;                              // terminating '\0'

    _vstprintf_s(lpDebugMessage, dwDebugMessageLength, MessageFormat, vArgs);

    return lpDebugMessage;
}

LPTSTR
_cdecl StringFormat(
    _In_z_ LPTSTR MessageFormat,
    ...)
{
    LPTSTR lpResultString = NULL;
    TCHAR  lpDebugMessage[(MAX_DEBUG_STRING + 2) * sizeof(TCHAR)];

    DWORD dwDebugMessageLength = 0;

    va_list vArgs;
    va_start(vArgs, MessageFormat);

    dwDebugMessageLength = _vsctprintf(MessageFormat, vArgs) // _vscprintf doesn't count
                           + 1;                              // terminating '\0'

    _vstprintf_s(lpDebugMessage, dwDebugMessageLength, MessageFormat, vArgs);

    lpResultString = lpDebugMessage;

    va_end(vArgs);

    return lpResultString;
}

#ifdef _DEBUG
VOID _cdecl DbgPrintEx(
    _In_z_ LPTSTR MessageFormat,
    ...)
{
    // STRSAFE_MAX_CCH

    va_list vArgs;
    va_start(vArgs, MessageFormat);

    //SIZE_T dwSize = (MAX_DEBUG_STRING + 2) * sizeof(TCHAR);
    //LPTSTR lpDebugMessage = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);

    ////  LPTSTR strDebug = (LPTSTR)(malloc(dwSize)); // do not use zalloc here
    //// SecureZeroMemory(strDebug, dwSize);

    //DWORD dwDebugMessageLength = _vsctprintf(MessageFormat, vArgs) // _vscprintf doesn't count
    //    + 1; // terminating '\0'

    //_vstprintf_s(lpDebugMessage, dwDebugMessageLength, MessageFormat, vArgs);

    // if (StringCchPrintf(lpDebugMessage, dwSize, MessageFormat, vArgs) == S_OK)
    OutputDebugString(VStringFormat(MessageFormat, vArgs));

    //HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, lpDebugMessage);
    //va_end(vArgs);
    // free(strDebug);
}

#endif // _DEBUG