#include "TTdrSiLi.h"
#include "TTdrSiLiHit.h"
#include "Globals.h"

/// \cond CLASSIMP
ClassImp(TTdrSiLiHit)
/// \endcond

TTdrSiLiHit::TTdrSiLiHit()
   : TDetectorHit()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
   Clear();
}

TTdrSiLiHit::TTdrSiLiHit(const TTdrSiLiHit& rhs) : TDetectorHit()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
   Clear();
   rhs.Copy(*this);
}

TTdrSiLiHit::~TTdrSiLiHit() = default;

void TTdrSiLiHit::Copy(TObject& rhs) const
{
   TDetectorHit::Copy(rhs);
   static_cast<TTdrSiLiHit&>(rhs).fFilter = fFilter;
}

bool TTdrSiLiHit::InFilter(Int_t)
{
   // check if the desired filter is in wanted filter;
   // return the answer;
   return true;
}

void TTdrSiLiHit::Clear(Option_t* opt)
{
   TDetectorHit::Clear(opt); // clears the base (address, position and waveform)
   fFilter = 0;
}

void TTdrSiLiHit::Print(Option_t*) const
{
	Print(std::cout);
}

void TTdrSiLiHit::Print(std::ostream& out) const
{
	std::ostringstream str;
   str<<"TdrSiLi Detector: "<<GetDetector()<<std::endl;
   str<<"TdrSiLi Energy:   "<<GetEnergy()<<std::endl;
   str<<"TdrSiLi hit time: "<<GetTime()<<std::endl;
	out<<str.str();
}

TVector3 TTdrSiLiHit::GetPosition(Double_t) const
{
   return TTdrSiLi::GetPosition(GetDetector());
}

TVector3 TTdrSiLiHit::GetPosition() const
{
   return GetPosition(GetDefaultDistance());
}
