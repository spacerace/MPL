/*----------------------------------------------------------
   VF02.C -- Display vector font stretched to client window
  ----------------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Stretch (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Hello!" ;
     static LONG cbText = sizeof szText - 1 ;
     POINTL      ptl ;

                              // Create font, select, and scale

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Display text

     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
