#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY I8042DeviceHookEntry;

// pointer to the I8042Device class used in __thiscall
// for c++ class member functions
typedef PVOID I8042Device;

//
// Hooked functions
//

/// @brief I8042Device::NotifyIoPortRead
/// @param This
/// @param IoPort
/// @param AccessSize
/// @param pData
/// @return
LONG __fastcall I8042DeviceNotifyIoPortReadHooked(
    _In_ I8042Device This,
    _In_ UINT16      IoPort,
    _In_ UINT16      AccessSize,
    _Inout_ UINT32 * pData);

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
    _In_ UINT32      Data);

BOOL
InitI8042DeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupI8042DeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);