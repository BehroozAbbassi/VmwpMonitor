#pragma once

typedef enum _HOOK_SETUP_MODE
{
    EnableHook,
    DisableHook
} HOOK_SETUP_MODE,
    *PHOOK_SETUP_MODE;

BOOL
SetupInlineHook(_In_ PVOID * pOrigfunc,
                _In_ PVOID *         pHookedfunc,
                _In_ HOOK_SETUP_MODE SetupMode);