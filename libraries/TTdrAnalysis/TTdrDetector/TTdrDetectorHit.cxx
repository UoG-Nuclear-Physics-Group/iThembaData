#include "TTdrDetectorHit.h"

#include "TChannel.h"
#include "TTdrMnemonic.h"

/// \cond CLASSIMP
ClassImp(TTdrDetectorHit)
/// \endcond

TTdrDetectorHit::TTdrDetectorHit(const int& Address) : TDetectorHit()
{
   /// Default constructor
   Clear();
   fAddress = Address;

#if MAJOR_ROOT_VERSION < 6
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
}

TTdrDetectorHit::TTdrDetectorHit(const TTdrDetectorHit& rhs, bool copywave) : TDetectorHit(rhs)
{
   /// Default Copy constructor
   rhs.Copy(*this);
   if(copywave) {
      rhs.CopyWave(*this);
   }
   ClearTransients();

#if MAJOR_ROOT_VERSION < 6
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
}

TTdrDetectorHit::~TTdrDetectorHit()
{
   // Default destructor
}

Double_t TTdrDetectorHit::GetTime(const ETimeFlag&, Option_t*) const
{
   if(IsTimeSet()) {
      return fTime;
   }

   TChannel* channel = GetChannel();
   if(channel == nullptr) {
      Error("GetTime", "No TChannel exists for address 0x%08x", GetAddress());
      return SetTime(10. * (static_cast<Double_t>((GetTimeStamp()) + gRandom->Uniform())));
   }
   switch(static_cast<EDigitizer>(channel->GetDigitizerType())) {
		Double_t dTime;
		case EDigitizer::kPixie:
		dTime = GetTimeStamp() * 10. + channel->CalibrateCFD(fCfd/3276.8);// CFD is reported as 15bit interpolation of 10 ns
		return SetTime(dTime - 10. * (channel->GetTZero(GetEnergy())));
		case EDigitizer::kFastPixie:
		dTime = GetTimeStamp() * 10. + channel->CalibrateCFD(fCfd/6553.6);// CFD is reported as 16bit interpolation of 10 ns
		return SetTime(dTime - 10. * (channel->GetTZero(GetEnergy())));
		default:
		dTime = static_cast<Double_t>((GetTimeStamp()) + gRandom->Uniform());
		return SetTime(10. * (dTime - channel->GetTZero(GetEnergy())));
	}
   return 0.;
}
