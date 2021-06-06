#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "DmaControllerDevice.h"

VM_EMULATED_DEVICES_HOOK_ENTRY DmaControllerDeviceHookEntry = {0};

/// @brief DmaControllerDevice::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall DmaControllerDeviceNotifyIoPortReadHooked(
    _In_ DmaControllerDevice This,
    _In_ UINT16              IoPort,
    _In_ UINT16              AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = DmaControllerDeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] DmaController [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief DmaControllerDevice::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall DmaControllerDeviceNotifyIoPortWriteHooked(
    _In_ DmaControllerDevice This,
    _In_ UINT16              IoPort,
    _In_ UINT16              AccessSize,
    _In_ UINT32              Data)
{
    dbgPrint(L"[!] DmaController [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return DmaControllerDeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitDmaControllerDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x11910;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x11a30;

    pEntry->DevceType            = EmulatedDeviceTypeDmaControllerDevice;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &DmaControllerDeviceNotifyIoPortReadHooked;
    pEntry->NotifyIoPortReadHooked  = &DmaControllerDeviceNotifyIoPortWriteHooked;

    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortReadHooked  = 0x%llX\n", DmaControllerDeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortWriteHooked = 0x%llX\n", DmaControllerDeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupDmaControllerDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitDmaControllerDeviceHooks(&DmaControllerDeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&DmaControllerDeviceHookEntry.NotifyIoPortReadOrig,
                             DmaControllerDeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] DmaControllerDevice::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&DmaControllerDeviceHookEntry.NotifyIoPortWriteOrig,
                             DmaControllerDeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] DmaControllerDevice::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] DmaControllerDevice::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}