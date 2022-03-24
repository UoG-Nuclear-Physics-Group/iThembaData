#include <iostream>
#include "TTdrSiLi.h"
#include "TRandom.h"
#include "TMath.h"

////////////////////////////////////////////////////////////
//
// TTdrSiLi
//
// The TTdrSiLi class defines the observables and algorithms used
// when analyzing GRIFFIN data.
//
////////////////////////////////////////////////////////////

/// \cond CLASSIMP
ClassImp(TTdrSiLi)
/// \endcond

bool TTdrSiLi::fSetCoreWave = false;

TTdrSiLi::TTdrSiLi() : TDetector()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	Clear();
}

TTdrSiLi::TTdrSiLi(const TTdrSiLi& rhs) : TDetector()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	rhs.Copy(*this);
}

void TTdrSiLi::Copy(TObject& rhs) const
{
	TDetector::Copy(rhs);

	static_cast<TTdrSiLi&>(rhs).fSetCoreWave = fSetCoreWave;
}

TTdrSiLi::~TTdrSiLi()
{
	// Default Destructor
}

void TTdrSiLi::Clear(Option_t* opt)
{
	// Clears the mother, all of the hits
	TDetector::Clear(opt);
}

void TTdrSiLi::Print(Option_t*) const
{
	// Prints out TTdrSiLi members, currently does nothing.
	printf("%lu fHits\n", fHits.size());
}

TTdrSiLi& TTdrSiLi::operator=(const TTdrSiLi& rhs)
{
	rhs.Copy(*this);
	return *this;
}

void TTdrSiLi::AddFragment(const std::shared_ptr<const TFragment>& frag, TChannel*)
{
	auto hit = new TTdrSiLiHit(*frag);
	fHits.push_back(hit);
}

TVector3 TTdrSiLi::GetPosition(int)
{
	// Gets the position vector for a crystal specified by DetNbr
	// Does not currently contain any positons.
	return TVector3(0, 0, 1);
}
