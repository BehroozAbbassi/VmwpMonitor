#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "VideoS3Device.h"

VM_EMULATED_DEVICES_HOOK_ENTRY VideoS3DeviceHookEntry = {0};

/// @brief VideoS3Device::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall VideoS3DeviceNotifyIoPortReadHooked(
    _In_ VideoS3Device This,
    _In_ UINT16        IoPort,
    _In_ UINT16        AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = VideoS3DeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] VideoS3Device [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

/// @brief VideoS3Device::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall VideoS3DeviceNotifyIoPortWriteHooked(
    _In_ VideoS3Device This,
    _In_ UINT16        IoPort,
    _In_ UINT16        AccessSize,
    _In_ UINT32        Data)
{
    dbgPrint(L"[!] VideoS3Device [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    return VideoS3DeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitVideoS3DeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x2b420;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x2b4b0;

    pEntry->DevceType            = EmulatedDeviceTypeVideoS3Device;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] VideoS3Device::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] VideoS3Device::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &VideoS3DeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &VideoS3DeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] VideoS3Device::NotifyIoPortReadHooked  = 0x%llX\n", VideoS3DeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] VideoS3Device::NotifyIoPortWriteHooked = 0x%llX\n", VideoS3DeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] VideoS3Device::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] VideoS3Device::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupVideoS3DeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitVideoS3DeviceHooks(&VideoS3DeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&VideoS3DeviceHookEntry.NotifyIoPortReadOrig,
                             VideoS3DeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] VideoS3Device::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] VideoS3Device::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&VideoS3DeviceHookEntry.NotifyIoPortWriteOrig,
                             VideoS3DeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] VideoS3Device::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] VideoS3Device::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}