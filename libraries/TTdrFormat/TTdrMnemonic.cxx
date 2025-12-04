#include "TTdrMnemonic.h"

#include <algorithm>

// Detector dependent includes
#include "TTdrClover.h"
#include "TTdrTigress.h"
#include "TTdrSiLi.h"
#include "TTdrPlastic.h"
#include "TTdrCloverBgo.h"
#include "TTdrTigressBgo.h"

void TTdrMnemonic::Clear(Option_t*)
{
   TMnemonic::Clear();
   fSystem = ESystem::kClear;
}

void TTdrMnemonic::EnumerateSystem()
{
   // Enumerating the fSystemString must come after the total mnemonic has been parsed as the details of other parts of
   // the mnemonic must be known
   if(SystemString() == "CL") {
      if(SubSystem() == EMnemonic::kS) {
         fSystem = ESystem::kTdrCloverBgo;
      } else {
         fSystem = ESystem::kTdrClover;
      }
   } else if(SystemString() == "TT") {
      if(SubSystem() == EMnemonic::kS) {
         fSystem = ESystem::kTdrTigressBgo;
      } else {
         fSystem = ESystem::kTdrTigress;
      }
   } else if(SystemString() == "SI") {
      fSystem = ESystem::kTdrSiLi;
   } else if(SystemString() == "PL") {
      fSystem = ESystem::kTdrPlastic;
   } else {
      fSystem = ESystem::kClear;
   }
}

void TTdrMnemonic::EnumerateDigitizer(TPriorityValue<std::string>& digitizerName, TPriorityValue<EDigitizer>& digitizerType, TPriorityValue<int>& timeStampUnit)
{
   std::string name = digitizerName.Value();
   std::transform(name.begin(), name.end(), name.begin(), ::toupper);
   EDigitizer tmpType = EDigitizer::kDefault;
   int        tmpUnit = 10;
   if(name.compare("PIXIE") == 0) {
      tmpType = EDigitizer::kPixie;
   } else if(name.compare("FASTPIXIE") == 0) {
      tmpType = EDigitizer::kFastPixie;
   } else if(name.compare("PIXIETAPEMOVE") == 0) {
      tmpType = EDigitizer::kPixieTapeMove;
   } else {
      std::cout << "Warning, digitizer type '" << name << "' not recognized, options are 'GRF16', 'GRF4G', 'TIG10', 'TIG64', and 'CAEN'!" << std::endl;
   }
   digitizerType.Set(tmpType, digitizerName.Priority());
   timeStampUnit.Set(tmpUnit, digitizerName.Priority());
}

void TTdrMnemonic::Parse(std::string* name)
{
   if((name == nullptr) || name->length() < 9) {
      return;
   }
   TMnemonic::Parse(name);
   // Enumerating the fSystemString must come last as the details of other parts of
   // the mnemonic must be known
   EnumerateSystem();

   return;
}

void TTdrMnemonic::Print(Option_t*) const
{
   std::ostringstream str;
   std::cout << "====== TdrMNEMONIC ======" << std::endl;
   TMnemonic::Print(str);
   std::cout << "=========================" << std::endl;
}

TClass* TTdrMnemonic::GetClassType() const
{
   if(TMnemonic::GetClassType() != nullptr) {
      return TMnemonic::GetClassType();
   }

   switch(System()) {
   case ESystem::kTdrCloverBgo: SetClassType(TTdrCloverBgo::Class()); break;
   case ESystem::kTdrTigressBgo: SetClassType(TTdrTigressBgo::Class()); break;
   case ESystem::kTdrClover: SetClassType(TTdrClover::Class()); break;
   case ESystem::kTdrTigress: SetClassType(TTdrTigress::Class()); break;
   case ESystem::kTdrSiLi: SetClassType(TTdrSiLi::Class()); break;
   case ESystem::kTdrPlastic: SetClassType(TTdrPlastic::Class()); break;
   default: SetClassType(nullptr);
   };
   return TMnemonic::GetClassType();
}

double TTdrMnemonic::GetTime(Long64_t timestamp, Float_t cfd, double energy, const TChannel* channel) const
{
   if(channel == nullptr) {
      Error("GetTime", "No TChannel provided");
      return static_cast<Double_t>(timestamp + gRandom->Uniform());
   }
   switch(static_cast<EDigitizer>(channel->GetDigitizerType())) {
      Double_t dTime;
   case EDigitizer::kPixie:
      dTime = timestamp * channel->GetTimeStampUnit() + channel->CalibrateCFD(cfd / 3276.8);   // CFD is reported as 15bit interpolation of 10 ns
      return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
   case EDigitizer::kFastPixie:
      dTime = timestamp * channel->GetTimeStampUnit() + channel->CalibrateCFD(cfd / 6553.6);   // CFD is reported as 16bit interpolation of 10 ns
      return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
   default:
      dTime = static_cast<Double_t>((timestamp * channel->GetTimeStampUnit()) + gRandom->Uniform());
      return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
   }
   return 0.;
}
