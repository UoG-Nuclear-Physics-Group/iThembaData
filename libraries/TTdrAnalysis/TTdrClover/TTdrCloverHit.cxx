
#include "TTdrClover.h"
#include "TTdrCloverHit.h"
#include "Globals.h"
#include <cmath>
#include <iostream>

TTdrCloverHit::TTdrCloverHit()
   : TDetectorHit()
{
// Default Ctor. Ignores TObject Streamer in ROOT < 6.
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 0, 0)
   Class()->IgnoreTObjectStreamer(kTRUE);
#endif
   Clear();
}

TTdrCloverHit::TTdrCloverHit(const TTdrCloverHit& rhs) : TDetectorHit()
{
   // Copy Ctor. Ignores TObject Streamer in ROOT < 6.
   Clear();
   rhs.Copy(*this);
}

TTdrCloverHit::TTdrCloverHit(const TFragment& frag) : TDetectorHit(frag)
{
   SetNPileUps(frag.GetNumberOfPileups());
}

TTdrCloverHit::~TTdrCloverHit() = default;

void TTdrCloverHit::Copy(TObject& rhs) const
{
   TDetectorHit::Copy(rhs);
   static_cast<TTdrCloverHit&>(rhs).fFilter              = fFilter;
   static_cast<TTdrCloverHit&>(rhs).fCrystal             = fCrystal;
   static_cast<TTdrCloverHit&>(rhs).fBremSuppressed_flag = fBremSuppressed_flag;
}

void TTdrCloverHit::Copy(TObject& obj, bool waveform) const
{
   Copy(obj);
   if(waveform) {
      CopyWave(obj);
   }
}

bool TTdrCloverHit::InFilter(Int_t)
{
   // check if the desired filter is in wanted filter;
   // return the answer;
   return true;
}

void TTdrCloverHit::Clear(Option_t* opt)
{
   // Clears the information stored in the TTdrCloverHit.
   TDetectorHit::Clear(opt);   // clears the base (address, position and waveform)
   fFilter              = 0;
   fCrystal             = 0xFFFF;
   fBremSuppressed_flag = false;
}

void TTdrCloverHit::Print(Option_t*) const
{
   // Prints the Detector Number, Crystal Number, Energy, Time and Angle.
   Print(std::cout);
}

void TTdrCloverHit::Print(std::ostream& out) const
{
   std::ostringstream str;
   str << "TdrClover Detector: " << GetDetector() << std::endl;
   str << "TdrClover Crystal:  " << GetCrystal() << std::endl;
   str << "TdrClover Energy:   " << GetEnergy() << std::endl;
   str << "TdrClover hit time: " << GetTime() << std::endl;
   str << "TdrClover hit TV3 theta: " << GetPosition().Theta() * 180. / 3.141597 << "\tphi" << GetPosition().Phi() * 180. / 3.141597 << std::endl;
   out << str.str();
}

TVector3 TTdrCloverHit::GetPosition(double dist) const
{
   return TTdrClover::GetPosition(GetDetector(), GetCrystal(), dist);
}

TVector3 TTdrCloverHit::GetPosition() const
{
   return GetPosition(GetDefaultDistance());
}

bool TTdrCloverHit::CompareEnergy(const TTdrCloverHit* lhs, const TTdrCloverHit* rhs)
{
   return (lhs->GetEnergy() > rhs->GetEnergy());
}

void TTdrCloverHit::Add(const TDetectorHit* hit)
{
   // add another TDR clover hit to this one (for addback),
   // using the time and position information of the one with the higher energy
   const TTdrCloverHit* cloverHit = dynamic_cast<const TTdrCloverHit*>(hit);
   if(cloverHit == nullptr) {
      throw std::runtime_error("trying to add non-clover hit to clover hit!");
   }

   if(!CompareEnergy(this, cloverHit)) {
      SetCfd(cloverHit->GetCfd());
      SetTime(cloverHit->GetTime());
      // SetPosition(cloverHit->GetPosition());
      SetAddress(cloverHit->GetAddress());
   } else {
      SetTime(GetTime());
   }
   SetEnergy(GetEnergy() + cloverHit->GetEnergy());
   // this has to be done at the very end, otherwise GetEnergy() might not work
   SetCharge(0);
   // Add all of the pileups.This should be changed when the max number of pileups changes
   if((NPileUps() + cloverHit->NPileUps()) < 4) {
      SetNPileUps(NPileUps() + cloverHit->NPileUps());
   } else {
      SetNPileUps(3);
   }
   if((PUHit() + cloverHit->PUHit()) < 4) {
      SetPUHit(PUHit() + cloverHit->PUHit());
   } else {
      SetPUHit(3);
   }
   // KValue is somewhate meaningless in addback, so I am using it as an indicator that a piledup hit was added-back RD
   if(GetKValue() > cloverHit->GetKValue()) {
      SetKValue(cloverHit->GetKValue());
   }
}

void TTdrCloverHit::SetTdrCloverFlag(enum ETdrCloverHitBits flag, Bool_t set)
{
   SetHitBit(static_cast<TDetectorHit::EBitFlag>(flag), set);
}

UShort_t TTdrCloverHit::NPileUps() const
{
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.
   return static_cast<UShort_t>(TestHitBit(static_cast<TDetectorHit::EBitFlag>(ETdrCloverHitBits::kTotalPU1)) +
                                TestHitBit(static_cast<TDetectorHit::EBitFlag>(ETdrCloverHitBits::kTotalPU2)));
}

UShort_t TTdrCloverHit::PUHit() const
{
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.
   return static_cast<UShort_t>(TestHitBit(static_cast<TDetectorHit::EBitFlag>(ETdrCloverHitBits::kPUHit1)) +
                                (TestHitBit(static_cast<TDetectorHit::EBitFlag>(ETdrCloverHitBits::kPUHit2)) >> static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kPUHitOffset)));
}

void TTdrCloverHit::SetNPileUps(UChar_t npileups)
{
   SetTdrCloverFlag(ETdrCloverHitBits::kTotalPU1, (npileups & static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kTotalPU1)) != 0);
   SetTdrCloverFlag(ETdrCloverHitBits::kTotalPU2, (npileups & static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kTotalPU2)) != 0);
}

void TTdrCloverHit::SetPUHit(UChar_t puhit)
{
   if(puhit > 2) {
      puhit = 3;
   }
   // The pluralized test bits returns the actual value of the fBits masked. Not just a bool.

   SetTdrCloverFlag(ETdrCloverHitBits::kPUHit1, ((puhit << static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kPUHitOffset)) & static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kPUHit1)) != 0);
   SetTdrCloverFlag(ETdrCloverHitBits::kPUHit2, ((puhit << static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kPUHitOffset)) & static_cast<std::underlying_type<ETdrCloverHitBits>::type>(ETdrCloverHitBits::kPUHit2)) != 0);
}
