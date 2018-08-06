/*--------------------------
   VF13.C -- Clipped Spokes
  --------------------------*/

#define INCL_GPI
#include <os2.h>
#include <math.h>
#include "vectfont.h"

VOID Display_Spokes (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "WOW" ;
     static LONG cbText = sizeof szText - 1 ;
     static LONG lColors[] = { CLR_BLUE, CLR_GREEN, CLR_CYAN,
                               CLR_RED,  CLR_PINK,  CLR_YELLOW,
                               CLR_WHITE } ;
     double      dMaxRadius ;
     INT         i, iNumColors = sizeof lColors / sizeof lColors[0] ;
     POINTL      ptl ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     ColorClient (hps, cxClient, cyClient, CLR_BLACK) ;

     GpiBeginPath (hps, ID_PATH) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text string
     GpiEndPath (hps) ;

     GpiSetClipPath (hps, ID_PATH, SCP_AND | SCP_ALTERNATE) ;

     dMaxRadius = sqrt (pow (cxClient / 2.0, 2.0) +
                        pow (cyClient / 2.0, 2.0)) ;
                                                       // Draw spokes
     for (i = 0 ; i < 360 ; i++)
          {
          GpiSetColor (hps, lColors[i % iNumColors]) ;

          ptl.x = cxClient / 2 ;
          ptl.y = cyClient / 2 ;
          GpiMove (hps, &ptl) ;

          ptl.x += (LONG) (dMaxRadius * cos (i * 6.28 / 360)) ;
          ptl.y += (LONG) (dMaxRadius * sin (i * 6.28 / 360)) ;
          GpiLine (hps, &ptl) ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
