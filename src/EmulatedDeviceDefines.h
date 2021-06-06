#pragma once

//
// Prototypes
//

typedef LONG(__fastcall * PfnNotifyIoPortRead)(
    _In_ PVOID  This,
    _In_ UINT16 IoPort,
    _In_ UINT16 AccessSize,
    _Inout_ UINT32 * pData);

typedef LONG(__fastcall * PfnNotifyIoPortWrite)(
    _In_ PVOID  This,
    _In_ UINT16 IoPort,
    _In_ UINT16 AccessSize,
    _In_ UINT32 Data);

//----Structures---------------------------------------------------------------

typedef enum _EMULATION_DEVICE_MODULES
{

    EmulationDeviceModuleNone = 0,

    /// @brief VmEmulatedDevices.dll
    /// Responsibility
    /// - Emulates many devices above 1250 and 1500 functions
    ///
    /// Devices/Logics in scope
    /// - PitDevice PicDevice PciBusDevice SpeakerDevice IsaBusDevice
    ///   Ps2Keyboard I8042Device VideoS3Device VideoDevice DmaControllerDevice
    ///
    EmulationDeviceModuleVmEmulatedDevices,

    /// @brief vmuidevices.dll
    /// Responsibility
    /// - Emulates many devices, over 1500 functions
    ///
    /// Devices/Logics in scope
    /// - VideoDevice VideoDirt InputManager SynthMouseDevice SynthKeyboardDevice HidDevice SynthRdpServerConnection VideoSynthDevice
    ///
    EmulationDeviceModuleVmUiDevices,

    /// @brief VmEmulatedStorage.dll
    /// Responsibility
    /// - Storage devices
    ///
    /// Devices/Logics in scope
    /// - IdeControllerDevice IdeOpticalDrive IdeHardDrive IdeChannel DiskMetricDevice FloppyDrive
    ///
    EmulationDeviceModuleVmEmulatedStorage,

    /// @brief vmEmulatedNic.dll
    /// Responsibility
    /// - Network card devices
    ///
    /// Devices/Logics in scope
    /// - Class EthernetDevice, emulates the following cards: DEC 21041, DEC 21140A, DEC 21143
    ///
    EmulationDeviceModuleVmEmulatedNic,

    /// @brief winHvEmulation.dll
    /// Responsibility
    /// - x86 instructions emulation at UM (EmulatorVP functions)
    ///
    /// Devices/Logics in scope
    /// - N/A
    ///
    EmulationDeviceModuleWinHvEmulation,

    /// @brief Vmchipset.dll
    /// Responsibility
    /// - Emulates many chipsets
    ///
    /// Devices/Logics in scope
    /// - IoApicEmulator PowerManagementEmulator
    ///
    EmulationDeviceModuleVmchipset,

    EmulationDeviceModuleMax = EmulationDeviceModuleVmchipset

} EMULATION_DEVICE_MODULES;

/// @brief Emulated devices in VmEmulatedDevices.dll
typedef enum _EMULATED_DEVICE_TYPE
{
    EmulatedDeviceTypeNon          = 0,
    EmulatedDeviceTypePciBusDevice = 1,
    EmulatedDeviceTypeI8042Device,
    EmulatedDeviceTypeIsaBusDevice,
    EmulatedDeviceTypePitDevice,
    EmulatedDeviceTypePicDevice,
    EmulatedDeviceTypeVideoS3Device,
    EmulatedDeviceTypeDmaControllerDevice,
    EmulatedDeviceTypeMax = EmulatedDeviceTypeDmaControllerDevice
} EMULATED_DEVICE_TYPE;

typedef enum _DEVICE_IO_ACTION
{
    DeviceIoActionNon,
    DeviceIoActionRead,
    DeviceIoActionWrite,
    DeviceIoActionMax = DeviceIoActionWrite
} DEVICE_IO_ACTION;

typedef struct _VMWP_RVA_OFFSETS
{
    /// @brief Address of vmemulateddevices!PciBusDevice::NotifyIoPortRead
    ULONG64 VmwpRvaOffsetPciBusDeviceNotifyIoPortRead;

    /// @brief Address of vmemulateddevices!PciBusDevice::NotifyIoPortWrite
    ULONG64 VmwpRvaOffsetPciBusDeviceNotifyIoPortWrit;

} VMWP_RVA_OFFSETS;

typedef struct _FUNCTION_RVA_OFFSETS
{
    ULONG64 NotifyIoPortRead;
    ULONG64 NotifyIoPortWrite;

} FUNCTION_RVA_OFFSETS;

typedef struct _VM_EMULATED_DEVICES_HOOK_ENTRY
{
    EMULATED_DEVICE_TYPE DevceType;
    PfnNotifyIoPortRead  NotifyIoPortReadOrig;
    PfnNotifyIoPortWrite NotifyIoPortWriteOrig;
    LPVOID               NotifyIoPortReadHooked;
    LPVOID               NotifyIoPortWriteHooked;
} VM_EMULATED_DEVICES_HOOK_ENTRY;
