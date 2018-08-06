/*------------------------------------------------------
   VF03.C -- Display four strings in mirror reflections
  ------------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Mirror (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Mirror" ;
     static LONG cbText = sizeof szText - 1 ;
     INT         i ;
     POINTL      ptl ;
     SIZEF       sizfx ;
                                   // Create font, select and scale

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient / 2, cyClient / 2) ;

     ptl.x = cxClient / 2 ;        // Center of client window
     ptl.y = cyClient / 2 ;

     for (i = 0 ; i < 4 ; i++)
          {
          GpiQueryCharBox (hps, &sizfx) ;

          if (i == 1 || i == 3)
               sizfx.cx *= -1 ;
                                   // Negate char box dimensions
          if (i == 2)
               sizfx.cy *= -1 ;

          GpiSetCharBox (hps, &sizfx) ;

          GpiCharStringAt (hps, &ptl, cbText, szText) ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;          // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;

     }
