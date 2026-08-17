#pragma once
#include <windows.h>
#include <fstream>
#include <string>

#pragma comment(lib, "urlmon.lib")


namespace FortAthena {
    namespace Configure {
        inline char GamePath[MAX_PATH] = "Fortnite path...";
        inline char Email[128] = "ExamplePlayer@Athena.dev";
        inline char Password[128] = "ExamplePassword";


        inline bool bUseArgs = false;
        inline char AdditionalArgs[256] = "-log -nosplash -Borderless -ResX=1200 -ResY=770"; // do here wtv or just use it from the ui

        inline void SaveConfig() {
            std::ofstream file("config.txt");
            if (file.is_open()) {
                file << GamePath << "\n";
                file << Email << "\n";
                file << Password << "\n";
                file << (bUseArgs ? "1" : "0") << "\n";
                file << AdditionalArgs << "\n";
                file.close();
            }
        }

        inline void DownloadRedirect()
        {
            char buffer[MAX_PATH];
            GetModuleFileNameA(NULL, buffer, MAX_PATH);
            std::string::size_type pos = std::string(buffer).find_last_of("\\/");
            std::string localDllPath = std::string(buffer).substr(0, pos) + "\\Tellurium.dll"; // repalce this with your dll name

            
            if (GetFileAttributesA(localDllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                // replace this with your dll download
                URLDownloadToFileA(NULL, "https://r2.ploosh.dev/Tellurium.dll", localDllPath.c_str(), 0, NULL);
            }
        }

        inline void LoadConfig() {
            std::ifstream file("config.txt");
            if (file.is_open()) {
                std::string line;
                if (std::getline(file, line)) strcpy_s(GamePath, sizeof(GamePath), line.c_str());
                if (std::getline(file, line)) strcpy_s(Email, sizeof(Email), line.c_str());
                if (std::getline(file, line)) strcpy_s(Password, sizeof(Password), line.c_str());

                if (std::getline(file, line)) bUseArgs = (line == "1");
                if (std::getline(file, line)) strcpy_s(AdditionalArgs, sizeof(AdditionalArgs), line.c_str());
                file.close();
            }
        }
    }
}
