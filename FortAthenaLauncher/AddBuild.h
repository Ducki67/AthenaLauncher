#pragma once
#include <windows.h>
#include <shlobj.h> //for (SHBrowseForFolderA)
#include <string>

#pragma comment(lib, "Shell32.lib")

namespace FortAthena::Build {
    inline bool BrowseFortniteBuild(char* outputBuffer, DWORD bufferSize) {
        BROWSEINFOA bi = { 0 };
        bi.lpszTitle = "Select the your Build's folder (where Engine and FortniteGame are located):";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

        LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
        if (pidl != nullptr) {
            char path[MAX_PATH];
            
            if (SHGetPathFromIDListA(pidl, path)) {
                
                strcpy_s(outputBuffer, bufferSize, path);

                
                CoTaskMemFree(pidl);
                return true;
            }
            CoTaskMemFree(pidl);
        }
        return false;
    }
}
