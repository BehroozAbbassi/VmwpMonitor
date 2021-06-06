#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "I8042Device.h"

VM_EMULATED_DEVICES_HOOK_ENTRY I8042DeviceHookEntry = {0};

/// @brief I8042Device::NotifyIoPortRead
/// Ps2Keyboard and Ps2Mouse
/// @param This[in]
/// @param IoPort[in] IO ports 0x60, 0x61, 0x62, 0x64
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall I8042DeviceNotifyIoPortReadHooked(
    _In_ I8042Device This,
    _In_ UINT16      IoPort,
    _In_ UINT16      AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = I8042DeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] I8042 [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief I8042Device::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall I8042DeviceNotifyIoPortWriteHooked(
    _In_ I8042Device This,
    _In_ UINT16      IoPort,
    _In_ UINT16      AccessSize,
    _In_ UINT32      Data)
{
    dbgPrint(L"[!] I8042 [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return I8042DeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitI8042DeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x1fbd0;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x1ff60;

    pEntry->DevceType            = EmulatedDeviceTypeI8042Device;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] I8042Device::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] I8042Device::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &I8042DeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &I8042DeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] I8042Device::NotifyIoPortReadHooked  = 0x%llX\n", I8042DeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] I8042Device::NotifyIoPortWriteHooked = 0x%llX\n", I8042DeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] I8042Device::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] I8042Device::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupI8042DeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitI8042DeviceHooks(&I8042DeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&I8042DeviceHookEntry.NotifyIoPortReadOrig,
                             I8042DeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] I8042Device::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] I8042Device::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&I8042DeviceHookEntry.NotifyIoPortWriteOrig,
                             I8042DeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] I8042Device::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] I8042Device::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}