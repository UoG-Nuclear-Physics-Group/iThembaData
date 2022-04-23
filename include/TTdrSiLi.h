#ifndef TTDRSILI_H
#define TTDRSILI_H

/** \addtogroup Detectors
 *  @{
 */

#include <vector>
#include <cstdio>

#include "TBits.h"
#include "TVector3.h"

#include "Globals.h"
#include "TDetector.h"
#include "TTdrSiLiHit.h"

class TTdrSiLi : public TDetector {

public:
   TTdrSiLi();
   TTdrSiLi(const TTdrSiLi&);
   ~TTdrSiLi() override;

public:
   TTdrSiLiHit* GetTdrSiLiHit(const int& i) const { return static_cast<TTdrSiLiHit*>(GetHit(i)); }

#ifndef __CINT__
   void AddFragment(const std::shared_ptr<const TFragment>&, TChannel*) override;
#endif
   static TVector3 GetPosition(int DetNbr); //!<!

   TTdrSiLi& operator=(const TTdrSiLi&); //!<!

private:
   static bool fSetCoreWave; //!<!  Flag for Waveforms ON/OFF

public:
   static bool SetCoreWave() { return fSetCoreWave; } //!<!

   void Copy(TObject&) const override;            //!<!
   void Clear(Option_t* opt = "all") override;    //!<!
   void Print(Option_t* opt = "") const override; //!<!
	void Print(std::ostream& out) const override;

   /// \cond CLASSIMP
   ClassDefOverride(TTdrSiLi, 4) // TdrSiLi Physics structure
   /// \endcond
};
/*! @} */
#endif
