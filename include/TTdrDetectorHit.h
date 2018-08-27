#ifndef TTDRDETECTORHIT_H
#define TTDRDETECTORHIT_H

/** \addtogroup Detectors
 *  @{
 */

#include "Globals.h"

#include "TDetectorHit.h"
#include "TFragment.h"

/////////////////////////////////////////////////////////////////
///
/// \class TTdrDetectorHit
///
/// This class overloads the GetTime function of TDetectorHit
/// and uses the TTdrMnemonic detectory type enumeration to
/// calculate the time.
///
/////////////////////////////////////////////////////////////////

class TTdrDetectorHit : public TDetectorHit {

   //
public:
   TTdrDetectorHit(const int& Address = 0xffffffff);
   TTdrDetectorHit(const TTdrDetectorHit&, bool copywave = true);
	TTdrDetectorHit(const TFragment& frag) : TDetectorHit(frag) {}
   ~TTdrDetectorHit() override;

   virtual Double_t GetTime(const ETimeFlag& correct_flag = ETimeFlag::kAll,
                            Option_t*     opt          = "") const override; ///< Returns a time value to the nearest nanosecond!
	
   /// \cond CLASSIMP
   ClassDefOverride(TTdrDetectorHit, 1) // Stores the information for a detector hit
   /// \endcond
};
/*! @} */
#endif
