#ifndef TTDRPLASTICHIT_H
#define TTDRPLASTICHIT_H

/** \addtogroup Detectors
 *  @{
 */

/////////////////////////////////////////////////////////////////
///
/// \class TTdrPlasticHit
///
/// This is class that contains the information about a sceptar
/// hit. This class is used to find energy, time, etc.
///
/////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cmath>
#include <vector>

#include "TFragment.h"
#include "TChannel.h"

#include "TVector3.h"

#include "TDetectorHit.h"

class TTdrPlasticHit : public TDetectorHit {
public:
   TTdrPlasticHit();
   ~TTdrPlasticHit() override;
   TTdrPlasticHit(const TTdrPlasticHit&);
   TTdrPlasticHit(const TFragment& frag);

private:
   Int_t fFilter{0};

public:
   /////////////////////////		/////////////////////////////////////
   inline void SetFilterPattern(const int& x) { fFilter = x; } //!<!

   /////////////////////////		/////////////////////////////////////
   inline Int_t GetFilterPattern() const { return fFilter; } //!<!

   bool InFilter(Int_t); //!<!

   TVector3 GetPosition(Double_t dist) const override; //!<!
   TVector3 GetPosition() const override;              //!<!

public:
   void Clear(Option_t* opt = "") override;       //!<!
   void Print(Option_t* opt = "") const override; //!<!
   void Copy(TObject&) const override;            //!<!
   void Copy(TObject&, bool) const override;      //!<!

private:
   Double_t GetDefaultDistance() const { return 0.0; }

   /// \cond CLASSIMP
   ClassDefOverride(TTdrPlasticHit, 3) // Stores the information for a TdrPlasticHit
   /// \endcond
};
/*! @} */
#endif
