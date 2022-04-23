#ifndef TTTDRPLASTIC_H
#define TTTDRPLASTIC_H

/** \addtogroup Detectors
 *  @{
 */

/////////////////////////////////////////////////////////////
///
/// \class TTdrPlastic
///
/// The TTdrPlastic class defines the observables and algorithms used
/// when analyzing GRIFFIN data. It includes detector positions,
/// etc.
///
/////////////////////////////////////////////////////////////

#include <vector>
#include <cstdio>

#include "TVector3.h"

#include "Globals.h"
#include "TDetector.h"
#include "TTdrPlasticHit.h"

class TTdrPlastic : public TDetector {
public:
   TTdrPlastic();
   ~TTdrPlastic() override;
   TTdrPlastic(const TTdrPlastic& rhs);

public:
   TTdrPlasticHit* GetTdrPlasticHit(const int& i) const { return static_cast<TTdrPlasticHit*>(GetHit(i)); }
#ifndef __CINT__
   void AddFragment(const std::shared_ptr<const TFragment>&, TChannel*) override; //!<!
#endif

   static TVector3 GetPosition(int DetNbr) { return gPaddlePosition[DetNbr]; } //!<!

   TTdrPlastic& operator=(const TTdrPlastic&); //!<!

private:
   static bool fSetWave; //  Flag for Waveforms ON/OFF

public:
   static bool SetWave() { return fSetWave; } //!<!

private:
   static TVector3 gPaddlePosition[21]; //!<!  Position of each Paddle

public:
   void Clear(Option_t* opt = "") override;       //!<!
   void Print(Option_t* opt = "") const override; //!<!
	void Print(std::ostream& out) const override;

   /// \cond CLASSIMP
   ClassDefOverride(TTdrPlastic, 2) // TdrPlastic Physics structure
	/// \endcond
};
/*! @} */
#endif
