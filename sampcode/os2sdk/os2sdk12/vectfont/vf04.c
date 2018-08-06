/*------------------------------------------
   VF04.C -- Display eight character angles
  ------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include "vectfont.h"

VOID Display_CharAngle (HPS hps, LONG cxClient, LONG cyClient)
     {
     static GRADIENTL agradl[8] = { 100,    0,  100,  100,
                                      0,  100, -100,  100,
                                   -100,    0, -100, -100,
                                      0, -100,  100, -100 } ;
     CHAR             szBuffer[40] ;
     INT              iIndex ;
     POINTL           ptl ;

                              // Create Helvetica font

     CreateVectorFont (hps, LCID_MYFONT, "Helv") ;
     GpiSetCharSet (hps, LCID_MYFONT) ;
     ScaleVectorFont (hps, 200, 200) ;

     ptl.x = cxClient / 2 ;   // Center of client window
     ptl.y = cyClient / 2 ;

     for (iIndex = 0 ; iIndex < 8 ; iIndex++)
          {
          GpiSetCharAngle (hps, agradl + iIndex) ;     // Char angle

          GpiCharStringAt (hps, &ptl,
               (LONG) sprintf (szBuffer, " Character Angle (%ld,%ld)",
                               agradl[iIndex].x, agradl[iIndex].y),
               szBuffer) ;
          }
     GpiSetCharSet (hps, LCID_DEFAULT) ;               // Clean up
     GpiDeleteSetId (hps, LCID_MYFONT) ;
     }
