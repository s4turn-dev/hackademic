#include "cryptor.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <openssl/evp.h>
#include <openssl/rand.h>

#define BUFFER_SIZE 1024

// CST & DST

AES256CBC::AES256CBC() {
    evp_aes256cbc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
    key = new unsigned char[keySize_];
}

AES256CBC::~AES256CBC() {
    EVP_CIPHER_free(evp_aes256cbc);
    delete[] key;
}


// Methods

bool AES256CBC::decryptFile(std::filesystem::path path) {
    cout() << "[#] Decrypting " << path << "...\n";
    if (path.extension() != extension_) {
        cout() << " └─ Nothing to do.\n";
        return false;
    }
    std::ifstream fin(path, std::ios::binary);
    std::ofstream fout(path.replace_extension(), std::ios::binary);
    // IMPORTANT: path does not have the extension from this point!
    if (!fin or !fout) {
        cerr() << " └─[☓] Error opening file.\n";
        return false;
    }
     
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    fin.readsome((char*)iv, keySize_);
    if ( !ctx or !EVP_DecryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << " └─[☓] Error initializing encryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    unsigned char buffer[BUFFER_SIZE];
    unsigned char decryptedBuffer[BUFFER_SIZE];
    int bytesRead, decryptedLen;

    while ((bytesRead = fin.readsome((char*)buffer, BUFFER_SIZE)) > 0) {
        EVP_DecryptUpdate(ctx, decryptedBuffer, &decryptedLen, buffer, bytesRead);
        fout.write((char*)decryptedBuffer, decryptedLen);
    }

    EVP_DecryptFinal_ex(ctx, decryptedBuffer, &decryptedLen);
    fout.write((char*)decryptedBuffer, decryptedLen);

    EVP_CIPHER_CTX_free(ctx);
    path += extension_;  // TODO: consider a way to make it look less stupid
                        // Maybe also unify the handling with the encryption
                        // function (also make it use one "path" and have it
                        // appended)
    std::filesystem::remove(path);
    cout() << " └─[✓] Done.\n";
    return true;
}

void AES256CBC::decryptRecursively(const std::filesystem::path& path) {
    //if (!key)  // Do we need this architecture-wise?
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Error: not a directory: " << path << "\n";
        return;
    }
    keyFromFile();
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::filesystem::path filename = entry.path();
        if (filename.extension() == extension_ and std::filesystem::is_regular_file(entry))
            decryptFile(filename);
    }
}

bool AES256CBC::encryptFile(const std::filesystem::path& pathIn) {
    cout() << "[#] Encrypting " << pathIn << "...\n";
    std::string pathOut = (std::string)pathIn + extension_;
    std::ifstream fin(pathIn, std::ios::binary);
    std::ofstream fout(pathOut, std::ios::binary);
    if (!fin or !fout) {
        cerr() << " └─[☓] Error opening file.\n";
        return false;
    }
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    RAND_bytes(iv, keySize_);
    if ( !ctx or !(*iv) or !EVP_EncryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << " └─[☓] Error initializing encryption.\n";
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    fout.write((char*)iv, keySize_);

    unsigned char buffer[BUFFER_SIZE];
    unsigned char encryptedBuffer[BUFFER_SIZE];
    int bytesRead, encryptedLen;

    while ((bytesRead = fin.readsome((char*)buffer, BUFFER_SIZE)) > 0) {
        EVP_EncryptUpdate(ctx, encryptedBuffer, &encryptedLen, buffer, bytesRead);
        fout.write((char*)encryptedBuffer, encryptedLen);
    }

    EVP_EncryptFinal_ex(ctx, encryptedBuffer, &encryptedLen);
    fout.write((char*)encryptedBuffer, encryptedLen);
    std::filesystem::remove(pathIn);

    EVP_CIPHER_CTX_free(ctx);
    cout() << " └─[✓] Done.\n";
    return true;
}

void AES256CBC::encryptRecursively(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Error: not a directory: " << path << "\n";
        return;
    }
    //if (!key)  // Do we need this architecture-wise?
    generateKey();
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::filesystem::path filename = entry.path();
        if (/*entry.extension() != extension_ and*/ std::filesystem::is_regular_file(entry))
            encryptFile(filename);
    }
}

void AES256CBC::generateKey() {
    RAND_bytes(key, keySize_);
    cout() << "[i] Generated key.\n";
    keyToFile();
}

void AES256CBC::keyFromFile() {
    std::string keyPath;
    std::cout << "[/] Enter a path where to read the key from (./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ifstream fin(keyPath, std::ios::binary);
    if (!fin)
        cerr() << " └─[☓] Error opening file.\n";
    else {
        fin.readsome((char*)key, keySize_);
        cout() << "[i] Read the key from:  " << keyPath << ".\n";
    }
}

void AES256CBC::keyToFile() {
    std::string keyPath;
    std::cout << "[/] Enter a path where to write the key into (default=./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ofstream fout(keyPath, std::ios::binary);
    if (!fout)
        cerr() << " └─[☓] Error opening file.\n";
    else {
        fout.write((char*)key, keySize_);
        cout() << "[i] Wrote the key to:  " << keyPath << ".\n";
    }
}

