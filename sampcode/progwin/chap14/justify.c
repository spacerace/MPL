/*----------------------------------------
   JUSTIFY.C -- Justified Type Program
                (c) Charles Petzold, 1990
  ----------------------------------------*/

#include <windows.h>
#include <string.h>
#include "justify.h"

typedef struct
     {
     short nNumFaces ;
     char  szFaceNames [MAX_FACES] [LF_FACESIZE] ;
     }
     ENUMFACE ;

typedef struct
     {
     short      nNumSizes ;
     short      xLogPixPerInch ;
     short      yLogPixPerInch ;
     LOGFONT    lf [MAX_SIZES] ;
     TEXTMETRIC tm [MAX_SIZES] ;
     }
     ENUMSIZE ;

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG) ;
int  FAR PASCAL EnumAllFaces (LPLOGFONT, LPTEXTMETRIC, short, ENUMFACE FAR *) ;
int  FAR PASCAL EnumAllSizes (LPLOGFONT, LPTEXTMETRIC, short, ENUMSIZE FAR *) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static   char szAppName[] = "Justify" ;
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }
     hwnd = CreateWindow (szAppName, "Justified Type",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

int FAR PASCAL EnumAllFaces (LPLOGFONT lplf, LPTEXTMETRIC lptm,
                             short nFontType, ENUMFACE FAR *lpef)
     {
     if (nFontType & RASTER_FONTTYPE)
          {
          lstrcpy (lpef->szFaceNames[lpef->nNumFaces], lplf->lfFaceName) ;
          if (++lpef->nNumFaces == MAX_FACES)
               return 0 ;
          }
     return 1 ;
     }

int FAR PASCAL EnumAllSizes (LPLOGFONT lplf, LPTEXTMETRIC lptm,
                             short nFontType, ENUMSIZE FAR *lpes)
     {
     if (lpes->xLogPixPerInch == lptm->tmDigitizedAspectX &&
         lpes->yLogPixPerInch == lptm->tmDigitizedAspectY)
          {
          lpes->lf [lpes->nNumSizes] = *lplf ;
          lpes->tm [lpes->nNumSizes] = *lptm ;
          if (++lpes->nNumSizes == MAX_SIZES)
               return 0 ;
          }
     return 1 ;
     }

short MakeSizeMenu (HWND hwnd, FARPROC lpfnEnumAllSizes,
                    ENUMSIZE *pes, char *szFaceName)
     {
     static LOGFONT lfBlank ;
     char           szBuffer[20] ;
     HDC            hdc ;
     HMENU          hPopup ;
     short          i ;

     hdc = GetDC (hwnd) ;
     hPopup = GetSubMenu (GetMenu (hwnd), SIZE_MENU) ;

     pes->nNumSizes = 0 ;
     EnumFonts (hdc, szFaceName, lpfnEnumAllSizes, (LPSTR) pes) ;
     ReleaseDC (hwnd, hdc) ;

     while (GetMenuItemCount (hPopup) > 0)
          DeleteMenu (hPopup, 0, MF_BYPOSITION) ;

     if (pes->nNumSizes)
          for (i = 0 ; i < pes->nNumSizes ; i++)
               {
               wsprintf (szBuffer, "%i  %2d / %2d", i + 1,
                    (pes->tm[i].tmHeight - pes->tm[i].tmInternalLeading + 10)
                                                                        / 20,
                    (pes->tm[i].tmHeight + 10) / 20) ;
               AppendMenu (hPopup, 0, IDM_ISIZE + i, szBuffer) ;
               }
     else           /* no fonts found that match aspect ratio of display */
          {
          pes->lf[0] = lfBlank ;
          strcpy (pes->lf[0].lfFaceName, szFaceName) ;
          AppendMenu (hPopup, 0, IDM_ISIZE, "Default") ;
          }

     CheckMenuItem (hPopup, IDM_ISIZE, MF_CHECKED) ;
     return 0 ;
     }

void DrawRuler (HDC hdc, POINT ptClient)
     {
     static short nRuleSize [16] = { 360, 72, 144, 72, 216, 72, 144, 72,
                                     288, 72, 144, 72, 216, 72, 144, 72 } ;
     short        i, j ;

     MoveTo (hdc, 0,          -360) ;
     LineTo (hdc, ptClient.x, -360) ;
     MoveTo (hdc, -360,          0) ;
     LineTo (hdc, -360, ptClient.y) ;

     for (i = 0, j = 0 ; i <= ptClient.x ; i += 1440 / 16, j++)
          {
          MoveTo (hdc, i, -360) ;
          LineTo (hdc, i, -360 - nRuleSize [j % 16]) ;
          }
     for (i = 0, j = 0 ; i <= ptClient.y ; i += 1440 / 16, j++)
          {
          MoveTo (hdc, -360, i) ;
          LineTo (hdc, -360 - nRuleSize [j % 16], i) ;
          }
     }

void Justify (HDC hdc, HANDLE hResource, POINT ptClient, short nCurAlign)
     {
     DWORD  dwExtent ;
     LPSTR  lpText, lpBegin, lpEnd ;
     short  i, xStart, yStart, nBreakCount ;

     lpText = LockResource (hResource) ;

     yStart = 0 ;
     do                            // for each text line
          {
          nBreakCount = 0 ;
          while (*lpText == ' ')   // skip over leading blanks
               lpText++ ;
          lpBegin = lpText ;

          do                       // until the line is known
               {
               lpEnd = lpText ;

               while (*lpText != '\0' && *lpText++ != ' ') ;
               if (*lpText == '\0')
                    break ;
                                   // for each space, calculate extents
               nBreakCount++ ;
               SetTextJustification (hdc, 0, 0) ;
               dwExtent = GetTextExtent (hdc, lpBegin, lpText - lpBegin - 1) ;
               }
          while (LOWORD (dwExtent) < ptClient.x) ;

          nBreakCount-- ;
          while (*(lpEnd - 1) == ' ')   // eliminate trailing blanks
               {
               lpEnd-- ;
               nBreakCount-- ;
               }

          if (*lpText == '\0' || nBreakCount <= 0)
               lpEnd = lpText ;

          SetTextJustification (hdc, 0, 0) ;
          dwExtent = GetTextExtent (hdc, lpBegin, lpEnd - lpBegin) ;

          if (nCurAlign == IDM_LEFT)         // use alignment for xStart
               xStart = 0 ;

          else if (nCurAlign == IDM_RIGHT)
               xStart = ptClient.x - LOWORD (dwExtent) ;

          else if (nCurAlign == IDM_CENTER)
               xStart = (ptClient.x - LOWORD (dwExtent)) / 2 ;

          else
               {
               if (*lpText != '\0' && nBreakCount > 0)
                    SetTextJustification (hdc, ptClient.x - LOWORD (dwExtent),
                                                  nBreakCount) ;
               xStart = 0 ;
               }

          TextOut (hdc, xStart, yStart, lpBegin, lpEnd - lpBegin) ;
          yStart += HIWORD (dwExtent) ;
          lpText = lpEnd ;
          }
     while (*lpText && yStart < ptClient.y) ;

     GlobalUnlock (hResource) ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static ENUMFACE ef ;
     static ENUMSIZE es ;
     static FARPROC  lpfnEnumAllFaces, lpfnEnumAllSizes ;
     static HANDLE   hResource ;
     static POINT    ptClient ;
     static short    nCurSize, nCurFace, nCurAttr, nCurAlign = IDM_LEFT ;
     HANDLE          hInstance ;
     HDC             hdc ;
     HFONT           hFont ;
     HMENU           hMenu, hPopup ;
     PAINTSTRUCT     ps ;
     short           i ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               es.xLogPixPerInch = GetDeviceCaps (hdc, LOGPIXELSX) ;
               es.yLogPixPerInch = GetDeviceCaps (hdc, LOGPIXELSY) ;

                              // Set Map Mode

               SetMapMode (hdc, MM_ANISOTROPIC) ;
               SetWindowExt (hdc, 1440, 1440) ;
               SetViewportExt (hdc, es.xLogPixPerInch, es.yLogPixPerInch) ;
               SetWindowOrg (hdc, -720, -720) ;

                              // MakeProcInstance for 2 routines

               hInstance = ((LPCREATESTRUCT) lParam)-> hInstance ;
               lpfnEnumAllFaces = MakeProcInstance (EnumAllFaces, hInstance) ;
               lpfnEnumAllSizes = MakeProcInstance (EnumAllSizes, hInstance) ;

                              // Enumerate the Font Faces

               EnumFonts (hdc, NULL, lpfnEnumAllFaces, (LPSTR) &ef) ;
               ReleaseDC (hwnd, hdc) ;

                              // Initialize the Menus

               hMenu  = GetMenu (hwnd) ;
               hPopup = CreateMenu () ;

               for (i = 0 ; i < ef.nNumFaces ; i++)
                    AppendMenu (hPopup, 0, IDM_IFACE + i, ef.szFaceNames[i]) ;

               ModifyMenu (hMenu, IDM_FACE, MF_POPUP, hPopup, "&FaceName") ;
               CheckMenuItem (hMenu, IDM_IFACE, MF_CHECKED) ;

               nCurSize = MakeSizeMenu (hwnd, lpfnEnumAllSizes, &es,
                                   ef.szFaceNames [nCurFace]) ;

                              // Load the Text Resource

               hResource = LoadResource (hInstance, 
                           FindResource (hInstance, "Ismael", "TEXT")) ;
               return 0 ;

          case WM_SIZE:
               hdc = GetDC (hwnd) ;
               ptClient = MAKEPOINT (lParam) ;
               DPtoLP (hdc, &ptClient, 1) ;
               ptClient.x -= 360 ;
               ReleaseDC (hwnd, hdc) ;
               return 0 ;

          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               if (wParam >= IDM_IFACE && wParam < IDM_IFACE + MAX_FACES)
                    {
                    CheckMenuItem (hMenu, nCurFace + IDM_IFACE, MF_UNCHECKED) ;
                    CheckMenuItem (hMenu, wParam, MF_CHECKED) ;
                    nCurFace = wParam - IDM_IFACE ;

                    nCurSize = MakeSizeMenu (hwnd, lpfnEnumAllSizes, &es,
                                        ef.szFaceNames [nCurFace]) ;
                    }

               else if (wParam >= IDM_ISIZE && wParam < IDM_ISIZE + MAX_SIZES)
                    {
                    CheckMenuItem (hMenu, nCurSize + IDM_ISIZE, MF_UNCHECKED) ;
                    CheckMenuItem (hMenu, wParam, MF_CHECKED) ;
                    nCurSize = wParam - IDM_ISIZE ;
                    }

               else switch (wParam)
                    {
                    case IDM_BOLD:
                    case IDM_ITALIC:
                    case IDM_STRIKE:
                    case IDM_UNDER:
                         CheckMenuItem (hMenu, wParam, MF_CHECKED &
                              GetMenuState (hMenu, wParam, MF_BYCOMMAND) ?
                                   MF_UNCHECKED : MF_CHECKED) ;
                         nCurAttr ^= wParam ;
                         break ;

                    case IDM_NORM:
                         nCurAttr = 0 ;
                         CheckMenuItem (hMenu, IDM_BOLD,   MF_UNCHECKED) ;
                         CheckMenuItem (hMenu, IDM_ITALIC, MF_UNCHECKED) ;
                         CheckMenuItem (hMenu, IDM_STRIKE, MF_UNCHECKED) ;
                         CheckMenuItem (hMenu, IDM_UNDER,  MF_UNCHECKED) ;
                         break ;

                    case IDM_LEFT:
                    case IDM_RIGHT:
                    case IDM_CENTER:
                    case IDM_JUST:
                         CheckMenuItem (hMenu, nCurAlign, MF_UNCHECKED) ;
                         nCurAlign = wParam ;
                         CheckMenuItem (hMenu, nCurAlign, MF_CHECKED) ;
                         break ;
                    }
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               es.lf[nCurSize].lfWeight    = nCurAttr & IDM_BOLD ? 700 : 400 ;
               es.lf[nCurSize].lfItalic    = (BYTE) (nCurAttr & IDM_ITALIC) ;
               es.lf[nCurSize].lfUnderline = (BYTE) (nCurAttr & IDM_UNDER) ;
               es.lf[nCurSize].lfStrikeOut = (BYTE) (nCurAttr & IDM_STRIKE) ;
                                          
               hFont = CreateFontIndirect (&es.lf[nCurSize]) ;
               hFont = SelectObject (hdc, hFont) ;

               DrawRuler (hdc, ptClient) ;
               Justify (hdc, hResource, ptClient, nCurAlign) ;

               DeleteObject (SelectObject (hdc, hFont)) ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               FreeResource (hResource) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
