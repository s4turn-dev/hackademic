#include "cryptor.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    std::string option;
    while (option != "e" and option != "d") {
        std::cout << "[/] Enter 'e' for encryption or 'd' for decryption: ";
        std::getline(std::cin, option);
    }

    std::string path;
    /*
    if (argc == 1) {
        std::cout << "[/] Enter a path (./dummy/): ";
        std::getline(std::cin, path);
        if (path == "")
            path = "./dummy/";
    } else
        path = argv[1];
    */path = "./dummy/"; argc=argc; argv=argv;
    
    std::cout << "[i] Working within  " << path << ".\n";
    AES256CBC AES;
    //AES.isLogging = false;

    // TODO : the following code looks ugly
    if (option == "e") {
        AES.generateKey();
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
            std::string filename = entry.path();
            if (!std::filesystem::is_directory(filename))
                AES.encryptFile(filename);
        }
    } else {
        AES.keyFromFile();
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
            std::string filename = entry.path();
            if (!std::filesystem::is_directory(filename))
                AES.decryptFile(filename);
        }
    }
    // end of ugly

    return 0;
}

