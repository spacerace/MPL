/*--------------------------------------------
   VF05.C -- Display "Hello, world" in circle
  --------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <math.h>
#include <stdlib.h>
#include "vectfont.h"

VOID Display_Rotate (HPS hps, LONG cxClient, LONG cyClient)
     {
     static CHAR szText[] = "Hello, world! " ;
     static LONG cbText = sizeof szText - 1L ;
     static LONG alWidthTable[256] ;
     double      ang, angCharWidth, angChar ;
     FONTMETRICS fm ;
     GRADIENTL   gradl ;
     INT         iChar ;
     LONG        lCircum, lRadius, lTotWidth, lCharRadius, cyChar ;
     POINTL      ptl ;

                         // Create the font and get font metrics

     CreateVectorFont (hps, LCID_MYFONT, "Tms Rmn") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;

     GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;

                         // Find circle dimensions and scale font

     lRadius = min (cxClient / 4, cyClient / 4) ;
     lCircum = (LONG) (2 * PI * lRadius) ;
     cyChar  = fm.lMaxBaselineExt * lRadius / fm.lMaxAscender ;

     ScaleFontToBox (hps, cbText, szText, lCircum, cyChar) ;

                         // Obtain width table and total width

     GpiQueryWidthTable (hps, 0L, 256L, alWidthTable) ;

     for (lTotWidth = 0, iChar = 0 ; iChar < (INT) cbText ; iChar ++)
          lTotWidth += alWidthTable [szText [iChar]] ;

     ang = PI / 2 ;      // Initial angle for first character

     for (iChar = 0 ; iChar < (INT) cbText ; iChar++)
          {
                              // Set character angle

          angCharWidth = 2 * PI * alWidthTable [szText [iChar]] / lTotWidth ;

          gradl.x = (LONG) (lRadius * cos (ang - angCharWidth / 2 - PI / 2)) ;
          gradl.y = (LONG) (lRadius * sin (ang - angCharWidth / 2 - PI / 2)) ;

          GpiSetCharAngle (hps, &gradl) ;

                              // Find position for character and display it

          angChar = atan2 ((double) alWidthTable [szText [iChar]] / 2,
                           (double) lRadius) ;

          lCharRadius = (LONG) (lRadius / cos (angChar)) ;
          angChar += ang - angCharWidth / 2 ;

          ptl.x = (LONG) (cxClient / 2 + lCharRadius * cos (angChar)) ;
          ptl.y = (LONG) (cyClient / 2 + lCharRadius * sin (angChar)) ;

          GpiCharStringAt (hps, &ptl, 1L, szText + iChar) ;

          ang -= angCharWidth ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;          // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
