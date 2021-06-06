#include "pch.h"
#include "Hook.h"

#include <detours/detours.h>

BOOL
SetupInlineHook(_In_ PVOID * pOrigfunc,
                _In_ PVOID *         pHookedfunc,
                _In_ HOOK_SETUP_MODE SetupMode)
{
    //
    // Validate function addrs
    //

    //if ((PVOID *)pOrigfunc == NULL || (PVOID *)pHookedfunc = NULL)
    //    return FALSE;

    if (DetourTransactionBegin() != NO_ERROR)
        return FALSE;

    if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR)
    {
        DetourTransactionCommit();
        return FALSE;
    }

    switch (SetupMode)
    {
    case EnableHook:
    {
        if (DetourAttach((PVOID *)pOrigfunc, pHookedfunc) != NO_ERROR)
        {
            DetourTransactionCommit();
            return FALSE;
        }
    }
    break;
    case DisableHook:
    {
        if (DetourDetach((PVOID *)pOrigfunc, pHookedfunc) != NO_ERROR)
        {
            DetourTransactionCommit();
            return FALSE;
        }
    }
    default:
        return FALSE;
    }

    if (DetourTransactionCommit() != NO_ERROR)
    {
        DetourTransactionAbort();
        return FALSE;
    }

    return TRUE;
}