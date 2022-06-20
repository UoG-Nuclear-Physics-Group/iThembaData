#include "TTdrMnemonic.h"

#include <algorithm>

// Detector dependent includes
#include "TTdrClover.h"
#include "TTdrTigress.h"
#include "TTdrSiLi.h"
#include "TTdrPlastic.h"
#include "TTdrCloverBgo.h"
#include "TTdrTigressBgo.h"

ClassImp(TTdrMnemonic)

void TTdrMnemonic::Clear(Option_t*)
{
	TMnemonic::Clear();
   fSystem = ESystem::kClear;
}

void TTdrMnemonic::EnumerateSystem()
{
   // Enumerating the fSystemString must come after the total mnemonic has been parsed as the details of other parts of
   // the mnemonic must be known
	if(fSystemString.compare("CL") == 0) {
		if(SubSystem() == EMnemonic::kS) {
			fSystem = ESystem::kTdrCloverBgo;
		} else {
			fSystem = ESystem::kTdrClover;
		}
	} else if(fSystemString.compare("TT") == 0) {
		if(SubSystem() == EMnemonic::kS) {
			fSystem = ESystem::kTdrTigressBgo;
		} else {
			fSystem = ESystem::kTdrTigress;
		}
	} else if(fSystemString.compare("SI") == 0) {
		fSystem = ESystem::kTdrSiLi;
	} else if(fSystemString.compare("PL") == 0) {
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
	int tmpUnit = 10;
	if(name.compare("PIXIE") == 0) {
		tmpType = EDigitizer::kPixie;
	} else if(name.compare("FASTPIXIE") == 0) {
		tmpType = EDigitizer::kFastPixie;
	} else if(name.compare("PIXIETAPEMOVE") == 0) {
		tmpType = EDigitizer::kPixieTapeMove;
	} else {
		std::cout<<"Warning, digitizer type '"<<name<<"' not recognized, options are 'GRF16', 'GRF4G', 'TIG10', 'TIG64', and 'CAEN'!"<<std::endl;
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
	printf("====== TdrMNEMONIC ======\n");
	printf("fArrayPosition           = %i\n", fArrayPosition);
	printf("fSegment                 = %i\n", fSegment);
	printf("fSystemString            = %s\n", fSystemString.c_str());
	printf("fSubSystemString         = %s\n", fSubSystemString.c_str());
	printf("fArraySubPositionString  = %s\n", fArraySubPositionString.c_str());
	printf("fCollectedChargeString   = %s\n", fCollectedChargeString.c_str());
	printf("fOutputSensorString      = %s\n", fOutputSensorString.c_str());
	printf("===============================\n");
}

TClass* TTdrMnemonic::GetClassType() const
{
	if(fClassType != nullptr) {
		return fClassType;
	}

	switch(System()) {
		case ESystem::kTdrCloverBgo:  fClassType = TTdrCloverBgo::Class(); break;
		case ESystem::kTdrTigressBgo: fClassType = TTdrTigressBgo::Class(); break;
		case ESystem::kTdrClover:     fClassType = TTdrClover::Class(); break;
		case ESystem::kTdrTigress:    fClassType = TTdrTigress::Class(); break;
		case ESystem::kTdrSiLi:       fClassType = TTdrSiLi::Class(); break;
		case ESystem::kTdrPlastic:    fClassType = TTdrPlastic::Class(); break;
		default:                      fClassType = nullptr;
	};
	return fClassType;
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
		dTime = timestamp * channel->GetTimeStampUnit() + channel->CalibrateCFD(cfd/3276.8);// CFD is reported as 15bit interpolation of 10 ns
		return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
		case EDigitizer::kFastPixie:
		dTime = timestamp * channel->GetTimeStampUnit() + channel->CalibrateCFD(cfd/6553.6);// CFD is reported as 16bit interpolation of 10 ns
		return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
		default:
		dTime = static_cast<Double_t>((timestamp * channel->GetTimeStampUnit()) + gRandom->Uniform());
		return dTime - channel->GetTZero(energy) - channel->GetTimeOffset();
	}
   return 0.;
}
