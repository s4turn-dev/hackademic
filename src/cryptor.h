#include <filesystem>
#include <iostream>
#include <openssl/evp.h>
#include <string>

class AES256CBC {
    public:
        bool isLogging = true;
        std::ostream nullStream{nullptr};

        std::ostream& cout() { return isLogging ? std::cout : nullStream; };
        std::ostream& cerr() { return isLogging ? std::cerr : nullStream; };
        bool decryptFile(std::filesystem::path path);
        bool decryptRecursively(const std::string path);
        bool encryptFile(const std::filesystem::path pathIn);
        bool encryptRecursively(const std::string path);
        void keyFromFile();
        void generateKey();
        void keyToFile();

        AES256CBC();
        AES256CBC(unsigned char* key);
        ~AES256CBC();

    protected:
        const int keySize = 32;
        const int blockSize = 16;
        const std::string extension = ".hackademic";
        unsigned char* key;
        EVP_CIPHER* evp_aes256cbc;

    private:
        // ...
};

