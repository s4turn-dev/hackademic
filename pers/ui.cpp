#include <iostream>
#include <windows.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

std::string getDiskSerial() {
    DWORD serialNum = 0;
    GetVolumeInformation("C:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
    return std::to_string(serialNum);
}

void createFile() {
    std::string filename = "C:\\Windows\\SysWOW64\\" + getDiskSerial() + ".txt";
    std::ofstream file(filename); // Просто создаём файл без записи
    if (file) {
        std::cout << "File: " << filename << std::endl;
    }
}
using namespace std;
int main() {
    int key = 123;
    int inkey;
    while (key != inkey) {
        cout << "Enter key: ";
        cin >> inkey;
    }
    createFile();
    Sleep(5000);
    return 0;
}
