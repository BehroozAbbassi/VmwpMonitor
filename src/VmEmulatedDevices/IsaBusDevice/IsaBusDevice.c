#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "IsaBusDevice.h"

VM_EMULATED_DEVICES_HOOK_ENTRY IsaBusDeviceHookEntry = {0};

/// @brief IsaBusDevice::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall IsaBusDeviceNotifyIoPortReadHooked(
    _In_ IsaBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = IsaBusDeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] IsaBus [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief IsaBusDevice::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall IsaBusDeviceNotifyIoPortWriteHooked(
    _In_ IsaBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _In_ UINT32       Data)
{
    dbgPrint(L"[!] IsaBus [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return IsaBusDeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitIsaBusDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x14a70;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x14a80;

    pEntry->DevceType            = EmulatedDeviceTypeIsaBusDevice;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &IsaBusDeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &IsaBusDeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortReadHooked  = 0x%llX\n", IsaBusDeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortWriteHooked = 0x%llX\n", IsaBusDeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] IsaBusDevice::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupIsaBusDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitIsaBusDeviceHooks(&IsaBusDeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&IsaBusDeviceHookEntry.NotifyIoPortReadOrig,
                             IsaBusDeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] IsaBusDevice::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] IsaBusDevice::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&IsaBusDeviceHookEntry.NotifyIoPortWriteOrig,
                             IsaBusDeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] IsaBusDevice::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] IsaBusDevice::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}