#include "TTdrDetectorInformation.h"

#include <iostream>

#include "TROOT.h"

#include "TGRSIOptions.h"
#include "TTdrMnemonic.h"

/// \cond CLASSIMP
ClassImp(TTdrDetectorInformation)
/// \endcond

TTdrDetectorInformation::TTdrDetectorInformation() : TDetectorInformation()
{
   /// Default ctor for TTdrDetectorInformation.
   Clear();
	Set();
}

TTdrDetectorInformation::~TTdrDetectorInformation() = default;

TEventBuildingLoop::EBuildMode TTdrDetectorInformation::BuildMode() const
{
	return TEventBuildingLoop::EBuildMode::kTimestamp;
}

void TTdrDetectorInformation::Print(Option_t* opt) const
{
   // Prints the TTdrDetectorInformation. Options:
   // a: Print out more details.
   if(strchr(opt, 'a') != nullptr) {
		printf("\t\tTDRCLOVER:          %s\n", TdrClover() ? "true" : "false");
		printf("\t\tTDRTIGRESS:         %s\n", TdrTigress() ? "true" : "false");
		printf("\t\tTDRSILI:            %s\n", TdrSiLi() ? "true" : "false");
		printf("\t\tTDRPLASTIC:         %s\n", TdrPlastic() ? "true" : "false");
		printf("\n");
	}
}

void TTdrDetectorInformation::Clear(Option_t*)
{
	// Clears the TTdrDetectorInformation. Currently, there are no available
	// options.

	TDetectorInformation::Clear();

	fTdrClover  = false;
	fTdrTigress = false;
	fTdrSiLi    = false;
	fTdrPlastic = false;
}

void TTdrDetectorInformation::Set()
{
	/// Sets the run info. This figures out what systems are available.
	for(auto iter = TChannel::GetChannelMap()->begin(); iter != TChannel::GetChannelMap()->end(); iter++) {
		std::string channelname = iter->second->GetName();

		//  detector system type.
		//  for more info, see: https://www.triumf.info/wiki/tigwiki/index.php/Detector_Nomenclature
		switch(static_cast<const TTdrMnemonic*>(iter->second->GetMnemonic())->System()) {
			case TTdrMnemonic::ESystem::kTdrClover:
				SetTdrClover();
				break;
			case TTdrMnemonic::ESystem::kTdrTigress:
				SetTdrTigress();
				break;
			case TTdrMnemonic::ESystem::kTdrSiLi:
				SetTdrSiLi();
				break;
			case TTdrMnemonic::ESystem::kTdrPlastic:
				SetTdrPlastic();
				break;
			default:
				break;
		};
	}
}

