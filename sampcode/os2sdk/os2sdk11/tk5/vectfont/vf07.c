/*--------------------------------------------------
   VF07.C -- Display characters with sheared shadow
  --------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Shadow (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Shadow" ;
     static LONG cbText = sizeof szText - 1 ;
     POINTL      ptl, ptlShear ;
     SIZEF       sizfx ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, 3 * cxClient / 4, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     ColorClient (hps, cxClient, cyClient, CLR_BLUE) ;

     GpiSavePS (hps) ;

     ptlShear.x = 200 ;                                // Set char shear
     ptlShear.y = 100 ;
     GpiSetCharShear (hps, &ptlShear) ;

     GpiQueryCharBox (hps, &sizfx) ;
     sizfx.cy += sizfx.cy / 4 ;                        // Set char box
     GpiSetCharBox (hps, &sizfx) ;

     GpiSetColor (hps, CLR_DARKBLUE) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Display shadow

     GpiRestorePS (hps, -1L) ;

     GpiSetColor (hps, CLR_RED) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Display text

     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
