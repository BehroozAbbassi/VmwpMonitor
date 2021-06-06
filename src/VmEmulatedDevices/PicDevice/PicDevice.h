#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY PicDeviceHookEntry;

// pointer to the PicDevice class used in __thiscall
// for c++ class member functions
typedef PVOID PicDevice;

//
// Hooked functions
//

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
    _Inout_ UINT32 * pData);

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
    _In_ UINT32    Data);

BOOL
InitPicDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupPicDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);