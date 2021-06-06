#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY DmaControllerDeviceHookEntry;

// pointer to the DmaControllerDevice class used in __thiscall
// for c++ class member functions
typedef PVOID DmaControllerDevice;

//
// Hooked functions
//

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
    _Inout_ UINT32 * pData);

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
    _In_ UINT32              Data);

BOOL
InitDmaControllerDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupDmaControllerDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);