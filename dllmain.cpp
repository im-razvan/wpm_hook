#include <Windows.h>
#include "MinHook.h"

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

/*
BOOL WriteProcessMemory(
  [in]  HANDLE  hProcess,
  [in]  LPVOID  lpBaseAddress,
  [in]  LPCVOID lpBuffer,
  [in]  SIZE_T  nSize,
  [out] SIZE_T  *lpNumberOfBytesWritten
);
*/

typedef int (WINAPI *WRITEPROCESSMEMORY)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T);

WRITEPROCESSMEMORY oWriteProcessMemory = NULL;

int WINAPI DetourWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten)
{
    // do something

    return oWriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, *lpNumberOfBytesWritten); // return original function
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (MH_Initialize() != MH_OK) return FALSE;

        if (MH_CreateHook(
            &WriteProcessMemory, &DetourWriteProcessMemory, reinterpret_cast<LPVOID*>(&oWriteProcessMemory)) != MH_OK)
        {
            return FALSE;
        }

        if (MH_EnableHook(&WriteProcessMemory) != MH_OK) return FALSE;
 
        break;
    case DLL_PROCESS_DETACH:
        // cleanup
        MH_DisableHook(&WriteProcessMemory);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}