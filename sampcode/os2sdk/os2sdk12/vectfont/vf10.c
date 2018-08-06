/*----------------------------
   VF10.C -- Solid block font
  ----------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Block (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = " Block " ;
     static LONG cbText = sizeof szText - 1 ;
     INT         i ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     ColorClient (hps, cxClient, cyClient, CLR_WHITE) ;
     GpiSetColor (hps, CLR_DARKGREEN) ;

     for (i = 0 ; i < 18 ; i++)
          {
          GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Block

          ptl.x -= 1 ;
          ptl.y -= 1 ;
          }

     GpiSetColor (hps, CLR_GREEN) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text string

     GpiBeginPath (hps, ID_PATH) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Outline
     GpiEndPath (hps) ;

     GpiSetColor (hps, CLR_DARKGREEN) ;
     GpiStrokePath (hps, ID_PATH, 0L) ;

     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
