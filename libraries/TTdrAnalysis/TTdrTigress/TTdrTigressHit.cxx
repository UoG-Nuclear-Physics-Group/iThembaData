
#include "TTdrTigress.h"
#include "TTdrTigressHit.h"
#include "Globals.h"
#include <cmath>
#include <iostream>

TTdrTigressHit::TTdrTigressHit()
   : TDetectorHit()
{
// Default Ctor. Ignores TObject Streamer in ROOT < 6.
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 0, 0)
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
   Clear();
}

TTdrTigressHit::TTdrTigressHit(const TTdrTigressHit& rhs) : TDetectorHit()
{
   // Copy Ctor. Ignores TObject Streamer in ROOT < 6.
   Clear();
   rhs.Copy(*this);
}

TTdrTigressHit::TTdrTigressHit(const TFragment& frag) : TDetectorHit(frag)
{
   SetNPileUps(frag.GetNumberOfPileups());
}

TTdrTigressHit::~TTdrTigressHit() = default;

void TTdrTigressHit::Copy(TObject& rhs) const
{
   TDetectorHit::Copy(rhs);
   static_cast<TTdrTigressHit&>(rhs).fFilter = fFilter;
   // We should copy over a 0 and let the hit recalculate, this is safest
   static_cast<TTdrTigressHit&>(rhs).fTdrTigressHitBits   = 0;
   static_cast<TTdrTigressHit&>(rhs).fCrystal             = fCrystal;
   static_cast<TTdrTigressHit&>(rhs).fBremSuppressed_flag = fBremSuppressed_flag;
}

void TTdrTigressHit::Copy(TObject& obj, bool waveform) const
{
   Copy(obj);
   if(waveform) {
      CopyWave(obj);
   }
}

bool TTdrTigressHit::InFilter(Int_t)
{
   // check if the desired filter is in wanted filter;
   // return the answer;
   return true;
}

void TTdrTigressHit::Clear(Option_t* opt)
{
   // Clears the information stored in the TTdrTigressHit.
   TDetectorHit::Clear(opt);   // clears the base (address, position and waveform)
   fFilter              = 0;
   fTdrTigressHitBits   = 0;
   fCrystal             = 0xFFFF;
   fBremSuppressed_flag = false;
}

void TTdrTigressHit::Print(Option_t*) const
{
   // Prints the Detector Number, Crystal Number, Energy, Time and Angle.
   Print(std::cout);
}

void TTdrTigressHit::Print(std::ostream& out) const
{
   std::ostringstream str;
   str << "TdrTigress Detector: " << GetDetector() << std::endl;
   str << "TdrTigress Crystal:  " << GetCrystal() << std::endl;
   str << "TdrTigress Energy:   " << GetEnergy() << std::endl;
   str << "TdrTigress hit time: " << GetTime() << std::endl;
   str << "TdrTigress hit TV3 theta: " << GetPosition().Theta() * 180. / 3.141597 << "\tphi" << GetPosition().Phi() * 180. / 3.141597 << std::endl;
   out << str.str();
}

TVector3 TTdrTigressHit::GetPosition(double dist) const
{
   return TTdrTigress::GetPosition(GetDetector(), GetCrystal(), dist);
}

TVector3 TTdrTigressHit::GetPosition() const
{
   return GetPosition(GetDefaultDistance());
}

bool TTdrTigressHit::CompareEnergy(const TTdrTigressHit* lhs, const TTdrTigressHit* rhs)
{
   return (lhs->GetEnergy() > rhs->GetEnergy());
}

void TTdrTigressHit::Add(const TDetectorHit* hit)
{
   // add another TDR tigress hit to this one (for addback),
   // using the time and position information of the one with the higher energy
   const TTdrTigressHit* tigressHit = dynamic_cast<const TTdrTigressHit*>(hit);
   if(tigressHit == nullptr) {
      throw std::runtime_error("trying to add non-tdr-tigress hit to tdr-tigress hit!");
   }

   if(!CompareEnergy(this, tigressHit)) {
      SetCfd(tigressHit->GetCfd());
      SetTime(tigressHit->GetTime());
      // SetPosition(tigressHit->GetPosition());
      SetAddress(tigressHit->GetAddress());
   } else {
      SetTime(GetTime());
   }
   SetEnergy(GetEnergy() + tigressHit->GetEnergy());
   // this has to be done at the very end, otherwise GetEnergy() might not work
   SetCharge(0);
   // Add all of the pileups.This should be changed when the max number of pileups changes
   if((NPileUps() + tigressHit->NPileUps()) < 4) {
      SetNPileUps(NPileUps() + tigressHit->NPileUps());
   } else {
      SetNPileUps(3);
   }
   if((PUHit() + tigressHit->PUHit()) < 4) {
      SetPUHit(PUHit() + tigressHit->PUHit());
   } else {
      SetPUHit(3);
   }
   // KValue is somewhate meaningless in addback, so I am using it as an indicator that a piledup hit was added-back RD
   if(GetKValue() > tigressHit->GetKValue()) {
      SetKValue(tigressHit->GetKValue());
   }
}

void TTdrTigressHit::SetTdrTigressFlag(enum ETdrTigressHitBits flag, Bool_t set)
{
   fTdrTigressHitBits.SetBit(flag, set);
}

UShort_t TTdrTigressHit::NPileUps() const
{
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.
   return static_cast<UShort_t>(fTdrTigressHitBits.TestBits(ETdrTigressHitBits::kTotalPU1) + fTdrTigressHitBits.TestBits(ETdrTigressHitBits::kTotalPU2));
}

UShort_t TTdrTigressHit::PUHit() const
{
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.
   return static_cast<UShort_t>(fTdrTigressHitBits.TestBits(ETdrTigressHitBits::kPUHit1) +
                                (fTdrTigressHitBits.TestBits(ETdrTigressHitBits::kPUHit2) >> static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kPUHitOffset)));
}

void TTdrTigressHit::SetNPileUps(UChar_t npileups)
{
   SetTdrTigressFlag(ETdrTigressHitBits::kTotalPU1, (npileups & static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kTotalPU1)) != 0);
   SetTdrTigressFlag(ETdrTigressHitBits::kTotalPU2, (npileups & static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kTotalPU2)) != 0);
}

void TTdrTigressHit::SetPUHit(UChar_t puhit)
{
   if(puhit > 2) {
      puhit = 3;
   }
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.

   SetTdrTigressFlag(ETdrTigressHitBits::kPUHit1, ((puhit << static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kPUHitOffset)) & static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kPUHit1)) != 0);
   SetTdrTigressFlag(ETdrTigressHitBits::kPUHit2, ((puhit << static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kPUHitOffset)) & static_cast<std::underlying_type<ETdrTigressHitBits>::type>(ETdrTigressHitBits::kPUHit2)) != 0);
}
