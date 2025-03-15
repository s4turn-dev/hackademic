#include <iostream>
#include <fstream>


class _string {
private:
    std::string data;
public:
    _string(const std::string& str) : data(str) {}
    char operator[](int index) const { return data[index % data.length()]; }
};


void encrypt(const char *filenameIn, const char *filenameOut, const _string key) {
    std::fstream fin, fout;

    fin.open(filenameIn, std::fstream::in);
    fout.open(filenameOut, std::fstream::out);
    char c;
    for (int i = 0; fin >> std::noskipws >> c; i++) {
        fout << (char)(c ^ key[i]);  // XOR 
                                     // 1000101
                                     // 1110010
                                     // 0110111
    }
    fin.close();
    fout.close();
}

void derive_key(const char *filenameEncrypted, const char *filenameOriginal) {
    std::fstream fe, fo;

    fe.open(filenameEncrypted, std::fstream::in);
    fo.open(filenameOriginal, std::fstream::in);
    std::cout << "DERIVED KEY: ";
    for(char c1, c2; fe >> std::noskipws >> c1, fo >> std::noskipws >>c2; ) {
        std::cout << (char)(c1 ^ c2);
    }
    std::cout << std::endl;
    fe.close();
    fo.close();
}


int main() {
    _string KEY("fa22ae097a8d2dab25e280e5a41aeafd");
    encrypt("dummy.txt", "weak_encrypted.txt", KEY);
    encrypt("weak_encrypted.txt", "weak_decrypted.txt", KEY);
    derive_key("weak_encrypted.txt", "dummy.txt");

    return 0;
}

