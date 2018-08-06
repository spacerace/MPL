/****************************** Module Header *********************************/
/*									      */
/* Module Name:  calc.c - Calc application				      */
/*									      */
/* OS/2 Presentation Manager version of Calc, ported from Windows version     */
/*									      */
/* Created by Microsoft Corporation, 1987				      */
/*									      */
/******************************************************************************/

#define INCL_WININPUT
#define INCL_WINPOINTERS
#define INCL_WINMENUS
#define INCL_WINSYS
#define INCL_WINCLIPBOARD
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_GPILCIDS
#define INCL_DEV
#define INCL_ERRORS
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSNLS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "calc.h"

/******************************************************************************/
/*									      */
/*  GLOBAL VARIABLES							      */
/*									      */
/******************************************************************************/

CHAR  chLastKey, chCurrKey;
CHAR  szreg1[20], szreg2[20], szmem[20], szregx[20];
CHAR  szTitle[30], szErrorString[20], szPlusMinus[2];
SHORT sCharWidth, sCharHeight;
extern BOOL fError;
BOOL  fValueInMemory = FALSE;
BOOL  fMDown = FALSE;		       /* TRUE iff 'm' key depressed  */
UCHAR uchMScan = 0;		       /* scan code for 'm' key       */

#define TOLOWER(x)   ( (((x) >= 'A') && ((x) <= 'Z')) ? (x)|0x20 : (x))
#define WIDTHCONST  28
#define CXCHARS     37
#define CYCHARS     13

HAB hab;
HDC hdcLocal;			    /* Local used for button bitmap */
HPS hpsLocal;
HDC hdcSqr;			    /* Sqr used for square-root bitmap */
HPS hpsSqr;
HBITMAP hbmLocal, hbmSqr;
HMQ  hmqCalc;
HWND hwndCalc, hwndMenu;
HWND hwndCalcFrame;
HPS  hpsCalc;
HDC  hdcCalc;
HPOINTER hptrFinger;

DEVOPENSTRUC dop =		    /* used by DevOpenDC */
{
    NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

static char achKeys[25] =	       /* keyboard keys */
{
    '\271', '0', '.', '\261', '+', '=',
    '\272', '1', '2', '3', '-', 'c',
    '\273', '4', '5', '6', '*', '%',
    '\274', '7', '8', '9', '/', 'q',
    NULL
};

static CHAR achDKeys[25] =    /* 4th key is plusminus */
{
    ' ', '0', '.', '+', '+', '=',
    ' ', '1', '2', '3', '-', 'C',
    ' ', '4', '5', '6', '*', '%',
    ' ', '7', '8', '9', '/', ' ',
    NULL
};

/******************************************************************************/
/*									      */
/*  PROCEDURE DECLARATIONS						      */
/*									      */
/******************************************************************************/

VOID FarStrcpy( PSZ, PSZ);
MPARAM CALLBACK AboutDlgProc( HWND, USHORT, MPARAM, MPARAM);
BOOL CalcInit(VOID);
VOID CalcPaint( HWND, HPS);
VOID CalcTextOut( HPS, INT, INT, PCH, INT);
MRESULT CALLBACK CalcWndProc( HWND, USHORT, MPARAM, MPARAM);
VOID cdecl main(VOID);
VOID DataXCopy( VOID);
VOID DataXPaste( VOID);
VOID DrawNumbers( HPS);
VOID Evaluate(BYTE);
BOOL FlashSqr( HPS, PWPOINT);
VOID FlipKey( HPS, INT, INT);
VOID FrameKey( HPS, INT, INT);
VOID InitCalc( VOID);
BOOL InterpretChar( CHAR);
VOID ProcessKey( PWPOINT);
BOOL PSInit( VOID);
CHAR Translate( PWPOINT);
VOID UpdateDisplay( VOID);


/******************************************************************************/
/******************************************************************************/
VOID CalcTextOut( hps, iX, iY, pch, iCount)

HPS hps;
INT iX, iY;
PCH pch;
INT iCount;
{
    POINTL ptl;

    ptl.x = iX;
    ptl.y = iY;

    GpiSetColor( hps, CLR_BLACK);
    GpiCharStringAt( hps, (PPOINTL)&ptl, (LONG)iCount, (PSZ)pch);
}


/******************************************************************************/
/* Write the appropriate number or error string to the display area	      */
/* and mark memory-in-use if appropriate.				      */
/******************************************************************************/
VOID
UpdateDisplay()
{
    RECTL rcl;

    rcl.xLeft = (6 * sCharWidth);
    rcl.yBottom = 1050 * sCharHeight / 100;
    rcl.xRight = rcl.xLeft + (12 * sCharWidth);
    rcl.yTop = rcl.yBottom + (3 * sCharHeight) / 2;

    WinFillRect( hpsCalc, &rcl, CLR_WHITE);	 /* paint display area white */
    if( fError)
	WinDrawText( hpsCalc
		   , -1
		   , szErrorString
		   , &rcl
		   , CLR_BLACK
		   , CLR_WHITE
		   , DT_RIGHT | DT_VCENTER );
    else
	WinDrawText( hpsCalc
		   , -1
		   , szreg1
		   , &rcl
		   , CLR_BLACK
		   , CLR_WHITE
		   , DT_RIGHT | DT_VCENTER );

    if (fValueInMemory) 		/* little black square shows mem use */
    {
	rcl.xLeft = (6 * sCharWidth);
	rcl.yBottom = 1050 * sCharHeight / 100;
	rcl.xRight = rcl.xLeft + (sCharWidth / 2);
	rcl.yTop = rcl.yBottom + (sCharHeight / 2);
	WinFillRect( hpsCalc, &rcl, CLR_BLACK);
    }
}


/******************************************************************************/
/*  Display helpful info						      */
/******************************************************************************/
MPARAM CALLBACK
AboutDlgProc( hwnd, msg, mp1, mp2)

HWND   hwnd;
USHORT msg;
MPARAM mp1;
MPARAM mp2;
{
    if (msg == WM_COMMAND)
    {
        WinDismissDlg(hwnd, TRUE);
	return(MPFROMSHORT(TRUE));
    }
    else return(WinDefDlgProc( hwnd, msg, mp1, mp2));
}


/******************************************************************************/
/*  General initialization						      */
/******************************************************************************/
BOOL
CalcInit()
{
    hab = WinInitialize( NULL);

    hmqCalc = WinCreateMsgQueue( hab, 0);
    if( !hmqCalc)
        return(FALSE);

    WinLoadString( NULL, NULL, 1, 30, (PSZ)szTitle);
    WinLoadString( NULL, NULL, 2, 20, (PSZ)szErrorString);
    WinLoadString( NULL, NULL, 3, 2, (PSZ)szPlusMinus);

    if (!WinRegisterClass( hab, szTitle, CalcWndProc, CS_SIZEREDRAW, 0))
        return(FALSE);

    hptrFinger = WinLoadPointer( HWND_DESKTOP, (HMODULE)NULL, IDP_FINGER);

    InitCalc(); 			/* arithmetic initialization */

    return(TRUE);
}

/******************************************************************************/
/*  main procedure							      */
/******************************************************************************/
VOID cdecl
main()
{
    QMSG  qmsg;
    ULONG ulFCF;

    if (!CalcInit()) {			    /* general initialization */
	WinAlarm(HWND_DESKTOP, WA_ERROR);
        goto exit;
    }

    if (!PSInit()) {			    /* presentation spaces & bitmaps */
	WinAlarm(HWND_DESKTOP, WA_ERROR);
        goto exit;
    }

    ulFCF = FCF_STANDARD & ~(LONG)(FCF_SIZEBORDER | FCF_MAXBUTTON);
    hwndCalcFrame = WinCreateStdWindow( HWND_DESKTOP
				      , WS_VISIBLE | FS_BORDER
				      , &ulFCF
				      , szTitle
				      , NULL
				      , 0L
				      , NULL
				      , IDR_CALC
				      , &hwndCalc);

    WinSetWindowPos( hwndCalcFrame
		   , (HWND)NULL
		   , 2
		   , 2
		   , CXCHARS * sCharWidth
		   , CYCHARS * sCharHeight
			     + (SHORT)WinQuerySysValue( HWND_DESKTOP
						      , SV_CYTITLEBAR )
			     + (SHORT)WinQuerySysValue( HWND_DESKTOP
						      , SV_CYMENU )
		   , SWP_MOVE | SWP_SIZE );

    while (WinGetMsg( hab, &qmsg, NULL, 0, 0))
	WinDispatchMsg( hab, &qmsg);

exit:					    /* clean up */
    if (hdcSqr) 			    /* square-root bitmap */
    {
	GpiDestroyPS( hpsSqr);
	if (hbmSqr)
	    GpiDeleteBitmap( hbmSqr);
    }

    if (hdcLocal)			    /* keypad button */
    {
	GpiDestroyPS( hpsLocal);
	if (hbmLocal)
	    GpiDeleteBitmap( hbmLocal);
    }

    WinDestroyWindow(hwndCalcFrame);

    WinDestroyMsgQueue(hmqCalc);
    WinTerminate(hab);

    DosExit(EXIT_PROCESS, 0);		    /* exit without error */
}


/******************************************************************************/
/* Calc Window Procedure						      */
/******************************************************************************/
MRESULT CALLBACK
CalcWndProc(hwnd, msg, mp1, mp2)

HWND	hwnd;
USHORT	msg;
MPARAM	mp1;
MPARAM	mp2;
{
    HPS     hps;
    RECTL   rclPaint;
    WPOINT  wpt;
    BOOL    fClip;
    USHORT  usFmtInfo;
    RECTL   rcl;
    SIZEL   sizl;

    switch (msg)
    {
    case WM_CREATE:
	hdcCalc = WinOpenWindowDC( hwnd);
	WinQueryWindowRect( hwnd, &rcl);
	sizl.cx = rcl.xRight - rcl.xLeft;
	sizl.cy = rcl.yTop - rcl.yBottom;
	hpsCalc = GpiCreatePS( hab
			     , hdcCalc
			     , &sizl
			     , GPIA_ASSOC | PU_PELS );
        break;

    case WM_DESTROY:
        WinDestroyPointer(hptrFinger);
	GpiDestroyPS( hpsSqr);
	GpiDeleteBitmap( hbmSqr);
	GpiDestroyPS( hpsLocal);
	GpiDeleteBitmap( hbmLocal);
        break;

    case WM_INITMENU:
        fClip = FALSE;
	if (WinOpenClipbrd( hab))
        {
	    fClip = WinQueryClipbrdFmtInfo( hab, CF_TEXT, &usFmtInfo);
	    WinCloseClipbrd( hab);
        }
	WinSendMsg((HWND)mp2, MM_SETITEMATTR,
		   (MPARAM) MAKELONG(CMD_PASTE, TRUE),
		   (MPARAM) MAKELONG(MIA_DISABLED, fClip ? 0 : MIA_DISABLED));
        break;

    case WM_PAINT:
	hps = WinBeginPaint(hwnd, NULL, &rclPaint);
	CalcPaint( hwnd, hps);			    /* re-draw calculator */
        WinEndPaint(hps);
        break;

    case WM_COMMAND:
	if (fError)
            break;
        switch(LOUSHORT(mp1))
        {
        case CMD_COPY:
	    DataXCopy();		    /* copy to clipboard */
            break;
        case CMD_PASTE:
	    DataXPaste();		    /* paste from clipboard */
            break;
        case CMD_EXIT:
	    WinPostMsg( hwndCalcFrame, WM_QUIT, 0L, 0L);
            break;
        case CMD_ABOUT:
	    WinDlgBox( HWND_DESKTOP
		     , hwndCalcFrame
		     , (PFNWP)AboutDlgProc
		     , NULL
		     , 1
		     , (PSZ)NULL );
            break;
        }
        break;

    case WM_MOUSEMOVE:
	WinSetPointer( HWND_DESKTOP, hptrFinger);
        break;

    case WM_BUTTON1DOWN:
	wpt.x = LOUSHORT(mp1);
	wpt.y = HIUSHORT(mp1);
	ProcessKey( &wpt);
        goto dwp;
        break;

    case WM_CHAR:
	if (SHORT1FROMMP(mp1) & KC_KEYUP)
	{
	    if (CHAR4FROMMP(mp1) == uchMScan)
		fMDown = FALSE; 		/* 'm' key went up */
	}
	else if (SHORT1FROMMP(mp1) & KC_CHAR)
        {
	    if (InterpretChar(CHAR1FROMMP(mp2)))
		UpdateDisplay();
	    else if ((CHAR1FROMMP(mp2)=='m') || (CHAR1FROMMP(mp2)=='M'))
	    {
		uchMScan = CHAR4FROMMP(mp1);	   /* save 'm' key scan code  */
		fMDown = TRUE;			   /* 'm' key went down       */
	    }
        }
        break;

    case WM_ACTIVATE:
        if (HIUSHORT(mp1))
	    WinSetFocus( HWND_DESKTOP, hwndCalc);
	break;

    case WM_SETFOCUS:
	if ((HWNDFROMMP(mp1)==hwndCalc) && !mp2);
	    fMDown = FALSE;			/* since we are losing focus */
	break;

dwp:
    default:
        return(WinDefWindowProc(hwnd, msg, mp1, mp2));
        break;
    }
    return(0L);
}


/******************************************************************************/
/*  translate & interpret keys (ie. locate in logical keyboard) 	      */
/******************************************************************************/
BOOL
InterpretChar( ch)

CHAR ch;
{
    BOOL fDone;
    NPCH pchStep;
    INT  i;

    fDone = FALSE;
    pchStep = achKeys;
    switch (ch)
    {
    case 'n':
	ch = szPlusMinus[0];
        break;
    case 27:                        /* xlate Escape into 'c' */
        ch = 'c';
        break;
    case '\r':                      /* xlate Enter into '=' */
        ch = '=';
        break;
    }

    if (fMDown) 		    /* Do memory keys */
    {
        switch (ch)
        {
        case 'c':
        case 'C':
            ch = '\274';
            break;
        case 'r':
        case 'R':
            ch = '\273';
            break;
        case '+':
            ch = '\272';
            break;
        case '-':
            ch = '\271';
            break;
        }
    }

    while (!fDone && *pchStep)
    {
	if (*pchStep++ == ch)
	    fDone = TRUE;		/* char found in logical keyboard */
    }
    if (fDone)
    {
	chLastKey = chCurrKey;
	i = pchStep - achKeys - 1;
	FlipKey( hpsCalc, i/6, i%6);
	Evaluate( achKeys[i]);
    }
    return (fDone);
}


/******************************************************************************/
/*  briefly reverse the shading on one of the keys			      */
/******************************************************************************/
VOID
FlipKey( hps, iRow, iCol)

HPS hps;
INT iRow, iCol;
{
    RECTL rcl;

    rcl.xLeft = (iCol * 6 * sCharWidth) + (14 * sCharWidth / 10);
    rcl.yBottom = (165 * sCharHeight / 100) + (2 * iRow * sCharHeight);
    rcl.xRight = rcl.xLeft + (11 * sCharWidth / 3);
    rcl.yTop = rcl.yBottom + (7 * sCharHeight / 4);
    WinInvertRect( hps, &rcl);
    DosSleep( 50L);
    WinInvertRect( hps, &rcl);
}


/******************************************************************************/
/*  compute whether a point is over a button and flash the button if so       */
/******************************************************************************/
BOOL
FlashSqr( hps, pwpt)

HPS	 hps;
PWPOINT  pwpt;
{
    INT  iRow, iCol;
    BOOL fDone;

    /* find x range */
    fDone = FALSE;
    iCol = 0;
    iRow = 3;
    while (!fDone && iCol<6)
    {
	if (pwpt->x <	(iCol * 6 * sCharWidth)
		       + (14 * sCharWidth / 10)
		       + (11*sCharWidth/3)	 )
        {
	    if (pwpt->x > (iCol * 6 * sCharWidth) + (14 * sCharWidth / 10))
                fDone = TRUE;
            else
                return FALSE;
        }
        else
	    iCol++;
    }
    if (!fDone)
        return FALSE;
    fDone = FALSE;
    while (!fDone && iRow >= 0)
    {
	if (pwpt->y > ((165 * sCharHeight / 100) + (2 * iRow * sCharHeight)))
        {
	    if (pwpt->y <   (165 * sCharHeight / 100)
			   + (2 * iRow * sCharHeight)
			   + (7 * sCharHeight / 4)     )
                fDone = TRUE;
            else
                return FALSE;
        }
        else
	    iRow--;
    }
    if (!fDone)
        return FALSE;
    pwpt->x = iCol;
    pwpt->y = iRow;
    FlipKey( hps, iRow, iCol);
    return TRUE;
}


/******************************************************************************/
/*  which key is point on?						      */
/******************************************************************************/
CHAR
Translate( pwpt)

PWPOINT pwpt;
{
    return( achKeys[ pwpt->y * 6 + pwpt->x]);
}


/******************************************************************************/
/*  invoke flashing, point-to-key translation, and result-display update      */
/******************************************************************************/
VOID
ProcessKey( pwpt)

PWPOINT pwpt;
{
    BOOL fFlashed;

    chLastKey = chCurrKey;
    fFlashed = FlashSqr( hpsCalc, pwpt);

    if (fFlashed)
	Evaluate( (BYTE)Translate( pwpt));
    UpdateDisplay();
}


/******************************************************************************/
/*  draw a blank key							      */
/******************************************************************************/
VOID
FrameKey(hps, iRow, iCol)

HPS hps;
INT iRow, iCol;
{
    POINTL aptl[3];

    aptl[0].x = (iCol * 6 * sCharWidth) + (14 * sCharWidth / 10);
    aptl[0].y = (165 * sCharHeight / 100) + (2 * iRow * sCharHeight);
    aptl[1].x = (11 * sCharWidth / 3) + (aptl[0].x);
    aptl[1].y = (7 * sCharHeight / 4) + (aptl[0].y);
    aptl[2].x = 0;
    aptl[2].y = 0;
    GpiBitBlt( hps, hpsLocal, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);
}


/******************************************************************************/
/*  draw the keys and fill in numbers					      */
/******************************************************************************/
VOID
DrawNumbers(hps)

HPS hps;
{
    INT iRow, iCol;

    /* Draw the keys and fill in the numbers we can */
    for (iRow = 0; iRow < 4; iRow++)
    {
	for (iCol = 0; iCol < 6; iCol++)
        {
	    FrameKey( hps, iRow, iCol);
	    CalcTextOut( hps
		       ,   (iCol * 6 * sCharWidth)
			 + (WIDTHCONST * sCharWidth / 10)
		       , (iRow + 1) * 2 * sCharHeight
		       , (PSZ)(achDKeys + (iRow * 6) + iCol)
		       , 1 );
        }
    }
}


/******************************************************************************/
/*  redraw the whole calculator 					      */
/******************************************************************************/
VOID
CalcPaint( hwnd, hps)

HWND hwnd;
HPS  hps;
{
    RECTL      rclDst;
    CHARBUNDLE cbnd;
    INT        iX, iY;

    WinQueryWindowRect( hwnd, &rclDst);
    WinFillRect( hps, &rclDst, CLR_GREEN);

    DrawNumbers(hps);
    CalcTextOut(hps, iX = (11 * sCharWidth / 5) + 1, iY = 2 * sCharHeight,
               (PSZ)"M-", 2);
    CalcTextOut(hps, iX, iY + 2 * sCharHeight, (PSZ)"M+", 2);
    CalcTextOut(hps, iX, iY + 4 * sCharHeight, (PSZ)"MR", 2);
    CalcTextOut(hps, iX, iY + 6 * sCharHeight, (PSZ)"MC", 2);

    /* Draw the minus of the plus/minus button */
    cbnd.usBackMixMode = FM_LEAVEALONE;
    GpiSetAttrs( hps, PRIM_CHAR, CBB_BACK_MIX_MODE, 0L, &cbnd);
    iX =  (3 * 6 * sCharWidth) + (WIDTHCONST * sCharWidth / 10);
    CalcTextOut( hps, iX, iY + sCharHeight / 4, (PSZ)"_", 1);

    /* Draw the square root bitmap */
    rclDst.xLeft = 160 * sCharWidth / 5;
    rclDst.yBottom = 31 * sCharHeight / 4;
    rclDst.xRight = rclDst.xLeft + 2 * sCharWidth;
    rclDst.yTop = rclDst.yBottom + (3 * sCharHeight / 2);
    WinDrawBitmap( hps
		 , hbmSqr
		 , NULL
		 , (PPOINTL)&rclDst
		 , CLR_WHITE
		 , CLR_BLACK
		 , DBM_STRETCH );

    UpdateDisplay();
}


/******************************************************************************/
/*  initialize the bitmaps for a blank key and for the square-root sign       */
/******************************************************************************/
BOOL
PSInit()
{
    HPS 	     hps;
    FONTMETRICS      fm;
    POINTL	     ptl;
    SIZEL	     sizl;
    BITMAPINFOHEADER bmp;
    POINTL	     aptl[4];
    LONG	     alCaps[2];

    /**************************************************************************/
    /*	compute the units of horizontal and vertical distance based on font   */
    /**************************************************************************/
    hps = WinGetPS( HWND_DESKTOP);
    GpiQueryFontMetrics( hps, (LONG)sizeof(FONTMETRICS), &fm);
    sCharHeight = (SHORT)(fm.lEmHeight); /* avg height of uppercase character */
    sCharWidth	= (SHORT)(fm.lEmInc);	 /* usually 'M' increment	      */
    WinReleasePS( hps);

    /**************************************************************************/
    /*	prepare the square root bitmap					      */
    /**************************************************************************/
    hdcSqr = DevOpenDC( hab, OD_MEMORY, "*", 3L, (PDEVOPENDATA)&dop, NULL);
    if( !hdcSqr)
	return(FALSE);

    sizl.cx = sizl.cy = 0L;
    hpsSqr = GpiCreatePS( hab
			, hdcSqr
			, &sizl
			, PU_PELS | GPIT_MICRO | GPIA_ASSOC );
    hbmSqr = GpiLoadBitmap( hpsSqr, NULL, IDB_SQR, 0L, 0L);

    /**************************************************************************/
    /*	prepare the bitmap of a blank key				      */
    /**************************************************************************/
    hdcLocal = DevOpenDC( hab, OD_MEMORY, "*", 3L, (PDEVOPENDATA)&dop, NULL);
    if( !hdcLocal)
	return(FALSE);

    sizl.cx = sizl.cy = 0L;
    hpsLocal = GpiCreatePS( hab
			  , hdcLocal
			  , &sizl
			  , PU_PELS | GPIT_MICRO | GPIA_ASSOC );
    bmp.cbFix = 12;
    bmp.cx = 11 * sCharWidth / 3;
    bmp.cy = sCharHeight * 2;
    DevQueryCaps( hdcLocal, CAPS_COLOR_PLANES, 2L, alCaps);
    bmp.cPlanes = (USHORT)alCaps[0];
    bmp.cBitCount = (USHORT)alCaps[1];
    hbmLocal = GpiCreateBitmap( hpsLocal, &bmp, 0L, NULL, NULL);
    if( !hbmLocal )
	return(FALSE);
    GpiSetBitmap( hpsLocal, hbmLocal);

    aptl[0].x = aptl[0].y = 0;
    aptl[1].x = 11 * sCharWidth / 3;
    aptl[1].y = 7 * sCharHeight / 4;
    aptl[2].x = aptl[2].y = 0;
    aptl[3].x = aptl[1].x;
    aptl[3].y = aptl[1].y;
    GpiSetColor( hpsLocal, CLR_GREEN);	    /* match the background to client */
    GpiBitBlt( hpsLocal, NULL, 2L, aptl, ROP_PATCOPY, BBO_IGNORE);

    /* Draw the rounded rect */
    ptl.x = 0;
    ptl.y = 0;
    GpiSetCurrentPosition( hpsLocal, &ptl);
    ptl.x = (11 * sCharWidth / 3) - 1;
    ptl.y = (7 * sCharHeight / 4) - 1;
    GpiSetColor( hpsLocal, CLR_WHITE);	    /* white interior		      */
    GpiBox( hpsLocal
	  , DRO_FILL
	  , &ptl
	  , (LONG)sCharWidth
	  , (LONG)(sCharHeight / 2) );
    ptl.x = 0;
    ptl.y = 0;
    GpiSetCurrentPosition( hpsLocal, &ptl);
    ptl.x = (11 * sCharWidth / 3) - 1;
    ptl.y = (7 * sCharHeight / 4) - 1;
    GpiSetColor( hpsLocal, CLR_BLACK);	    /* black border		      */
    GpiBox( hpsLocal
	  , DRO_OUTLINE
	  , &ptl
	  , (LONG)sCharWidth
	  , (LONG)(sCharHeight / 2) );
    return( TRUE);
}
