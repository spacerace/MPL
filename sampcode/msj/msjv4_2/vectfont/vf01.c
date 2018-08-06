/*-----------------------------------------
   VF01.C -- Display 24-point vector fonts
  -----------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <string.h>
#include "vectfont.h"

VOID Display_24Point (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR *szFacename[] = {
                                 "Courier",      "Courier Italic",
                                 "Courier Bold", "Courier Bold Italic",
                                 "Tms Rmn",      "Tms Rmn Italic",
                                 "Tms Rmn Bold", "Tms Rmn Bold Italic",
                                 "Helv",         "Helv Italic",
                                 "Helv Bold",    "Helv Bold Italic"
                                 } ;
     static INT  iNumFonts = sizeof szFacename / sizeof szFacename[0] ;
     FONTMETRICS fm ;
     INT         iFont ;
     POINTL      ptl ;

     ptl.x = cxClient / 8 ;
     ptl.y = cyClient ;

     for (iFont = 0 ; iFont < iNumFonts ; iFont++)
          {
                                   // Create font, select it and scale

          CreateVectorFont (hps, LCID_MYFONT, szFacename[iFont]) ;
          GpiSetCharSet (hps, LCID_MYFONT) ;
          ScaleVectorFont (hps, 240, 240) ;

                                   // Get font metrics for scaled font

          GpiQueryFontMetrics (hps, (LONG) sizeof (FONTMETRICS), &fm) ;
          ptl.y -= fm.lMaxBaselineExt ;

                                   // Display the font facename

          GpiCharStringAt (hps, &ptl, (LONG) strlen (szFacename[iFont]),
                           szFacename[iFont]) ;

          GpiCharString (hps, 10L, " - abcdefg") ;

          GpiSetCharSet (hps, LCID_DEFAULT) ;     // Clean up
          GpiDeleteSetId (hps, LCID_MYFONT) ;
          }
     }
