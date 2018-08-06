/*--------------------------------------
   VF14.C -- Clipped wavy spline curves
  --------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include "vectfont.h"

VOID Display_Wavy (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Hello!" ;
     static LONG cbText = sizeof szText - 1 ;
     static LONG lColors[] = { CLR_BLUE, CLR_GREEN,  CLR_CYAN, CLR_RED,
                               CLR_PINK, CLR_YELLOW, CLR_WHITE } ;
     INT         i ;
     POINTL      ptl, aptl[8] ;

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn Italic") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleFontToBox (hps, cbText, szText, cxClient, cyClient) ;
     QueryStartPointInTextBox (hps, cbText, szText, &ptl) ;

     ColorClient (hps, cxClient, cyClient, CLR_BLACK) ;

     GpiBeginPath (hps, ID_PATH) ;
     GpiCharStringAt (hps, &ptl, cbText, szText) ;     // Text string
     GpiEndPath (hps) ;

     GpiSetClipPath (hps, ID_PATH, SCP_AND | SCP_ALTERNATE) ;

     for (i = 0 ; i < 14 ; i++)
          {
          aptl[0].x = 0 ;
          aptl[0].y = i * cyClient / 14 ;

          aptl[1].x = cxClient / 3 ;
          aptl[1].y = min (cyClient, 2 * i * cyClient / 14) ;

          aptl[2].x = 2 * cxClient / 3 ;
          aptl[2].y = max (0L, (2 * i - 14) * cyClient / 14) ;

          aptl[3].x = cxClient ;
          aptl[3].y = i * cyClient / 14 ;

          aptl[4].x = cxClient ;
          aptl[4].y = (i + 1) * cyClient / 14 ;

          aptl[5].x = 2 * cxClient / 3 ;
          aptl[5].y = max (0L, (2 * (i + 1) - 14) * cyClient / 14) ;

          aptl[6].x = cxClient / 3 ;
          aptl[6].y = min (cyClient, 2 * (i + 1) * cyClient / 14) ;

          aptl[7].x = 0 ;
          aptl[7].y = (i + 1) * cyClient / 14 ;

          GpiSetColor (hps, lColors[i % 7]) ;
          GpiBeginArea (hps, BA_BOUNDARY | BA_ALTERNATE) ;

          GpiMove (hps, aptl) ;                        // Splines
          GpiPolySpline (hps, 3L, aptl + 1) ;
          GpiLine (hps, aptl + 4) ;
          GpiPolySpline (hps, 3L, aptl + 5) ;
          GpiLine (hps, aptl) ;

          GpiEndArea (hps) ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
