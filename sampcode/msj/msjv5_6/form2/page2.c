/*
 * Page Selection Control - WINDOW FUNCTION MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This module defines the page control window message processing
 * function.  This function is responsible for interpreting all
 * the commands and generating notification messages.
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 11/30/89 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define  NOCOMM
 
#include <windows.h>
#include <control.h>

#include "page.h"
#include "page.d"

/* instance variable definitions */
#define  PAGE_AUTO         0
#define  PAGE_ROWS         2
#define  PAGE_COLS         4
#define  PAGE_RANGE        6
#define  PAGE_SELECT       8
#define  PAGE_HOFFSET      10
#define  PAGE_VOFFSET      12
#define  PAGE_HSPACING     14
#define  PAGE_VSPACING     16

#define  AUTO              GetWindowWord(hWnd,PAGE_AUTO)
#define  ROWS              GetWindowWord(hWnd,PAGE_ROWS)
#define  COLS              GetWindowWord(hWnd,PAGE_COLS)
#define  RANGE             GetWindowWord(hWnd,PAGE_RANGE)
#define  SELECT            GetWindowWord(hWnd,PAGE_SELECT)
#define  HOFFSET           GetWindowWord(hWnd,PAGE_HOFFSET)
#define  VOFFSET           GetWindowWord(hWnd,PAGE_VOFFSET)
#define  HSPACING          GetWindowWord(hWnd,PAGE_HSPACING)
#define  VSPACING          GetWindowWord(hWnd,PAGE_VSPACING)

#define  SET_AUTO(x)       SetWindowWord(hWnd,PAGE_AUTO,x)
#define  SET_ROWS(x)       SetWindowWord(hWnd,PAGE_ROWS,x)
#define  SET_COLS(x)       SetWindowWord(hWnd,PAGE_COLS,x)
#define  SET_RANGE(x)      SetWindowWord(hWnd,PAGE_RANGE,x)
#define  SET_SELECT(x)     SetWindowWord(hWnd,PAGE_SELECT,x)
#define  SET_HOFFSET(x)    SetWindowWord(hWnd,PAGE_HOFFSET,x)
#define  SET_VOFFSET(x)    SetWindowWord(hWnd,PAGE_VOFFSET,x)
#define  SET_HSPACING(x)   SetWindowWord(hWnd,PAGE_HSPACING,x)
#define  SET_VSPACING(x)   SetWindowWord(hWnd,PAGE_VSPACING,x)

/* local definitions */
#define  PAGE_WIDTH        ((5*GetSystemMetrics(SM_CXHTHUMB))/4)
#define  PAGE_HEIGHT       ((3*GetSystemMetrics(SM_CYVTHUMB))/2)

#define  PAGE_CALCWIDTH    ((25*GetSystemMetrics(SM_CXHTHUMB))/16)
#define  PAGE_CALCHEIGHT   ((10*GetSystemMetrics(SM_CYVTHUMB))/6)

/* local function definitions */
VOID FAR PASCAL   CvtString( WORD, LPSTR );
VOID FAR PASCAL   DrawCaret( HDC, LPRECT );
VOID FAR PASCAL   CalcPage( HWND, LPRECT, WORD );
VOID FAR PASCAL   DrawPage( HDC, LPRECT, WORD, BOOL, BOOL );

/**/

/*
 * PageWndFn( hWnd, wMessage, wParam, lParam ) : LONG;
 *
 *    hWnd           handle to page window
 *    wMessage       message number
 *    wParam         single word parameter
 *    lParam         double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the page selection control window.  Note how the
 * code is written to avoid potential re-entrancy problems.
 *
 */

LONG FAR PASCAL PageWndFn(
   HWND        hWnd,
   WORD        wMessage,
   WORD        wParam,
   LONG        lParam )
{
   LONG        lResult;

   /* initialization */
   lResult = TRUE;

   /* process message */
   switch ( wMessage )
      {
   case WM_CREATE: /* window being created */

      /* define instance variables */
      SET_RANGE( 0 );
      SET_SELECT( 0 );
      SET_AUTO( TRUE );

      break;
   case WM_GETDLGCODE : /* interface query by dialog manager */
      lResult = DLGC_WANTARROWS;
      break;
   case WM_SIZE : /* window being resized */

      /* calculate number of rows & columns */
      SET_COLS(
         (LOWORD(lParam)<PAGE_CALCWIDTH) ?
            1 :
            LOWORD(lParam)/PAGE_CALCWIDTH
      );
      SET_ROWS(
         (HIWORD(lParam)<PAGE_CALCHEIGHT) ?
            1 :
            HIWORD(lParam)/PAGE_CALCHEIGHT
      );

      /* calculate horizontal & vertical page spacing */
      SET_HSPACING( (LOWORD(lParam)-(COLS*PAGE_WIDTH))/(COLS+1) );
      SET_VSPACING( (HIWORD(lParam)-(ROWS*PAGE_HEIGHT))/(ROWS+1) );
      
      /* calculate horizontal & vertical page offsets */
      SET_HOFFSET(
         (LOWORD(lParam)-(COLS*(PAGE_WIDTH+HSPACING))-HSPACING)/2
      );
      SET_VOFFSET(
         (HIWORD(lParam)-(ROWS*(PAGE_HEIGHT+VSPACING))-VSPACING)/2
      );

      /* define range if undefined */
      if ( (AUTO)||(RANGE>ROWS*COLS) ) 
         SET_RANGE( ROWS*COLS );

      break;
   case WM_SETFOCUS: /* receiving the keyboard focus */
   case WM_KILLFOCUS: /* losing the keyboard focus */
      {
         HDC         hDC;
         RECT        rcPage;
         HBRUSH      hNewBrush;
         HBRUSH      hOldBrush;

         /* calculate update region */
         CalcPage( hWnd, &rcPage, SELECT );

         /* force a repaint */
         hDC = GetDC( hWnd );
         if ( hDC ) {
   
            /* define appropriate brush & text colors */
            hNewBrush = (HBRUSH)SendMessage( PARENT(hWnd),
               WM_CTLCOLOR, hDC, MAKELONG(hWnd,CTLCOLOR_BTN) );
            hOldBrush = hNewBrush ?
               SelectObject(hDC,hNewBrush) :
               NULL;

            /* draw caret */
            DrawCaret( hDC, &rcPage );

            /* restore original brush */
            if ( hNewBrush ) {
               SelectObject( hDC, hOldBrush );
               DeleteObject( hNewBrush );
            }

            /* release display context */
            ReleaseDC( hWnd, hDC );

         }

      }
      break;
   case WM_KEYDOWN: /* key pressed */
      {
         HDC         hDC;
         WORD        wSel;
         WORD        wRow;
         WORD        wCol;
         RECT        rcPage;

         /* save current selection */
         wSel = SELECT;
         
         /* calculate current row & column */
         wRow = wSel / COLS;
         wCol = wSel % COLS;
      
         /* process virtual key code */
         switch( wParam )
            {
         case VK_HOME : /* home key */
            SET_SELECT( 0 );
            break;
         case VK_END : /* end key */
            SET_SELECT( RANGE-1 );
            break;
         case VK_UP : /* cursor up key */
            SET_SELECT( (wRow) ? ((wRow-1)*COLS)+wCol : wSel );
            break;
         case VK_PRIOR : /* page up key */
         case VK_LEFT : /* cursor left key */
            SET_SELECT( (wSel) ? wSel-1 : wSel );
            break;
         case VK_NEXT : /* page down key */
         case VK_RIGHT : /* cursor right key */
            SET_SELECT( (wSel<(RANGE-1)) ? wSel+1 : wSel );
            break;
         case VK_DOWN : /* cursor down key */
            SET_SELECT(
               (((wRow+1)*COLS)+wCol<RANGE) ?
                  ((wRow+1)*COLS)+wCol :
                  wSel
            );
            break;
         default : /* something else */
            break;
         }

         /* update window if necessary */
         if ( wSel != SELECT ) {

            /* retrieve display context */
            hDC = GetDC( hWnd );
            if ( hDC ) {
         
               /* update previous page (unselected without caret) */
               CalcPage( hWnd, &rcPage, wSel );
               DrawPage( hDC, &rcPage, wSel, FALSE, FALSE );
            
               /* update new page (selected with caret) */
               CalcPage( hWnd, &rcPage, SELECT );
               DrawPage( hDC, &rcPage, SELECT, TRUE,
                  (GetFocus()==hWnd)?TRUE:FALSE );

               /* release display context */
               ReleaseDC( hWnd, hDC );

               /* notify parent of change */
               SendMessage( PARENT(hWnd), WM_COMMAND, ID(hWnd),
                  MAKELONG(hWnd,PN_SELCHANGE) );

            }

         }

      }
      break;
   case WM_LBUTTONDOWN: /* left mouse button down */
   case WM_LBUTTONDBLCLK: /* left mouse button double-clicked */
      {
         HDC         hDC;
         BOOL        bHit;
         WORD        wPage;
         RECT        rcNewPage;
         RECT        rcOldPage;
         POINT       ptMouse;

         /* define mouse point */
         ptMouse.x = LOWORD(lParam);
         ptMouse.y = HIWORD(lParam);

         /* check each page rectangle */
         for (wPage=0,bHit=FALSE;(!bHit)&&(wPage<RANGE);wPage++) {

            /* calculate page rectangle */
            CalcPage( hWnd, &rcNewPage, wPage );
         
            /* check if point inside a page */
            if ( PtInRect(&rcNewPage,ptMouse) ) {
       
               /* set hit flag */
               bHit = TRUE;

               /* update window if necessary */
               if ( SELECT != wPage ) {
               
                  /* retrieve display context */
                  hDC = GetDC( hWnd );
                  if ( hDC ) {
                     CalcPage( hWnd, &rcOldPage, SELECT );
                     DrawPage( hDC, &rcOldPage, SELECT, FALSE,
                        FALSE );
                     DrawPage( hDC, &rcNewPage, wPage, TRUE,
                        (GetFocus()==hWnd) );
                     ReleaseDC( hWnd, hDC );
                  }

                  /* update selection */
                  SET_SELECT( wPage );

                  /* notify parent of change */
                  SendMessage( PARENT(hWnd), WM_COMMAND, ID(hWnd),
                     MAKELONG(hWnd,PN_SELCHANGE) );

               } 

               /* grab focus if necessary */
               if ( GetFocus() != hWnd )
                  SetFocus( hWnd );

            }

         }

      }
      break;
   case WM_PAINT: /* paint control window */
      {
         PAINTSTRUCT Ps;
         WORD        wPage;
         HPEN        hOldPen;
         HPEN        hNewPen;
         RECT        rcClient;
         HBRUSH      hOldBrush;
         HBRUSH      hNewBrush;

         /* prepare for painting */
         GetClientRect( hWnd, &rcClient );

         /* start paint operation */
         BeginPaint( hWnd, &Ps );

         /* define appropriate pen */
         hNewPen = CreatePen( PS_SOLID, 1,
            GetSysColor(COLOR_WINDOWFRAME) );
         hOldPen = (hNewPen) ? SelectObject(Ps.hdc,hNewPen) : NULL;

         /* define appropriate brush & text colors */
         hNewBrush = (HBRUSH)SendMessage( PARENT(hWnd), WM_CTLCOLOR,
            Ps.hdc, MAKELONG(hWnd,CTLCOLOR_BTN) );
         hOldBrush = hNewBrush ? SelectObject(Ps.hdc,hNewBrush) :
            NULL;

         /* draw border */
         Rectangle(
            Ps.hdc,
            rcClient.left,
            rcClient.top,
            rcClient.right, 
            rcClient.bottom
         );

         /* draw individual pages */
         for ( wPage=0; wPage<RANGE; wPage++ ) {
            CalcPage(
               hWnd,
               &rcClient,
               wPage
            );
            DrawPage(
               Ps.hdc,
               &rcClient,
               wPage,
               (SELECT==wPage),
               (GetFocus()==hWnd)&&(SELECT==wPage)
            );
         }

         /* restore original brush */
         if ( hNewBrush ) {
            SelectObject( Ps.hdc, hOldBrush );
            DeleteObject( hNewBrush );
         }

         /* restore original pen */
         if ( hNewPen ) {
            SelectObject( Ps.hdc, hOldPen );
            DeleteObject( hNewPen );
         }

         /* end paint operation */
         EndPaint( hWnd, &Ps );

      }
      break;
   case PM_SETRANGE : /* set page range */

      /* turn off auto ranging flag */
      SET_AUTO( FALSE );

      /* update range (after validation) */
      SET_RANGE(
         (wParam)&&(wParam<=ROWS*COLS) ? 
         wParam : 
         ROWS*COLS
      );

      /* update window */
      InvalidateRect( hWnd, NULL, TRUE );
      UpdateWindow( hWnd );
   
      break;
   case PM_GETRANGE : /* get page range */
      lResult = RANGE;
      break;
   case PM_SETSELECT : /* set selected page */
      {
         HDC         hDC;
         WORD        wSel;
         RECT        rcPage;

         if ( wParam <= ROWS*COLS ) {
         
            /* retrieve old selection */
            wSel = SELECT;

            /* define new selection */
            SET_SELECT( wParam );

            /* update window if necessary */
            if ( wSel != SELECT ) {

               /* retrieve display context */
               hDC = GetDC( hWnd );
               if ( hDC ) {
         
                  /* update previous page (without caret) */
                  CalcPage( hWnd, &rcPage, wSel );
                  DrawPage( hDC, &rcPage, wSel, FALSE, FALSE );
            
                  /* update new page (selected with caret) */
                  CalcPage( hWnd, &rcPage, SELECT );
                  DrawPage( hDC, &rcPage, SELECT, TRUE, TRUE );

                  /* release display context */
                  ReleaseDC( hWnd, hDC );

               }

            }

         }

         /* return new selection */
         lResult = SELECT;

      }
      break;
   case PM_GETSELECT : /* get selected page */
      lResult = SELECT;
      break;
   default: /* default window message processing */
      lResult = DefWindowProc( hWnd, wMessage, wParam, lParam );
      break;
   }

   /* return final result */
   return( lResult );
}

/**/

/*
 * CalcPage( hWnd, lprcRect, wPage ) : VOID;
 *
 *    hWnd           handle to page selection window
 *    lprcRect       new page rectangle
 *    wPage          desired page number
 *
 * This function calculates a specific page rectangle based
 * on the page number provided.
 *
 */

VOID FAR PASCAL CalcPage(
   HWND        hWnd,
   LPRECT      lprcRect,
   WORD        wPage )
{
   WORD        wRow;
   WORD        wCol;

   /* calculate row and column */
   wRow = wPage / COLS;
   wCol = wPage % COLS;

   /* calculate page rectangle */
   lprcRect->left = (wCol*(HSPACING+PAGE_WIDTH))+HSPACING+HOFFSET;
   lprcRect->top =   (wRow*(VSPACING+PAGE_HEIGHT))+VSPACING+VOFFSET;
   lprcRect->right = ((wCol+1)*(HSPACING+PAGE_WIDTH))+HOFFSET;
   lprcRect->bottom = ((wRow+1)*(VSPACING+PAGE_HEIGHT))+VOFFSET;
   
}

/**/

/*
 * DrawPage( hDC, lprcPage, wNumber, bSelect, bCaret ) : VOID;
 *
 *    hDC            handle to display context
 *    lprcPage       rectangle in which to draw page
 *    wNumber        number to display inside page
 *    bSelect        boolean page selection flag
 *    bCaret         boolean caret flag
 *
 * This function draws a page icon inside the rectangle provided.  If
 * the bSelect flag is TRUE, the page is displayed in inverse video.
 * If the bCaret flag is TRUE, a caret is displayed inside the page.
 *
 */

VOID FAR PASCAL DrawPage(
   HDC         hDC,
   LPRECT      lprcPage,
   WORD        wNumber,
   BOOL        bSelect,
   BOOL        bCaret )
{
   WORD        wFlap;
   HPEN        hOldPen;
   HBRUSH      hOldBrush;
   RECT        rcNumber;
   DWORD       dwOldBkColor;
   DWORD       dwOldTextColor;
   POINT       ptPage[7];
   char        szNumber[16];

   /* calculate page flap */
   wFlap = (lprcPage->right-lprcPage->left)/3;
      
   /* convert number to string */
   CvtString( wNumber+1, szNumber );

   /* define number rectangle */
   rcNumber.top = lprcPage->top + wFlap + 2;
   rcNumber.left = lprcPage->left + 2;
   rcNumber.right = lprcPage->right - 2;
   rcNumber.bottom = lprcPage->bottom - 2;

   /* draw page (based on selection state */
   if ( bSelect ) {
      
      /* define page polygon */
      ptPage[0].x = lprcPage->left;
      ptPage[0].y = lprcPage->top;
      ptPage[1].x = lprcPage->right - wFlap - 1;
      ptPage[1].y = lprcPage->top;
      ptPage[2].x = lprcPage->right - 1;
      ptPage[2].y = lprcPage->top + wFlap;
      ptPage[3].x = lprcPage->right - 1;
      ptPage[3].y = lprcPage->bottom - 1;
      ptPage[4].x = lprcPage->left;
      ptPage[4].y = lprcPage->bottom - 1;
      ptPage[5].x = lprcPage->left;
      ptPage[5].y = lprcPage->top;

      /* draw page outline */
      hOldBrush = SelectObject( hDC,
         CreateSolidBrush(GetSysColor(COLOR_WINDOWFRAME)) );
      Polygon( hDC, ptPage, 6 );
      DeleteObject( SelectObject(hDC,hOldBrush) );

      /* draw flap with background pen color */
      hOldPen = SelectObject( hDC,
         CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOW)) );
      MoveTo( hDC, lprcPage->right - wFlap - 1,
         lprcPage->top + 1 );
      LineTo( hDC, lprcPage->right - wFlap - 1,
         lprcPage->top + wFlap );
      LineTo( hDC, lprcPage->right - 1, lprcPage->top + wFlap );
      DeleteObject( SelectObject(hDC,hOldPen) );

      /* draw inverted page number */
      dwOldBkColor = SetBkColor( hDC,
         GetSysColor(COLOR_WINDOWFRAME) );
      dwOldTextColor = SetTextColor( hDC,
         GetSysColor(COLOR_WINDOW) );
      DrawText(
         hDC,
         szNumber,
         -1,
         &rcNumber,
         DT_CENTER|DT_VCENTER|DT_SINGLELINE
      );
      SetTextColor( hDC, dwOldTextColor );
      SetBkColor( hDC, dwOldBkColor );

   } else {

      /* define page polygon */
      ptPage[0].x = lprcPage->left;
      ptPage[0].y = lprcPage->top;
      ptPage[1].x = lprcPage->right - wFlap - 1;
      ptPage[1].y = lprcPage->top;
      ptPage[2].x = lprcPage->right - wFlap - 1;
      ptPage[2].y = lprcPage->top + wFlap;
      ptPage[3].x = lprcPage->right - 1;
      ptPage[3].y = lprcPage->top + wFlap;
      ptPage[4].x = lprcPage->right - 1;
      ptPage[4].y = lprcPage->bottom - 1;
      ptPage[5].x = lprcPage->left;
      ptPage[5].y = lprcPage->bottom - 1;
      ptPage[6].x = lprcPage->left;
      ptPage[6].y = lprcPage->top;

      /* draw page outline */
      hOldBrush = SelectObject( hDC,
         CreateSolidBrush(GetSysColor(COLOR_BTNFACE)) );
      Polygon( hDC, ptPage, 7 );
      DeleteObject( SelectObject(hDC,hOldBrush) );

      /* draw page flap */
      hOldBrush = SelectObject( hDC,
         CreateSolidBrush(GetSysColor(COLOR_WINDOW)) );
      Polygon( hDC, &ptPage[1], 3 );
      DeleteObject( SelectObject(hDC,hOldBrush) );

      /* draw page number */
      dwOldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
      dwOldTextColor = SetTextColor( hDC,
         GetSysColor(COLOR_BTNTEXT) );
      DrawText(
         hDC,
         szNumber,
         -1,
         &rcNumber,
         DT_CENTER|DT_VCENTER|DT_SINGLELINE
      );
      SetTextColor( hDC, dwOldTextColor );
      SetBkColor( hDC, dwOldBkColor );

      /* draw white border */
      hOldPen = SelectObject( hDC,
         CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOW)) );
      MoveTo( hDC, ptPage[5].x + 1, ptPage[5].y - 1 );
      LineTo( hDC, ptPage[5].x + 1, ptPage[6].y + 1 );
      LineTo( hDC, ptPage[1].x, ptPage[1].y + 1 );
      DeleteObject( SelectObject(hDC,hOldPen) );

      /* draw shadow */
      hOldPen = SelectObject( hDC,
         CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW)) );
      MoveTo( hDC, ptPage[5].x + 1, ptPage[5].y - 1 );
      LineTo( hDC, ptPage[4].x - 1, ptPage[4].y - 1 );
      LineTo( hDC, ptPage[3].x - 1, ptPage[3].y );
      MoveTo( hDC, ptPage[5].x + 2, ptPage[5].y - 2 );
      LineTo( hDC, ptPage[4].x - 2, ptPage[4].y - 2 );
      LineTo( hDC, ptPage[3].x - 2, ptPage[3].y );
      DeleteObject( SelectObject(hDC,hOldPen) );

   }

   /* draw caret (if requested) */
   if ( bCaret )
      DrawCaret( hDC, lprcPage );

}

/**/

/*
 * DrawCaret( hDC, lprcPage ) : VOID;
 *
 *    hDC            handle to display context
 *    lprcPage       rectangle in which to draw caret
 *
 * This function draws a caret inside the page icon using the
 * rectangle provided.  Since a PATINVERT operation is used, the
 * previous caret can be erased by a second call with the same
 * function parameters.
 *
 */

VOID FAR PASCAL DrawCaret(
   HDC         hDC,
   LPRECT      lprcPage )
{
   WORD        wFlap;
   HBRUSH      hOldBrush;

   /* calculate page flap */
   wFlap = (lprcPage->right-lprcPage->left)/3;
      
   /* initialize display context */
   hOldBrush = (HBRUSH)SelectObject( hDC,
      GetStockObject(GRAY_BRUSH) );

   /* draw caret */
   PatBlt( hDC, lprcPage->left+1, lprcPage->top+1,
      (lprcPage->right-lprcPage->left)-wFlap-1, 1, PATINVERT );
   PatBlt( hDC, lprcPage->right-2, lprcPage->top+wFlap+1, 1,
      (lprcPage->bottom-lprcPage->top)-wFlap-3, PATINVERT );
   PatBlt( hDC, lprcPage->left+2, lprcPage->bottom-2,
      (lprcPage->right-lprcPage->left)-3, 1, PATINVERT );
   PatBlt( hDC, lprcPage->left+1, lprcPage->top+2, 1,
      (lprcPage->bottom-lprcPage->top)-3, PATINVERT );

   /* restore display context */
   SelectObject( hDC, hOldBrush );

}

/**/

/*
 * CvtString( wNumber, lpszNumber ) : VOID;
 *
 *    wNumber        number to convert
 *    lpszNumber     long pointer to string
 *
 * This function converts the number provided into a string,
 * assuming an unsigned number.  This function is used by the page
 * drawing routine to display the current page number.
 *
 */

VOID FAR PASCAL CvtString(
   WORD        wNumber, 
   LPSTR       lpszNumber )
{
   WORD        wBase;
   WORD        wRemainder;
   BOOL        bDigit;

   /* initialization */
   wBase = 10000;
   bDigit = FALSE;
   
   /* parse digits */
   while ( wBase ) {

      /* calculate new remainder & update string */
      wRemainder = wNumber / wBase;
      if ( (wRemainder)||(bDigit)||(wBase==1)) {
         bDigit = TRUE;
         *lpszNumber++ = (char)wRemainder + '0';
      }
   
      /* update number & base */
      wNumber -= (wRemainder * wBase);
      wBase = wBase / 10;

   }

   /* add trailing null */
   *lpszNumber = 0;

}
