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
    */path = "C:/dummy/"; argc=argc; argv=argv;
    
    std::cout << "[i] Working within " << path << "\n";
    AES256CBC AES;
    //AES.isLogging = false;;

    if (option == "e")
        AES.encryptRecursively(path);
    else
        AES.decryptRecursively(path);

    return 0;
}

