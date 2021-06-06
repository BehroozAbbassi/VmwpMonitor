
#include "pch.h"

#include "EmulatedDeviceDefines.h"
#include "VmEmulatedDevices/VmEmulatedDevices.h"

BOOL OnAttachEvent(VOID)
{
    TCHAR szTargetProcessName[MAX_PATH];
    GetModuleFileName(NULL, szTargetProcessName, MAX_PATH);

    dbgPrint(_T("[!] VmwpMonitor.dll injected to [%s]\n"), szTargetProcessName);
    dbgPrint(_T("[!] VmwpMonitor started.\n"));

    //
    // VmEmulatedDevices hooks
    //

    SetupPciBusDeviceHooks(EnableHook);
    SetupI8042DeviceHooks(EnableHook);
    SetupIsaBusDeviceHooks(EnableHook);
    SetupPitDeviceHooks(EnableHook);
    SetupPicDeviceHooks(EnableHook);
    SetupVideoS3DeviceHooks(EnableHook);
    SetupDmaControllerDeviceHooks(EnableHook);

    return TRUE;
}

BOOL APIENTRY
DllMain(HMODULE hModule,
        DWORD   ul_reason_for_call,
        LPVOID  lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    DisableThreadLibraryCalls(hModule);

    dbgPrint(_T("[!] VmwpMonitor.dll injected.\n"));

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return OnAttachEvent();

    case DLL_THREAD_ATTACH:
        dbgPrint(L"DLL_THREAD_ATTACH\n");
        break;
    case DLL_THREAD_DETACH:
        dbgPrint(L"DLL_THREAD_DETACH\n");
        break;
    case DLL_PROCESS_DETACH:
        //SetupVmEmulatedDevicesHooks(DisableHook);
        //dbgPrint(_T("Detached From [%s]\n"), szTargetProcessName);
        dbgPrint(_T("[!] VmwpMonitor end.\n"));
        break;
    }
    return TRUE;
}
