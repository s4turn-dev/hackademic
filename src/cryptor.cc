#include "cryptor.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <openssl/rand.h>
#include <openssl/evp.h>

// CST & DST

AES256CBC::AES256CBC() {
    evp_aes256cbc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
    key = new unsigned char[keySize];
    generateKey();
}

AES256CBC::AES256CBC(unsigned char* existingKey) {
    evp_aes256cbc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
    key = new unsigned char[keySize];
    std::memcpy(key, existingKey, keySize);
}

AES256CBC::~AES256CBC() {
    EVP_CIPHER_free(evp_aes256cbc);
    delete[] key;
}


// Methods

/*bool AES256CBC::decryptFile(const std::filesystem::path path) {
    return false;
}

void AES256CBC::decryptRecursively(const std::string path) {
    return false;
}*/

bool AES256CBC::encryptFile(const std::filesystem::path pathIn) {
    std::cout << "[#] Encrypting " << pathIn << "...\n";
    std::string pathOut = (std::string)pathIn + extension;
    std::ifstream fin(pathIn, std::ios::binary);  // No need for .close() because ifstream and ofstream
    std::ofstream fout(pathOut, std::ios::binary);// automatically close themselves when they go out of scope
    if (!fin or !fout) {
        std::cerr << " └─[☓] Error opening file.\n";
        return false;
    }
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize];
    RAND_bytes(iv, keySize);
    if ( !ctx or !(*iv) or !EVP_EncryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        std::cerr << " └─[☓] Error initializing encryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    fout.write((char*)iv, keySize);

    unsigned char buffer[1024];
    unsigned char encryptedBuffer[1024 + blockSize];
    int bytesRead, encryptedLen;

    while ((bytesRead = fin.readsome((char*)buffer, sizeof(buffer))) > 0) {
        EVP_EncryptUpdate(ctx, encryptedBuffer, &encryptedLen, buffer, bytesRead);
        fout.write((char*)encryptedBuffer, encryptedLen);
    }

    EVP_EncryptFinal_ex(ctx, encryptedBuffer, &encryptedLen);
    fout.write((char*)encryptedBuffer, encryptedLen);
    // fin.selfDestruct();

    EVP_CIPHER_CTX_free(ctx);
    std::cout << " └─[✓] Done.\n";
    return true;
}

/*void AES256CBC::encryptRecursively(const std::string path) {
    ;
}*/

void AES256CBC::generateKey() {
    RAND_bytes(key, keySize);
    std::cout << "[*] Generated key " << key << "\n";
}

