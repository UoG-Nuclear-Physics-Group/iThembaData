#ifndef TTDRMNEMONIC_H
#define TTDRMNEMONIC_H

#include <string>
#include "TMnemonic.h"
#include "Globals.h"
#include "TClass.h"

enum class EDigitizer : char { kDefault,
                               kCaen,
                               kPixie,
                               kFastPixie,
                               kPixieTapeMove };

class TTdrMnemonic : public TMnemonic {
public:
   TTdrMnemonic() : TMnemonic() { Clear(); }
   TTdrMnemonic(const char* name) : TTdrMnemonic() { TMnemonic::Parse(name); }
   ~TTdrMnemonic() override = default;

   // standard C++ makes these enumerations global to the class. ie, the name of the enumeration
   // EMnemonic or ESystem has no effect on the clashing of enumerated variable names.
   // These separations exist only to easily see the difference when looking at the code here.
   enum class ESystem {
      kTdrClover,   //0
      kTdrCloverBgo,
      kTdrTigress,
      kTdrTigressBgo,
      kTdrSiLi,
      kTdrPlastic,
      kClear   //6
   };

   ESystem System() const { return fSystem; }

   void Parse(std::string* name) override;

   void EnumerateDigitizer(TPriorityValue<std::string>& digitizerName, TPriorityValue<EDigitizer>& digitizerType, TPriorityValue<int>& timeStampUnit) override;

   TClass* GetClassType() const override;

   double GetTime(Long64_t timestamp, Float_t cfd, double energy, const TChannel* channel) const override;

   void Print(Option_t* opt = "") const override;
   void Clear(Option_t* opt = "") override;

private:
   ESystem fSystem;

   void EnumerateSystem();

   /// \cond CLASSIMP
   ClassDefOverride(TTdrMnemonic, 1)
   /// \endcond
};

#endif
