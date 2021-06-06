#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "PicDevice.h"

VM_EMULATED_DEVICES_HOOK_ENTRY PicDeviceHookEntry = {0};

/// @brief PicDevice::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PicDeviceNotifyIoPortReadHooked(
    _In_ PicDevice This,
    _In_ UINT16    IoPort,
    _In_ UINT16    AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = PicDeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] PicDevice [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief PicDevice::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PicDeviceNotifyIoPortWriteHooked(
    _In_ PicDevice This,
    _In_ UINT16    IoPort,
    _In_ UINT16    AccessSize,
    _In_ UINT32    Data)
{
    dbgPrint(L"[!] PicDevice [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return PicDeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitPicDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x1a230;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x1a460;

    pEntry->DevceType            = EmulatedDeviceTypePicDevice;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] PicDevice::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] PicDevice::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &PicDeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &PicDeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] PicDevice::NotifyIoPortReadHooked  = 0x%llX\n", PicDeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] PicDevice::NotifyIoPortWriteHooked = 0x%llX\n", PicDeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] PicDevice::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] PicDevice::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupPicDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitPicDeviceHooks(&PicDeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&PicDeviceHookEntry.NotifyIoPortReadOrig,
                             PicDeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PicDevice::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PicDevice::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&PicDeviceHookEntry.NotifyIoPortWriteOrig,
                             PicDeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PicDevice::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PicDevice::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}