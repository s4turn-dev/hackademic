#include <filesystem>
#include <openssl/evp.h>
#include <string>

class AES256CBC {
    public:
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

