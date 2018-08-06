/*----------------------------------------------------------
   VF06.C -- Display seven different character shear angles
  ----------------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include "vectfont.h"

VOID Display_CharShear (HPS hps, LONG cxClient, LONG cyClient)
     {
     static POINTL aptlShear[7] = { -100,  41, -100, 100,
                                     -41, 100,    0, 100,
                                      41, 100,  100, 100,
                                     100,  41 } ;
     CHAR          szBuffer[40] ;
     FONTMETRICS   fm ;
     INT           iIndex ;
     POINTL        ptl ;

                         // Create and scale Helvetica font

     CreateVectorFont (hps, LCID_MYFONT, "Helv") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleVectorFont (hps, 480, 480) ;

                         // Get font metrics for scaled font

     GpiQueryFontMetrics (hps, (LONG) sizeof (FONTMETRICS), &fm) ;

     ptl.x = cxClient / 8 ;
     ptl.y = cyClient ;

     for (iIndex = 0 ; iIndex < 7 ; iIndex++)
          {
          GpiSetCharShear (hps, aptlShear + iIndex) ;  // Char shear

          ptl.y -= fm.lMaxBaselineExt ;

          GpiCharStringAt (hps, &ptl,
               (LONG) sprintf (szBuffer, "Character Shear (%ld,%ld)",
                         aptlShear[iIndex].x, aptlShear[iIndex].y),
               szBuffer) ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
