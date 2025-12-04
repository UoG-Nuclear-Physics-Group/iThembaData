#ifndef TTDRCLOVERBGO_H
#define TTDRCLOVERBGO_H

/** \addtogroup Detectors
 *  @{
 */

#include "Globals.h"
#include "TBgo.h"

class TTdrCloverBgo : public TBgo {
public:
   TTdrCloverBgo();
   TTdrCloverBgo(const TTdrCloverBgo&);
   virtual ~TTdrCloverBgo();

   TTdrCloverBgo& operator=(const TTdrCloverBgo&);   //!<!

   /// \cond CLASSIMP
   ClassDef(TTdrCloverBgo, 1)   // TdrCloverBgo Physics structure
   /// \endcond
};
/*! @} */
#endif
