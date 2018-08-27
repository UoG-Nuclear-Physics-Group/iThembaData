#ifndef TTDRDETECTORINFORMATION_H
#define TTDRDETECTORINFORMATION_H

/** \addtogroup Sorting
 *  @{
 */

/////////////////////////////////////////////////////////////////
///
/// \class TTdrDetectorInformation
///
/// This Class is designed to store setup dependent
/// information. It is used to store existence of
/// detector systems, etc.
///
/////////////////////////////////////////////////////////////////

#include "Globals.h"

#include "TDetectorInformation.h"
#include "TChannel.h"

class TTdrDetectorInformation : public TDetectorInformation {
public:
   TTdrDetectorInformation();
   ~TTdrDetectorInformation() override;
	
	TEventBuildingLoop::EBuildMode BuildMode() const override;

	void Print(Option_t* opt = "") const override;
	void Clear(Option_t* = "") override;
	void Set() override;

	inline void SetTdrClover(bool flag = true) { fTdrClover = flag; }
	inline void SetTdrTigress(bool flag = true) { fTdrTigress = flag; }
	inline void SetTdrSiLi(bool flag = true) { fTdrSiLi = flag; }
	inline void SetTdrPlastic(bool flag = true) { fTdrPlastic = flag; }

	inline bool TdrClover() const { return fTdrClover; }
	inline bool TdrTigress() const { return fTdrTigress; }
	inline bool TdrSiLi() const { return fTdrSiLi; }
	inline bool TdrPlastic() const { return fTdrPlastic; }

private:
	//  detector types to switch over in Set()

	bool fTdrClover{false};  // flag for TdrClover on/off
	bool fTdrTigress{false}; // flag for TdrTigress on/off
	bool fTdrSiLi{false};    // flag for TdrSiLi on/off
	bool fTdrPlastic{false}; // flag for TdrPlastic on/off

	/// \cond CLASSIMP
	ClassDefOverride(TTdrDetectorInformation, 1); // Contains the run-dependent information.
	/// \endcond
};
/*! @} */
#endif
