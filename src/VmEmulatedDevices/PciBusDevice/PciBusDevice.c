#include "pch.h"
#include "EmulatedDeviceDefines.h"
#include "PciBusDevice.h"

VM_EMULATED_DEVICES_HOOK_ENTRY PciBusDeviceHookEntry = {0};

VOID
PrintLogPciConfigAddress(_In_ PCI_CONFIG_ADDRESS PciConfigAddress)
{
    dbgPrint(L"PciConfigAddress Value [ 0x%X ]\n"
             L"    + Rzv            : 0x%x \n"
             L"    + RegisterNumber : 0x%x \n"
             L"    + FunctionNumber : 0x%x \n"
             L"    + DeviceNumber   : 0x%x \n"
             L"    + BusNumber      : 0x%x \n"
             L"    + Reserverd1     : 0x%x \n"
             L"    + EnableBit      : 0x%x \n\n",
             PciConfigAddress.AsUint32,
             PciConfigAddress.Rzv,
             PciConfigAddress.RegisterNumber,
             PciConfigAddress.FunctionNumber,
             PciConfigAddress.DeviceNumber,
             PciConfigAddress.BusNumber,
             PciConfigAddress.Reserverd1,
             PciConfigAddress.EnableBit);
}

//-----------------------------------------------------------------------------
// Hooked functions
//-----------------------------------------------------------------------------

LONG __fastcall PciBusDeviceNotifyIoPortReadHooked(
    _In_ PciBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _Inout_ UINT32 * pData)
{
    LONG Ret = PciBusDeviceHookEntry.NotifyIoPortReadOrig(This, IoPort, AccessSize, pData);

    dbgPrint(L"[!] PciBus [R] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             *pData);
    //Sleep(5);
    return Ret;
}

LONG __fastcall PciBusDeviceNotifyIoPortWriteHooked(
    _In_ PciBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _In_ UINT32       Data)
{
    dbgPrint(L"[!] PciBus [W] ( Port = 0x%hX, AccessSize = %d, Data = 0x%x )\n",
             IoPort,
             AccessSize,
             Data);

    // PCI_CONFIG_ADDRESS
    if (IoPort == 0xCF8)
    {
        PCI_CONFIG_ADDRESS PciConfigAddress = {Data};
        PrintLogPciConfigAddress(PciConfigAddress);
    }

    return PciBusDeviceHookEntry.NotifyIoPortWriteOrig(This, IoPort, AccessSize, Data);
}

BOOL
InitPciBusDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry)
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

    ULONG64 NotifyIoPortReadRvaOffset   = 0x175a0;
    ULONG64 NotifyIoPortRWriteRvaOffset = 0x175e0;

    pEntry->DevceType            = EmulatedDeviceTypePciBusDevice;
    pEntry->NotifyIoPortReadOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortRead,        // Function Prototype
                                                          hVmEmulatedDevices,         // Module Base address
                                                          NotifyIoPortReadRvaOffset); // Function RVA Offset

    pEntry->NotifyIoPortWriteOrig = GET_FUNCTION_BY_OFFSET(PfnNotifyIoPortWrite,         // Function Prototype
                                                           hVmEmulatedDevices,           // Module Base address
                                                           NotifyIoPortRWriteRvaOffset); // Function RVA Offset

    dbgPrint(L"[!] PciBusDevice::NotifyIoPortReadOrig    = 0x%llX\n", pEntry->NotifyIoPortReadOrig);
    dbgPrint(L"[!] PciBusDevice::NotifyIoPortWriteOrig   = 0x%llX\n", pEntry->NotifyIoPortWriteOrig);

    //
    // Validate function pointer/address
    //

    if (pEntry->NotifyIoPortReadOrig == NULL || pEntry->NotifyIoPortWriteOrig == NULL)
        return FALSE;

    pEntry->NotifyIoPortWriteHooked = &PciBusDeviceNotifyIoPortWriteHooked;
    pEntry->NotifyIoPortReadHooked  = &PciBusDeviceNotifyIoPortReadHooked;

    dbgPrint(L"[!] PciBusDevice::NotifyIoPortReadHooked  = 0x%llX\n", PciBusDeviceNotifyIoPortReadHooked);
    dbgPrint(L"[!] PciBusDevice::NotifyIoPortWriteHooked = 0x%llX\n", PciBusDeviceNotifyIoPortWriteHooked);

    dbgPrint(L"[!] PciBusDevice::NotifyIoPortReadHooked  = 0x%llX\n", pEntry->NotifyIoPortReadHooked);
    dbgPrint(L"[!] PciBusDevice::NotifyIoPortWriteHooked = 0x%llX\n", pEntry->NotifyIoPortWriteHooked);

    return TRUE;
}

BOOL
SetupPciBusDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode)
{
    BOOL Status = FALSE;

    if (SetupMode == EnableHook)
    {
        if (InitPciBusDeviceHooks(&PciBusDeviceHookEntry) == FALSE)
            return FALSE;
    }

    Status = SetupInlineHook(&PciBusDeviceHookEntry.NotifyIoPortReadOrig,
                             PciBusDeviceHookEntry.NotifyIoPortReadHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PciBusDevice::NotifyIoPortRead Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PciBusDevice::NotifyIoPortRead Hooke Failed!\n");

    Status = SetupInlineHook(&PciBusDeviceHookEntry.NotifyIoPortWriteOrig,
                             PciBusDeviceHookEntry.NotifyIoPortWriteHooked,
                             SetupMode);
    if (Status)
        dbgPrint(L"[!] PciBusDevice::NotifyIoPortWrite Hooked Succesfully!\n");
    else
        dbgPrint(L"[-] PciBusDevice::NotifyIoPortWrite Hooke Failed!\n");

    return Status;
}