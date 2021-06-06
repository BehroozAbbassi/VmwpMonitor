#pragma once

extern VM_EMULATED_DEVICES_HOOK_ENTRY VideoS3DeviceHookEntry;

// pointer to the VideoS3Device class used in __thiscall
// for c++ class member functions
typedef PVOID VideoS3Device;

//
// Hooked functions
//

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
    _Inout_ UINT32 * pData);

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
    _In_ UINT32        Data);

BOOL
InitVideoS3DeviceHooks(_Inout_ VM_EMULATED_DEVICES_HOOK_ENTRY * pEntry);

BOOL
SetupVideoS3DeviceHooks(_In_ HOOK_SETUP_MODE SetupMode);
