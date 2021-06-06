#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY PitDeviceHookEntry;

// pointer to the PitDevice class used in __thiscall
// for c++ class member functions
typedef PVOID PitDevice;

//
// Hooked functions
//

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
    _Inout_ UINT32 * pData);

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
    _In_ UINT32    Data);

BOOL
InitPitDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupPitDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);