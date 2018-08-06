/*--------------------------------------------------
   VF00.C -- Routines for working with vector fonts
  --------------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "vectfont.h"

extern HAB hab ;

LONG CreateVectorFont (HPS hps, LONG lcid, CHAR *szFacename)
     {
     FATTRS fat ;

     fat.usRecordLength  = sizeof fat ;
     fat.fsSelection     = 0 ;
     fat.lMatch          = 0 ;
     fat.idRegistry      = 0 ;
     fat.usCodePage      = GpiQueryCp (hps) ;
     fat.lMaxBaselineExt = 0 ;
     fat.lAveCharWidth   = 0 ;
     fat.fsType          = 0 ;
     fat.fsFontUse       = FATTR_FONTUSE_OUTLINE |
                           FATTR_FONTUSE_TRANSFORMABLE ;

     strcpy (fat.szFacename, szFacename) ;

     return GpiCreateLogFont (hps, NULL, lcid, &fat) ;
     }

BOOL ScaleVectorFont (HPS hps, SHORT xPointSize, SHORT yPointSize)
     {
     HDC    hdc ;
     LONG   xDeviceRes, yDeviceRes ;
     POINTL ptlFont ;
     SIZEF  sizfx ;
                         // Get device resolution in pixels per meter

     hdc = GpiQueryDevice (hps) ;

     DevQueryCaps (hdc, CAPS_HORIZONTAL_RESOLUTION, 1L, &xDeviceRes) ;
     DevQueryCaps (hdc, CAPS_VERTICAL_RESOLUTION,   1L, &yDeviceRes) ;

                         // Find desired font size in pixels

     ptlFont.x = 254L * xPointSize * xDeviceRes / 7200000L ;
     ptlFont.y = 254L * yPointSize * yDeviceRes / 7200000L ;

                         // Convert to page units

     GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptlFont) ;

                         // Set the character box

     sizfx.cx = MAKEFIXED (ptlFont.x, 0) ;
     sizfx.cy = MAKEFIXED (ptlFont.y, 0) ;

     return GpiSetCharBox (hps, &sizfx) ;
     }

BOOL ScaleFontToBox (HPS hps, LONG cbText, CHAR *szText, LONG cxBox,
                                                         LONG cyBox)
     {
     POINTL aptl[TXTBOX_COUNT] ;
     SIZEF  sizfx ;

     GpiQueryCharBox (hps, &sizfx) ;
     GpiQueryTextBox (hps, cbText, szText, TXTBOX_COUNT, aptl) ;

     sizfx.cx = sizfx.cx /
          (max (aptl[TXTBOX_TOPRIGHT].x, aptl[TXTBOX_BOTTOMRIGHT].x) -
           min (aptl[TXTBOX_TOPLEFT].x,  aptl[TXTBOX_BOTTOMLEFT].x))
          * cxBox ;

     sizfx.cy = sizfx.cy /
          (max (aptl[TXTBOX_TOPRIGHT].y,    aptl[TXTBOX_TOPLEFT].y) -
           min (aptl[TXTBOX_BOTTOMRIGHT].y, aptl[TXTBOX_BOTTOMLEFT].y))
          * cyBox ;

     return GpiSetCharBox (hps, &sizfx) ;
     }

VOID QueryStartPointInTextBox (HPS hps, LONG cbText, CHAR *szText,
                                        POINTL *pptl)
     {
     POINTL aptl[TXTBOX_COUNT] ;

     GpiQueryTextBox (hps, cbText, szText, TXTBOX_COUNT, aptl) ;

     pptl->x = max(-aptl[TXTBOX_TOPLEFT].x, -aptl[TXTBOX_BOTTOMLEFT].x);
     pptl->y = max(-aptl[TXTBOX_TOPLEFT].y, -aptl[TXTBOX_BOTTOMLEFT].y);
     }

VOID ColorClient (HPS hps, LONG cxClient, LONG cyClient, LONG lColor)
     {
     RECTL rcl ;

     WinSetRect (hab, &rcl, 0, 0, (SHORT) cxClient, (SHORT) cyClient) ;
     WinFillRect (hps, &rcl, lColor) ;
     }
