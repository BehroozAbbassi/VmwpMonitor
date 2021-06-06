#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "PitDevice.h"

VM_EMULATED_DEVICES_HOOK_ENTRY PitDeviceHookEntry = {0};

/// @brief PitDevice::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PitDeviceNotifyIoPortReadHooked(
    _In_ PitDevice This,
    _In_ UINT16    IoPort,
    _In_ UINT16    AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = PitDeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] PitDevice [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief PitDevice::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PitDeviceNotifyIoPortWriteHooked(
    _In_ PitDevice This,
    _In_ UINT16    IoPort,
    _In_ UINT16    AccessSize,
    _In_ UINT32    Data)
{
    dbgPrint(L"[!] PitDevice [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return PitDeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitPitDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
{
    if (pEntry == NULL)
        return FALSE;

    HMODULE hVmEmulatedDevices = GetModuleHandle(_T("VmEmulatedDevices"));
    if (hVmEmulatedDevices == NULL)
        return FALSE;

    dbgPrint(_T("[!] Base address of VmEmulatedDevices.dll module  0x%" _T(PRIX64) L" .\n"),
             hVmEmulatedDevices);

    //
    // RVA Offset of target functions,
    // Notice that these offsets can change on diffrent versions of target module
    //
    // RVA Offset = FunctionAddress - BaseAddress
    //

    ULONG64 NotifyIoPortReadRvaOffset   = 0x1cff0;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x1d0a0;

    pEntry->DevceType            = EmulatedDeviceTypePitDevice;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] PitDevice::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] PitDevice::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &PitDeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &PitDeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] PitDevice::NotifyIoPortReadHooked  = 0x%llX\n", PitDeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] PitDevice::NotifyIoPortWriteHooked = 0x%llX\n", PitDeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] PitDevice::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] PitDevice::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupPitDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitPitDeviceHooks(&PitDeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&PitDeviceHookEntry.NotifyIoPortReadOrig,
                             PitDeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PitDevice::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PitDevice::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&PitDeviceHookEntry.NotifyIoPortWriteOrig,
                             PitDeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PitDevice::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PitDevice::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}