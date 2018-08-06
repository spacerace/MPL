/*-----------------------
   VF08.C -- Hollow font
  -----------------------*/

#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

VOID Display_Hollow (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Hollow" ;
     static LONG cbText = sizeof szText - 1 ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     GpiBeginPath (hps, ID_PATH) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text in path
     GpiEndPath (hps) ;

     GpiStrokePath (hps, ID_PATH, 0L) ;                // Stroke path

     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
