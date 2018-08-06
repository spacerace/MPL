//
// histgram.c - (c) 1990 Chiverton Graphics, Inc.
//

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <memory.h>
#include "histgram.h"

HPS   hPS        = NULL ;
HWND  hwndClient = NULL ;
HHEAP hHeap      = NULL ;

INT   xClientWidth  = 0 ,
      yClientHeight = 0 ;

INT   indexMax,
      index,
      iArraySize,
      *npArray;


VOID create_histogram (HAB hab, HWND hwndClientWindow)
     {
     HDC    hdc   ;
     SIZEL  sizel = { 0L, 0L};
     RECTL  rectl = { 0L, 0L, 3000, 3000 } ;

     hwndClient = hwndClientWindow ;
     hdc        = WinOpenWindowDC (hwndClient) ;
     hPS        = GpiCreatePS (hab, hdc, &sizel, PU_PELS | GPIT_MICRO | GPIA_ASSOC) ;

     GpiSetBackColor (hPS, CLR_WHITE);
     GpiSetColor (hPS, CLR_BLACK);
     GpiSetLineType (hPS, LINETYPE_SOLID) ;

     //  Create a heap on the automatic data segment.
     //  Allocate an array of INTs. The array is large enough
     //  to store the bar chart for a maximized client window.
     //
     iArraySize = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
     hHeap      = WinCreateHeap (0,0, (sizeof(INT)*iArraySize),0,0,0) ;
     npArray    = (PINT) WinAllocMem (hHeap, (sizeof(INT)*iArraySize)) ;
     index      = 0 ;
     indexMax   = iArraySize - 1 ;
     }


void size_histogram (INT xPixelWidth, INT yPixelHeight)
     {
     if (xPixelWidth < index) index = 0 ;

     xClientWidth  = xPixelWidth ;
     yClientHeight = yPixelHeight;
     indexMax      = xPixelWidth - 1 ;

     WinInvalidateRect (hwndClient, NULL, FALSE) ;
     }


void update_histogram (INT iPercentOfLoad)
     {
     POINTL ptl;
     //
     //  If histogram is full, move it 50% to left.
     //
     if (index > indexMax)
          {
          INT iSkipAmount = xClientWidth / 2;

          memcpy (npArray, npArray + iSkipAmount, iSkipAmount*sizeof(INT));
          index = iSkipAmount;
          npArray [index++] = iPercentOfLoad ;

          // redraw histogram.
          WinInvalidateRect (hwndClient, NULL, FALSE) ;
          return;
          }

     // draw the newest bar
     //
     ptl.x = index;
     ptl.y = 0L;
     GpiMove (hPS, &ptl);
     ptl.y = (iPercentOfLoad/100.) * yClientHeight ;
     GpiLine (hPS, &ptl);

     npArray [index++] = iPercentOfLoad ;
     }


void paint_histogram (void)
     {
     INT    i;
     POINTL ptl;
     RECTL  rectl;

     if (hPS == NULL) return;

     WinBeginPaint (hwndClient, hPS, &rectl) ;
     GpiErase (hPS);

     for (i=0; i<index; i++)
          {
          ptl.x = i;
          ptl.y = 0L;
          GpiMove (hPS, &ptl);
          ptl.y = (npArray [i]/100.) * yClientHeight ;
          GpiLine (hPS, &ptl);
          }

     WinEndPaint (hPS) ;
     }



VOID destroy_histogram (void)
     {
     GpiDestroyPS (hPS) ;
     WinDestroyHeap (hHeap) ;
     }
