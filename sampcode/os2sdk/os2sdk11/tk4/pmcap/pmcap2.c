/*----------------------------------
   PMCAP2.C -- Routines for PMCAP.C
  ----------------------------------*/

#define INCL_WIN
#define INCL_GPI
#define INCL_BITMAPFILEFORMAT
#include <os2.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pmcap.h"

extern CHAR szClientClass [] ;
extern HAB  hab ;

VOID CheckMenuItem (HWND hwndMenu, SHORT idItem, BOOL fCheck)
     {
     WinSendMsg (hwndMenu, MM_SETITEMATTR,
                 MPFROM2SHORT (idItem, TRUE),
                 MPFROM2SHORT (MIA_CHECKED, fCheck ? MIA_CHECKED : 0)) ;
     }

VOID EnableMenuItem (HWND hwndMenu, SHORT idItem, BOOL fEnable)
     {
     WinSendMsg (hwndMenu, MM_SETITEMATTR,
                 MPFROM2SHORT (idItem, TRUE),
                 MPFROM2SHORT (MIA_DISABLED, fEnable ? 0 : MIA_DISABLED)) ;
     }

VOID ErrorMessage (HWND hwnd, SHORT idError)
     {
     CHAR achString [80] ;

     WinLoadString (hab, NULL, idError, sizeof achString, achString) ;

     WinMessageBox (HWND_DESKTOP, hwnd, achString, szClientClass, 0,
                    MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE) ;
     }

VOID AddItemToSysMenu (HWND hwndFrame)
     {
     static CHAR     *szMenuText [2] = { NULL, "~Begin countdown" } ;
     static MENUITEM mi [2] = {
                              MIT_END, MIS_SEPARATOR, 0,         0, NULL, NULL,
                              MIT_END, MIS_TEXT,      0, IDM_BEGIN, NULL, NULL
                              } ;
     HWND            hwndSysMenu, hwndSysSubMenu ;
     MENUITEM        miSysMenu ;
     SHORT           idSysMenu, sItem ;

     hwndSysMenu = WinWindowFromID (hwndFrame, FID_SYSMENU) ;
     idSysMenu   = SHORT1FROMMR (WinSendMsg (hwndSysMenu,
                                             MM_ITEMIDFROMPOSITION,
                                             NULL, NULL)) ;

     WinSendMsg (hwndSysMenu, MM_QUERYITEM,
                 MPFROM2SHORT (idSysMenu, FALSE),
                 MPFROMP (&miSysMenu)) ;

     hwndSysSubMenu = miSysMenu.hwndSubMenu ;

     for (sItem = 0 ; sItem < 2 ; sItem++)
          WinSendMsg (hwndSysSubMenu, MM_INSERTITEM,
                      MPFROMP (mi + sItem),
                      MPFROMP (szMenuText [sItem])) ;
     return ;
     }

VOID SaveColorSettings (BOOL fSave, BOOL fSetMono)
     {
     static LONG clrTableDef [2 * SYSCLR_CSYSCOLORS] ;
     static LONG clrTable [] = { SYSCLR_WINDOWSTATICTEXT, 0x000000,
                                 SYSCLR_SCROLLBAR,        0xC0C0C0,
                                 SYSCLR_BACKGROUND,       0xFFFFFF,
                                 SYSCLR_ACTIVETITLE,      0x000000,
                                 SYSCLR_INACTIVETITLE,    0xFFFFFF,
                                 SYSCLR_MENU,             0xFFFFFF,
                                 SYSCLR_WINDOW,           0xFFFFFF,
                                 SYSCLR_WINDOWFRAME,      0x000000,
                                 SYSCLR_MENUTEXT,         0x000000,
                                 SYSCLR_WINDOWTEXT,       0x000000,
                                 SYSCLR_TITLETEXT,        0xFFFFFF,
                                 SYSCLR_ACTIVEBORDER,     0xA0A0A0,
                                 SYSCLR_INACTIVEBORDER,   0xFFFFFF,
                                 SYSCLR_APPWORKSPACE,     0xE0E0E0,
                                 SYSCLR_HELPBACKGROUND,   0xFFFFFF,
                                 SYSCLR_HELPTEXT,         0x000000,
                                 SYSCLR_HELPHILITE,       0x000000 } ;
     SHORT i ;

     if (fSave)
          for (i = 0 ; i < SYSCLR_CSYSCOLORS ; i++)
               clrTableDef[2*i+1] = WinQuerySysColor (HWND_DESKTOP,
                                        clrTableDef[2*i] = clrTable[2*i], 0L);

     if (fSetMono)
          WinSetSysColors (HWND_DESKTOP, 0L, LCOLF_INDRGB,
                           0L, 2 * SYSCLR_CSYSCOLORS, clrTable) ;
     else
          WinSetSysColors (HWND_DESKTOP, 0L, LCOLF_INDRGB,
                           0L, 2 * SYSCLR_CSYSCOLORS, clrTableDef) ;
     return ;
     }

VOID IncrementFilename (CHAR *pchName)
     {
     SHORT sIndex ;

     for (sIndex = strlen (pchName) - 1 ; sIndex >= 0 ; sIndex--)
          {
          if (pchName [sIndex] == '\\')      // past filename
               return ;

          if (isdigit (pchName [sIndex]))
               break ;
          }

     for ( ; sIndex >= 0 ; sIndex--)
          {
          if (!isdigit (pchName [sIndex]))
               return ;

          if (pchName [sIndex] == '9')
               pchName [sIndex] = '0' ;
          else
               {
               pchName [sIndex] += '\001' ;
               return ;
               }
          }
     return ;
     }

HBITMAP CopyBitmap (HBITMAP hbmSrc)
     {
     BITMAPINFOHEADER bmp ;
     HBITMAP          hbmDst ;
     HDC              hdcSrc, hdcDst ;
     HPS              hpsSrc, hpsDst ;
     POINTL           aptl[3] ;
     SIZEL            sizl ;

     hdcSrc = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;
     hdcDst = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsSrc = GpiCreatePS (hab, hdcSrc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                               GPIT_MICRO | GPIA_ASSOC) ;

     hpsDst = GpiCreatePS (hab, hdcDst, &sizl, PU_PELS    | GPIF_DEFAULT |
                                               GPIT_MICRO | GPIA_ASSOC) ;

     GpiQueryBitmapParameters (hbmSrc, &bmp) ;
     hbmDst = GpiCreateBitmap (hpsDst, &bmp, 0L, NULL, NULL) ;

     if (hbmDst != NULL)
          {
          GpiSetBitmap (hpsSrc, hbmSrc) ;
          GpiSetBitmap (hpsDst, hbmDst) ;

          aptl[0].x = aptl[0].y = 0 ;
          aptl[1].x = bmp.cx ;
          aptl[1].y = bmp.cy ;
          aptl[2]   = aptl[0] ;

          GpiBitBlt (hpsDst, hpsSrc, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;
          }
     GpiDestroyPS (hpsSrc) ;
     GpiDestroyPS (hpsDst) ;
     DevCloseDC (hdcSrc) ;
     DevCloseDC (hdcDst) ;

     return hbmDst ;
     }

VOID CopyPointerToScreen (HPS hpsScreen, LONG alBitmapFormats[], BOOL fCopy)
     {
     static HBITMAP     hbm ;
     static HDC         hdcMemory ;
     static HPS         hpsMemory ;
     static POINTERINFO ptri ;
     static POINTL      ptlPointer, aptl[3] ;
     static SHORT       cxPointer, cyPointer ;
     BITMAPINFOHEADER   bmp ;
     HPOINTER           hptr ;
     SIZEL              sizl ;

     if (WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT) == 0L)
          return ;

     if (fCopy)
          {
                                   // Get Pointer Information

          cxPointer = (SHORT) WinQuerySysValue (HWND_DESKTOP, SV_CXPOINTER) ;
          cyPointer = (SHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYPOINTER) ;

          hptr = WinQueryPointer (HWND_DESKTOP) ;
          WinQueryPointerInfo (hptr, &ptri) ;
          WinQueryPointerPos (HWND_DESKTOP, &ptlPointer) ;

                                   // Create memory DC and PS

          hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

          sizl.cx = sizl.cy = 0 ;
          hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl,
                                   PU_PELS    | GPIF_DEFAULT |
                                   GPIT_MICRO | GPIA_ASSOC) ;

                                   // Create bitmap for destination

          bmp.cbFix     = sizeof bmp ;
          bmp.cx        = cxPointer ;
          bmp.cy        = cyPointer ;
          bmp.cPlanes   = (USHORT) alBitmapFormats[0] ;
          bmp.cBitCount = (USHORT) alBitmapFormats[1] ;

          hbm = GpiCreateBitmap (hpsMemory, &bmp, 0L, NULL, NULL) ;

                                   // Copy from screen to bitmap

          GpiSetBitmap (hpsMemory, hbm) ;

          aptl[0].x = 0 ;
          aptl[0].y = 0 ;
          aptl[1].x = cxPointer ;
          aptl[1].y = cyPointer ;
          aptl[2].x = ptlPointer.x - ptri.xHotspot ;
          aptl[2].y = ptlPointer.y - ptri.yHotspot ;

          GpiBitBlt (hpsMemory, hpsScreen, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;

                                   // Draw pointer on screen

          WinDrawPointer (hpsScreen, (SHORT) aptl[2].x, (SHORT) aptl[2].y,
                          hptr, DP_NORMAL) ;
          }
     else
          {
                                   // Copy from bitmap to screen

          aptl[0].x = ptlPointer.x - ptri.xHotspot ;
          aptl[0].y = ptlPointer.y - ptri.yHotspot ;
          aptl[1].x = aptl[0].x + cxPointer ;
          aptl[1].y = aptl[0].y + cyPointer ;
          aptl[2].x = 0 ;
          aptl[2].y = 0 ;

          GpiBitBlt (hpsScreen, hpsMemory, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE) ;

                                   // Clean up

          GpiSetBitmap (hpsMemory, NULL) ;
          GpiDestroyPS (hpsMemory) ;
          DevCloseDC (hdcMemory) ;
          GpiDeleteBitmap (hbm) ;
          }
     }

HBITMAP ScreenToBitmap (SHORT cxScreen, SHORT cyScreen, BOOL fIncludePtr,
                                                        BOOL fMonochrome)
     {
     BITMAPINFOHEADER bmp ;
     HBITMAP          hbm ;
     HDC              hdcMemory ;
     HPS              hpsScreen, hpsMemory ;
     LONG             alBitmapFormats [2] ;
     POINTL           aptl[3] ;
     SIZEL            sizl ;

                                   // Create memory DC and PS

     hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl,
                              PU_PELS    | GPIF_DEFAULT |
                              GPIT_MICRO | GPIA_ASSOC) ;

                                   // Create bitmap for destination


     bmp.cbFix     = sizeof bmp ;
     bmp.cx        = cxScreen ;
     bmp.cy        = cyScreen ;

     if (fMonochrome)
          {
          bmp.cPlanes   = 1 ;
          bmp.cBitCount = 1 ;
          }
     else
          {
          GpiQueryDeviceBitmapFormats (hpsMemory, 2L, alBitmapFormats) ;

          bmp.cPlanes   = (USHORT) alBitmapFormats[0] ;
          bmp.cBitCount = (USHORT) alBitmapFormats[1] ;
          }

     hbm = GpiCreateBitmap (hpsMemory, &bmp, 0L, NULL, NULL) ;

                                   // Copy from screen to bitmap

     if (hbm != NULL)
          {
          GpiSetBitmap (hpsMemory, hbm) ;
          hpsScreen = WinGetScreenPS (HWND_DESKTOP) ;

          aptl[0].x = 0 ;
          aptl[0].y = 0 ;
          aptl[1].x = cxScreen ;
          aptl[1].y = cyScreen ;
          aptl[2].x = 0 ;
          aptl[2].y = 0 ;

          if (fIncludePtr)
               CopyPointerToScreen (hpsScreen, alBitmapFormats, TRUE) ;

          WinLockVisRegions (HWND_DESKTOP, TRUE) ;

          GpiBitBlt (hpsMemory, hpsScreen, 3L, aptl,
                     fMonochrome ? ROP_NOTSRCCOPY : ROP_SRCCOPY, BBO_IGNORE) ;

          WinLockVisRegions (HWND_DESKTOP, FALSE) ;

          if (fIncludePtr)
               CopyPointerToScreen (hpsScreen, alBitmapFormats, FALSE) ;

          WinReleasePS (hpsScreen) ;
          GpiDestroyPS (hpsMemory) ;
          DevCloseDC (hdcMemory) ;
          }

     return hbm ;
     }

BOOL IsBitmapMonoEGA (HBITMAP hbm)
     {
     BITMAPINFOHEADER bmp ;

     if (hbm != NULL)
          {
          GpiQueryBitmapParameters (hbm, &bmp) ;

          if (bmp.cx        == 640 &&
              bmp.cy        == 350 &&
              bmp.cPlanes   == 1   &&
              bmp.cBitCount == 1)

               return TRUE ;
          }
     return FALSE ;
     }

SHORT SaveBitmap (HBITMAP hbm, CHAR *szFilename)
     {
     BITMAPFILEHEADER bfh ;
     BITMAPINFO       *pbmi ;
     BYTE             *pbScan ;
     FILE             *file ;
     HDC              hdcMemory ;
     HPS              hpsMemory ;
     LONG             lBmpDataSize ;
     SHORT            sRgbTableSize, sScanLineSize, sScan ;
     SIZEL            sizl ;

                              // Get bitmap information

     bfh.bmp.cbFix = sizeof (BITMAPINFOHEADER) ;
     GpiQueryBitmapParameters (hbm, &bfh.bmp) ;

     if (bfh.bmp.cPlanes != 1)
          return IDS_MULTIPLANE ;

     sRgbTableSize = (1 << bfh.bmp.cPlanes * bfh.bmp.cBitCount) * sizeof (RGB);
     sScanLineSize = ((bfh.bmp.cBitCount * bfh.bmp.cx + 31) / 32) * 4 *
                       bfh.bmp.cPlanes ;
     lBmpDataSize  = (LONG) sScanLineSize * bfh.bmp.cy ;

                              // Open file

     if (NULL == (file = fopen (szFilename, "wb")))
          return IDS_FILEOPEN ;

                              // Set up file header and write it to file

     bfh.usType   = BFT_BMAP ;
     bfh.cbSize   = sizeof (BITMAPFILEHEADER) + sRgbTableSize + lBmpDataSize ;
     bfh.xHotspot = 0 ;
     bfh.yHotspot = 0 ;
     bfh.offBits  = sizeof (BITMAPFILEHEADER) + sRgbTableSize ;

     fwrite (&bfh, sizeof bfh, 1, file) ;

                              // Create memory DC and PS, and set bitmap in it

     hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl, PU_PELS | GPIF_DEFAULT |
                                                     GPIT_MICRO | GPIA_ASSOC) ;

     GpiSetBitmap (hpsMemory, hbm) ;

                              // Allocate memory for BITMAPINFO table & scans

     pbmi = malloc (sizeof (BITMAPINFOHEADER) + sRgbTableSize) ;

     pbmi->cbFix     = sizeof (BITMAPINFOHEADER) ;
     pbmi->cPlanes   = bfh.bmp.cPlanes ;
     pbmi->cBitCount = bfh.bmp.cBitCount ;

     pbScan = malloc (sScanLineSize) ;

                              // Loop through scan lines

     for (sScan = 0 ; sScan < bfh.bmp.cy ; sScan++)
          {
          GpiQueryBitmapBits (hpsMemory, (LONG) sScan, 1L, pbScan, pbmi) ;

          if (sScan == 0)
               fwrite (&pbmi->argbColor[0], sRgbTableSize, 1, file) ;

          if (fwrite (pbScan, sScanLineSize, 1, file) == 0)
               break ;
          }
                              // Cleanup after completion

     fclose (file) ;
     free (pbmi) ;
     free (pbScan) ;
     GpiDestroyPS (hpsMemory) ;
     DevCloseDC (hdcMemory) ;

     if (sScan != bfh.bmp.cy)
          {
          unlink (szFilename) ;
          return IDS_DISKFULL ;
          }

     return 0 ;
     }

typedef struct
     {
     SHORT key1 ;
     SHORT key2 ;
     SHORT dxFile ;
     SHORT dyFile ;
     SHORT ScrAspectX ;
     SHORT ScrAspectY ;
     SHORT PrnAspectX ;
     SHORT PrnAspectY ;
     SHORT dxPrinter ;
     SHORT dyPrinter ;
     SHORT AspCorX ;
     SHORT AspCorY ;
     SHORT wCheck ;
     SHORT res1 ;
     SHORT res2 ;
     SHORT res3 ;
     SHORT bitmap[350][40] ;
     }
     PAINT ;

SHORT SavePaintFormat (HBITMAP hbm, CHAR *szFilename)
     {
     BITMAPINFO *pbmi ;
     FILE       *file ;
     HDC        hdcMemory ;
     HPS        hpsMemory ;
     PAINT      *ppaint ;
     SHORT      i, j, sHold, sWrite ;
     SIZEL      sizl ;
                              // Allocate memory and open file

     if (!IsBitmapMonoEGA (hbm))
          return IDS_MONOEGA ;

     if (NULL == (ppaint = malloc (sizeof (PAINT))))
          return IDS_MEMORY ;

     if (NULL == (file = fopen (szFilename, "wb")))
          {
          free (ppaint) ;
          return IDS_FILEOPEN ;
          }
                              // Set up Paint header for EGA

     ppaint->key1       = 0x6144 ;
     ppaint->key2       = 0x4D6E ;
     ppaint->dxFile     = 0x0280 ;
     ppaint->dyFile     = 0x015E ;
     ppaint->ScrAspectX = 0x0026 ;
     ppaint->ScrAspectY = 0x0030 ;
     ppaint->PrnAspectX = 0x0026 ;
     ppaint->PrnAspectY = 0x0030 ;
     ppaint->dxPrinter  = 0x0280 ;
     ppaint->dyPrinter  = 0x015E ;
     ppaint->AspCorX    = 0x0000 ;
     ppaint->AspCorY    = 0x0000 ;
     ppaint->wCheck     = 0x2C2A ;
     ppaint->res1       = 0xB80E ;
     ppaint->res2       = 0x0050 ;
     ppaint->res3       = 0x8D50 ;

                              // Create memory DC and PS, and set bitmap in it

     hdcMemory = DevOpenDC (hab, OD_MEMORY, "*", 0L, NULL, NULL) ;

     sizl.cx = sizl.cy = 0 ;
     hpsMemory = GpiCreatePS (hab, hdcMemory, &sizl, PU_PELS | GPIF_DEFAULT |
                                                     GPIT_MICRO | GPIA_ASSOC) ;

     GpiSetBitmap (hpsMemory, hbm) ;

                              // Allocate memory for bitmap header info

     pbmi = malloc (sizeof (BITMAPINFO) + sizeof (RGB)) ;

     pbmi->cbFix     = sizeof (BITMAPINFOHEADER) ;
     pbmi->cx        = 640 ;
     pbmi->cy        = 350 ;
     pbmi->cPlanes   = 1 ;
     pbmi->cBitCount = 1 ;
                              // Get bitmap bits

     GpiQueryBitmapBits (hpsMemory, 0L, 350L, (PBYTE) ppaint->bitmap, pbmi) ;

     free (pbmi) ;
                              // Convert to paint format and write

     for (i = 0 ; i < 350 / 2 ; i++)
          for (j = 0 ; j < 40 ; j++)
               {
               sHold = ~ppaint->bitmap[i][j] ;
               ppaint->bitmap[i][j] = ~ppaint->bitmap[350-i-1][j] ;
               ppaint->bitmap[350-i-1][j] = sHold ;
               }

     sWrite = fwrite (ppaint, sizeof (PAINT), 1, file) ;

                              // Clean up after completion
     fclose (file) ;
     free (ppaint) ;
     GpiDestroyPS (hpsMemory) ;
     DevCloseDC (hdcMemory) ;

     if (sWrite == 0)
          {
          unlink (szFilename) ;
          return IDS_DISKFULL ;
          }
     return 0 ;
     }
