#include <string>

#include "TTdrFile.h"
#include "TTdrDataParser.h"
#include "iThembaDataVersion.h"

extern "C" TTdrFile* CreateFile(std::string& fileName) { return new TTdrFile(fileName.c_str()); }
extern "C" void DestroyFile(TTdrFile* obj) { delete obj; }

extern "C" TTdrDataParser* CreateParser() { return new TTdrDataParser; }
extern "C" void DestroyParser(TTdrDataParser* obj) { delete obj; }

extern "C" std::string LibraryVersion() { return std::string(ITHEMBADATA_RELEASE); }
