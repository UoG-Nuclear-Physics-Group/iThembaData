#ifndef TTDRTIGRESSBGO_H
#define TTDRTIGRESSBGO_H

/** \addtogroup Detectors
 *  @{
 */

#include "Globals.h"
#include "TBgo.h"

class TTdrTigressBgo : public TBgo {
public:
   TTdrTigressBgo();
   TTdrTigressBgo(const TTdrTigressBgo&);
   virtual ~TTdrTigressBgo();

   TTdrTigressBgo& operator=(const TTdrTigressBgo&); //!<!

   /// \cond CLASSIMP
   ClassDef(TTdrTigressBgo, 1) // TdrTigressBgo Physics structure
   /// \endcond
};
/*! @} */
#endif
