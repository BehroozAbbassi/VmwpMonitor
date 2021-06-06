#pragma once

//
// Windows API
//

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <inttypes.h>

//
// Project internal headers
//

#include "Debug.h"

//
// Hook utils
//
#include "HookUtils/Hook.h"

#define GET_FUNCTION_ADDR_BY_OFFSET(Base, Offset) \
    (LPVOID)((ULONG64)Base + Offset)

#define GET_FUNCTION_BY_OFFSET(Protoytype, Base, Offset) \
    (Protoytype)(GET_FUNCTION_ADDR_BY_OFFSET(Base, Offset))
