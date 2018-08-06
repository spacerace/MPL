/*------------------------------------------------------
   VF12.C -- Fading font with various pattern densities
  ------------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Fade (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Fade" ;
     static LONG cbText = sizeof szText - 1 ;
     LONG        lPattern ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     GpiSetBackMix (hps, BM_OVERPAINT) ;

     for (lPattern = 8 ; lPattern >= 1 ; lPattern--)
          {
          GpiBeginPath (hps, ID_PATH) ;
          GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text out
          GpiEndPath (hps) ;

          GpiSetPattern (hps, lPattern) ;
          GpiFillPath (hps, ID_PATH, FPATH_ALTERNATE) ;     // Fill path

          ptl.x += 2 ;
          ptl.y -= 2 ;
          }

     GpiSetPattern (hps, PATSYM_SOLID) ;
     GpiSetBackMix (hps, BM_LEAVEALONE) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;          // Solid

     GpiSetCharSet (hps, LCID_DEFAULT) ;                    // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
