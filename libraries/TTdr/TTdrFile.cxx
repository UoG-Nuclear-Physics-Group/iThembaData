#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cassert>
#include <cstdlib>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include "TString.h"

#include "TTdrFile.h"
#include "TTdrEvent.h"
#include "TRunInfo.h"
#include "TTdrDetectorInformation.h"
#include "TTdrMnemonic.h"
#include "iThembaDataVersion.h"

TTdrFile::TTdrFile(const char* filename, TRawFile::EOpenType open_type) : TTdrFile()
{
   switch(open_type) {
   case TRawFile::EOpenType::kRead: Open(filename); break;

   case TRawFile::EOpenType::kWrite: break;
   }
}

TTdrFile::~TTdrFile()
{
   // Default dtor. It closes the read in midas file as well as the output midas file.
   Close();
}

std::string TTdrFile::Status(bool)
{
   return Form(HIDE_CURSOR " Processed event, have processed %.2fMB/%.2f MB              " SHOW_CURSOR "\r",
               (BytesRead() / 1000000.0), (FileSize() / 1000000.0));
}

/// Open a tdr file with given file name.
///
/// \param[in] filename The file to open.
/// \returns "true" for succes, "false" for error, use GetLastError() to see why
bool TTdrFile::Open(const char* filename)
{
   Filename(filename);
   try {
      fInputFile.open(GetFilename(), std::ifstream::in | std::ifstream::binary);
      fInputFile.seekg(0, std::ifstream::end);
      if(fInputFile.tellg() < 0) {
         std::cout << R"(Failed to open ")" << GetFilename() << "/" << Filename() << R"("!)" << std::endl;
         return false;
      }
      FileSize(fInputFile.tellg());
      fInputFile.seekg(0, std::ifstream::beg);
      ResizeBuffer(0x10000);
   } catch(std::exception& e) {
      std::cout << "Caught " << e.what() << std::endl;
   }
   // Do we need these?
   // signal(SIGPIPE,SIG_IGN); // crash if reading from closed pipe
   // signal(SIGXFSZ,SIG_IGN); // crash if reading from file >2GB without O_LARGEFILE

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

   // setup TChannel to use our mnemonics
   TChannel::SetMnemonicClass(TTdrMnemonic::Class());

   TRunInfo::SetRunInfo(GetRunNumber(), GetSubRunNumber());
   TRunInfo::ClearVersion();
   TRunInfo::SetVersion(ITHEMBADATA_RELEASE);

   std::cout << "Successfully opened file with " << FileSize() << " bytes!" << std::endl;

   //std::cout<<std::hex<<std::setfill('0');
   //for(size_t i = 0; i < fReadBuffer.size() && i < 256; ++i) {
   //	std::cout<<std::setw(2)<<(static_cast<int16_t>(fReadBuffer[i])&0xff)<<" ";
   //	if(i%16 == 15) std::cout<<std::endl;
   //}
   //std::cout<<std::dec<<std::setfill(' ');

   TTdrDetectorInformation* detInfo = new TTdrDetectorInformation();
   TRunInfo::SetDetectorInformation(detInfo);

   return true;
}

void TTdrFile::Close()
{
}

/// \param [in] tdrEvent Pointer to an empty TTdrEvent
/// \returns "true" for success, "false" for failure, see GetLastError() to see why
///
///  EDITED FROM THE ORIGINAL TO RETURN TOTAL SUCESSFULLY BYTES READ INSTEAD OF TRUE/FALSE,  PCB
///
int TTdrFile::Read(std::shared_ptr<TRawEvent> tdrEvent)
{
   if(!fInputFile.is_open()) {
      return 0;
   }
   // try to read next 64k buffer
   fInputFile.read(BufferData(), 0x10000);
   if(!fInputFile.good()) {
      std::cout << "Failed to read next 64k buffer, currently at " << BytesRead() << "/" << FileSize() << std::endl;
      fInputFile.close();
      return 0;
   }
   //read 24 byte header
   if(strncmp(BufferData(), "EBYEDATA", 8) != 0) {
      std::cerr << BytesRead() << ": Failed to find 'EBYEDATA' (or 0x45 42 59 45 44 41 54 41) at beginning of header (0x" << std::hex << std::setfill('0') << std::setw(8) << *reinterpret_cast<uint64_t*>(BufferData()) << std::hex << std::setfill(' ') << ")" << std::endl;
      return 0;
   }
   try {
      std::static_pointer_cast<TTdrEvent>(tdrEvent)->SetHeader(BufferData());
   } catch(std::exception& e) {
      std::cout << e.what() << std::endl;
   }
   uint32_t dataSize = std::static_pointer_cast<TTdrEvent>(tdrEvent)->GetHeader().fDataLength;
   if(24 + dataSize < 0x10000) {
      try {
         std::static_pointer_cast<TTdrEvent>(tdrEvent)->SetData(std::vector<char>(ReadBuffer().begin() + 24, ReadBuffer().begin() + 24 + dataSize));
      } catch(std::exception& e) {
         std::cout << e.what() << std::endl;
      }
      BytesRead(fInputFile.tellg());
      if(BytesRead() == FileSize()) {
         fInputFile.close();
      }
      return 0x10000;
   }
   return 0;
}

void TTdrFile::Skip(size_t nofEvents)
{
   if(!fInputFile.is_open()) {
      std::cerr << __PRETTY_FUNCTION__ << ": input file is not open!" << std::endl;
      return;
   }
   // try to skip next nofEvents 64k buffer
   fInputFile.seekg(nofEvents * 0x10000, std::ifstream::cur);
   if(!fInputFile.good()) {
      std::cout << "Failed to skip next " << nofEvents << " 64k buffer(s), currently at " << BytesRead() << "/" << FileSize() << std::endl;
      fInputFile.close();
   }
}

int TTdrFile::GetRunNumber()
{
   // Parse the run number from the current TMidasFile. This assumes a format of
   // R#*_#* or R#* (#* denoting one or more digits).
   if(Filename().length() == 0) {
      return 0;
   }
   std::size_t foundslash = Filename().rfind('/');
   std::size_t found      = Filename().rfind('R');
   if(found < foundslash || found == std::string::npos) {
      std::cout << "Warning, failed to find 'R' in filename '" << Filename() << "'!" << std::endl;
      return 0;
   }
   std::size_t found2 = Filename().rfind('-');
   if((found2 < foundslash && foundslash != std::string::npos) || found2 == std::string::npos) {
      found2 = Filename().rfind('_');
   }
   //   printf("found 2 = %i\n",found2);
   if(found2 < foundslash && foundslash != std::string::npos) {
      found2 = std::string::npos;
   }
   std::string temp;
   if(found2 == std::string::npos) {
      // no subrun number found, use rest of filename
      temp = Filename().substr(found + 1);
   } else {
      // subrun number found, use everything between 'R' and '_'/'-'
      temp = Filename().substr(found + 1, found2 - (found + 1));
   }
   return atoi(temp.c_str());
}

int TTdrFile::GetSubRunNumber()
{
   // Parse the sub run number from the current TMidasFile. This assumes a format of
   // R#*_#* or R#* (#* denoting one or more digits).
   if(Filename().length() == 0) {
      return -1;
   }
   std::size_t foundslash = Filename().rfind('/');
   std::size_t found      = Filename().rfind('-');
   if((found < foundslash && foundslash != std::string::npos) || found == std::string::npos) {
      found = Filename().rfind('_');
   }
   if(found < foundslash && foundslash != std::string::npos) {
      found = std::string::npos;
   }
   if(found != std::string::npos) {
      std::string temp = Filename().substr(found + 1);
      return atoi(temp.c_str());
   }
   return -1;
}

// end
