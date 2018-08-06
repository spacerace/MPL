/*---------------------------------
   VF09.C -- Font with Drop Shadow
  ---------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_DropShadow (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Hello!" ;
     static LONG cbText = sizeof szText - 1 ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Shadow

     ptl.x -= 12 ;       // 1/6 inch
     ptl.y += 12 ;

     GpiSetColor (hps, CLR_BACKGROUND) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text string

     GpiBeginPath (hps, ID_PATH) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Outline
     GpiEndPath (hps) ;

     GpiSetColor (hps, CLR_NEUTRAL) ;
     GpiStrokePath (hps, ID_PATH, 0L) ;

     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
