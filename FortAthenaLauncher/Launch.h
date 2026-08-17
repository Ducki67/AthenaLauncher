#pragma once
#include <windows.h>
#include <string>
#include "Injector.h"
#include "Configure.h"

namespace FortAthena::Launch {
    inline std::string GetLocalDllPath() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos) + "\\Tellurium.dll";
    }

    inline bool LaunchFortniteWithRedirect(const std::string& path, const std::string& email, const std::string& password) {
        std::string exePath = path + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";

        
        std::string arguments = " -AUTH_LOGIN=" + email +
            " -AUTH_PASSWORD=" + password +
            " -AUTH_TYPE=epic -epicapp=Fortnite -epicenv=Prod -epiclocale=en -epicplatform=WIN -plat=WIN -fltoken=f7d8a9b2c3d4 -fromfl=be -noeac -nobattleye -eac_launcher_is_not_required -epicportal";

        if (Configure::bUseArgs) {
            arguments += " " + std::string(Configure::AdditionalArgs);
        }

        
        std::string fullCommand = "\"" + exePath + "\"" + arguments;
        std::string dllPath = GetLocalDllPath();

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        
        bool bCreated = CreateProcessA(
            exePath.c_str(),
            const_cast<char*>(fullCommand.c_str()), 
            nullptr,
            nullptr,
            FALSE,
            CREATE_SUSPENDED,
            nullptr,
            nullptr,
            &si,
            &pi
        );

        if (bCreated) {
           
            if (GetFileAttributesA(dllPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                FortAthena::Injector::InjectRedirectDLL(pi.dwProcessId, dllPath);
            }

            
            ResumeThread(pi.hThread);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return true;
        }
        return false;
    }
}
