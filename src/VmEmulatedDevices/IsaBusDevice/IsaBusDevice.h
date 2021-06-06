#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY IsaBusDeviceHookEntry;

// pointer to the IsaBusDevice class used in __thiscall
// for c++ class member functions
typedef PVOID IsaBusDevice;

//
// Hooked functions
//

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
    _Inout_ UINT32 * pData);

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
    _In_ UINT32       Data);

BOOL
InitIsaBusDeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupIsaBusDeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);