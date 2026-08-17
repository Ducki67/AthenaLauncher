#pragma once
#pragma once
#include <windows.h>
#include <string>

// basic af

namespace FortAthena::Injector {
    inline bool InjectRedirectDLL(DWORD processId, const std::string& dllPath) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (!hProcess) return false;

        LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pDllPath) { CloseHandle(hProcess); return false; }

        if (!WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), dllPath.length() + 1, nullptr)) {
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        LPVOID pLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllPath, 0, nullptr);

        if (!hThread) {
            VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return false;
        }

        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return true;
    }
}
