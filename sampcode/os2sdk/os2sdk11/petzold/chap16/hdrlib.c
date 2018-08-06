/*-----------------------------------------------------------
   HDRLIB.C -- "Handy Drawing Routines" Dynamic Link Library
  -----------------------------------------------------------*/

#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "hdrlib.h"

SHORT APIENTRY HdrPuts (HPS hps, PPOINTL pptl, PCHAR szText)
     {
     SHORT sLength = strlen (szText) ;

     if (pptl == NULL)
          GpiCharString (hps, (LONG) sLength, szText) ;
     else
          GpiCharStringAt (hps, pptl, (LONG) sLength, szText) ;

     return sLength ;
     }

SHORT cdecl FAR HdrPrintf (HPS hps, PPOINTL pptl, PCHAR szFormat, ...)
     {
     static CHAR chBuffer [1024] ;
     SHORT       sLength ;
     va_list     pArguments ;

     va_start (pArguments, szFormat) ;
     sLength = vsprintf (chBuffer, szFormat, pArguments) ;

     if (pptl == NULL)
          GpiCharString (hps, (LONG) sLength, chBuffer) ;
     else
          GpiCharStringAt (hps, pptl, (LONG) sLength, chBuffer) ;

     va_end (pArguments) ;
     return sLength ;
     }

LONG APIENTRY HdrEllipse (HPS hps, LONG lOption, PPOINTL pptl)
     {
     POINTL ptlCurrent ;

     GpiQueryCurrentPosition (hps, &ptlCurrent) ;

     return GpiBox (hps, lOption, pptl, labs (pptl->x - ptlCurrent.x),
                                        labs (pptl->y - ptlCurrent.y)) ;
     }
