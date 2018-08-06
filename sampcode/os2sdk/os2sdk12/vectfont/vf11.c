/*-----------------------------------------------------
   VF11.C -- Neon font using geometrically-thick lines
  -----------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Neon (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = " Neon " ;
     static LONG cbText = sizeof szText - 1 ;
     static LONG lForeColor[] = { CLR_DARKRED, CLR_DARKRED, CLR_RED,
                                  CLR_RED,     CLR_WHITE,   CLR_WHITE };
     static LONG lBackColor[] = { CLR_BLACK,   CLR_DARKRED, CLR_DARKRED,
                                  CLR_RED,     CLR_RED,     CLR_WHITE };
     static LONG lWidth[] = { 34, 28, 22, 16, 10, 4 } ;

     INT         iIndex ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     ColorClient (hps, cxClient, cyClient, CLR_BLACK) ;

     for (iIndex = 0 ; iIndex < 6 ; iIndex++)
          {
          GpiBeginPath (hps, ID_PATH) ;
          GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text out
          GpiEndPath (hps) ;

          GpiSetColor (hps, lForeColor[iIndex]) ;
          GpiSetBackColor (hps, lBackColor[iIndex]) ;
          GpiSetBackMix (hps, BM_OVERPAINT) ;
          GpiSetPattern (hps, PATSYM_HALFTONE) ;
          GpiSetLineWidthGeom (hps, lWidth[iIndex]) ;

          GpiStrokePath (hps, ID_PATH, 0L) ;           // Stroke path
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
