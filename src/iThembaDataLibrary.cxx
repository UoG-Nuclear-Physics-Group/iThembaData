#include <string>

#include "TClassRef.h"

#include "TTdrFile.h"
#include "TTdrDataParser.h"
#include "iThembaDataVersion.h"
#include "TChannel.h"
#include "TTdrMnemonic.h"

extern "C" TTdrFile* CreateFile(std::string& fileName) { return new TTdrFile(fileName.c_str()); }
extern "C" void DestroyFile(TTdrFile* obj) { delete obj; }

extern "C" TTdrDataParser* CreateParser() { return new TTdrDataParser; }
extern "C" void DestroyParser(TTdrDataParser* obj) { delete obj; }

extern "C" std::string LibraryVersion() { return std::string(ITHEMBADATA_RELEASE); }

extern "C" void InitLibrary() { TChannel::SetMnemonicClass(TClassRef("TTdrMnemonic")); }
