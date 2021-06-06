#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY PciBusDeviceHookEntry;

// pointer to the PciBusDevice class used in __thiscall
// for c++ class member functions
typedef PVOID PciBusDevice;

typedef union _PCI_CONFIG_ADDRESS
{
    UINT32 AsUint32;
    struct
    {
        UINT32 Rzv : 2;            /* 1-0   */
        UINT32 RegisterNumber : 6; /* 7-2   */
        UINT32 FunctionNumber : 3; /* 10-8  */
        UINT32 DeviceNumber : 5;   /* 15-11 */
        UINT32 BusNumber : 8;      /* 23-16 */
        UINT32 Reserverd1 : 7;     /* 30-24 */
        UINT32 EnableBit : 1;      /* 31    */
    };
} PCI_CONFIG_ADDRESS;

//
// Hooked functions
//

/// @brief PciBusDevice::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PciBusDeviceNotifyIoPortReadHooked(
    _In_ PciBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _Inout_ UINT32 * pData);

/// @brief PciBusDevice::NotifyIoPortWrite
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall PciBusDeviceNotifyIoPortWriteHooked(
    _In_ PciBusDevice This,
    _In_ UINT16       IoPort,
    _In_ UINT16       AccessSize,
    _In_ UINT32       Data);

BOOL
InitPciBusDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupPciBusDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);