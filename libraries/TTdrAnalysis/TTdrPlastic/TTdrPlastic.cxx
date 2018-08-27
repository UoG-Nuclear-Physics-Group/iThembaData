#include <iostream>
#include "TTdrPlastic.h"
#include <TRandom.h>
#include <TMath.h>

/// \cond CLASSIMP
ClassImp(TTdrPlastic)
/// \endcond

bool TTdrPlastic::fSetWave = false;

TVector3 TTdrPlastic::gPaddlePosition[21] = {
	// TdrPlastic positions from Evan; Thanks Evan.
	TVector3(0, 0, 1),
	TVector3(14.3025, 4.6472, 22.8096),
	TVector3(0, 15.0386, 22.8096),
	TVector3(-14.3025, 4.6472, 22.8096),
	TVector3(-8.8395, -12.1665, 22.8096),
	TVector3(8.8395, -12.1665, 22.8096),
	TVector3(19.7051, 6.4026, 6.2123),
	TVector3(0, 20.7192, 6.2123),
	TVector3(-19.7051, 6.4026, 6.2123),
	TVector3(-12.1784, -16.7622, 6.2123),
	TVector3(12.1784, -16.7622, 6.2123),
	TVector3(19.7051, 6.4026, -6.2123),
	TVector3(0, 20.7192, -6.2123),
	TVector3(-19.7051, 6.4026, -6.2123),
	TVector3(-12.1784, -16.7622, -6.2123),
	TVector3(12.1784, -16.7622, -6.2123),
	TVector3(14.3025, 4.6472, -22.8096),
	TVector3(0, 15.0386, -22.8096),
	TVector3(-14.3025, 4.6472, -22.8096),
	TVector3(-8.8395, -12.1665, -22.8096),
	TVector3(8.8395, -12.1665, -22.8096)};

TTdrPlastic::TTdrPlastic()
{
	// Default Constructor
#if MAJOR_ROOT_VERSION < 6
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	// Class()->AddRule("TTdrPlastic sceptar_hits attributes=NotOwner");
	// Class()->AddRule("TTdrPlastic sceptardata attributes=NotOwner");
	Clear();
}

TTdrPlastic::~TTdrPlastic()
{
	// Default Destructor
}

TTdrPlastic::TTdrPlastic(const TTdrPlastic& rhs) : TDetector()
{
	// Copy Contructor
#if MAJOR_ROOT_VERSION < 6
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	rhs.Copy(*this);
}

void TTdrPlastic::Clear(Option_t* opt)
{
	// Clears all of the hits
	// The Option "all" clears the base class.
	TDetector::Clear(opt);
}

TTdrPlastic& TTdrPlastic::operator=(const TTdrPlastic& rhs)
{
	rhs.Copy(*this);
	return *this;
}

void TTdrPlastic::AddFragment(const std::shared_ptr<const TFragment>& frag, TChannel*)
{
	auto scHit = new TTdrPlasticHit(*frag);                 // Construction of TTdrPlasticHit is handled in the constructor
	fHits.push_back(std::move(scHit)); // Can't use scHit outside of vector after using std::move
}

void TTdrPlastic::Print(Option_t*) const
{
	// Prints out TTdrPlastic Multiplicity, currently does little.
	printf("%lu fHits\n", fHits.size());
}

