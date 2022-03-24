#include "TTdrPlasticHit.h"

#include <iostream>
#include <algorithm>
#include <climits>

#include "Globals.h"
#include "TTdrPlastic.h"
#include "TGRSIOptions.h"

/// \cond CLASSIMP
ClassImp(TTdrPlasticHit)
/// \endcond

TTdrPlasticHit::TTdrPlasticHit()
{
	// Default Constructor
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	Clear();
}

TTdrPlasticHit::~TTdrPlasticHit() = default;

TTdrPlasticHit::TTdrPlasticHit(const TTdrPlasticHit& rhs) : TDetectorHit()
{
	// Copy Constructor
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
	Class()->IgnoreTObjectStreamer(kTRUE);
#endif
	Clear();
	rhs.Copy(*this);
}

TTdrPlasticHit::TTdrPlasticHit(const TFragment& frag) : TDetectorHit(frag)
{
	if(TTdrPlastic::SetWave()) {
		if(frag.GetWaveform()->empty()) {
			printf("Warning, TTdrPlastic::SetWave() set, but data waveform size is zero!\n");
		}
		if(false) {
			std::vector<Short_t> x;
			// Need to reorder waveform data for S1507 data from December 2014
			// All pairs of samples are swapped.
			// The first two samples are also delayed by 8.
			// We choose to throw out the first 2 samples (junk) and the last 6 samples (convience)
			x              = *(frag.GetWaveform());
			size_t  length = x.size() - (x.size() % 8);
			Short_t temp;

			if(length > 8) {
				for(size_t i = 0; i < length - 8; i += 8) {
					x[i]     = x[i + 9];
					x[i + 1] = x[i + 8];
					temp     = x[i + 2];
					x[i + 2] = x[i + 3];
					x[i + 3] = temp;
					temp     = x[i + 4];
					x[i + 4] = x[i + 5];
					x[i + 5] = temp;
					temp     = x[i + 6];
					x[i + 6] = x[i + 7];
					x[i + 7] = temp;
				}
				x.resize(length - 8);
			}
			SetWaveform(x);
		} else {
			frag.CopyWave(*this);
		}
	}
}

void TTdrPlasticHit::Copy(TObject& rhs) const
{
	// Copies a TTdrPlasticHit
	TDetectorHit::Copy(rhs);
	static_cast<TTdrPlasticHit&>(rhs).fFilter = fFilter;
}

void TTdrPlasticHit::Copy(TObject& obj, bool waveform) const
{
	Copy(obj);
	if(waveform) {
		CopyWave(obj);
	}
}

TVector3 TTdrPlasticHit::GetPosition(Double_t) const
{
	// Gets the position of the current TTdrPlasticHit
	return TTdrPlastic::GetPosition(GetDetector());
}

TVector3 TTdrPlasticHit::GetPosition() const
{
	// Gets the position of the current TTdrPlasticHit
	return GetPosition(GetDefaultDistance());
}

bool TTdrPlasticHit::InFilter(Int_t)
{
	// check if the desired filter is in wanted filter;
	// return the answer;
	return true;
}

void TTdrPlasticHit::Clear(Option_t*)
{
	// Clears the TdrPlasticHit
	fFilter = 0;
	TDetectorHit::Clear();
}

void TTdrPlasticHit::Print(Option_t*) const
{
	// Prints the TdrPlasticHit. Returns:
	// Detector
	// Energy
	// Time
	printf("TdrPlastic Detector: %i\n", GetDetector());
	printf("TdrPlastic hit energy: %.2f\n", GetEnergy());
	printf("TdrPlastic hit time:   %.lf\n", GetTime());
}

