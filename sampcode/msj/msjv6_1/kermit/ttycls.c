/*
 * TTYCLS module
 *
 * Written by Bill Hall
 * 3665  Benton Street, #66
 * Santa Clara, CA 95051
 *
 * This file supports a teletype style window in either Microsoft
 * windows or Presentation Manager.
 *
 * For an example of its use, see the source code for WINAUX
 * or PMAUX.
 *
 * If being used in a Windows program, be sure to define WINDOWS
 */

#ifdef WINDOWS
#define NOCOMM		/* stops a bunch of unneeded level 3 warnings */
#define NOKANJI
#define NOATOMS
#define NOMINMAX
#include <windows.h>
#else
#define INCL_PM
#include <os2.h>
extern HAB hAB;		/* these should be defined in the main program */
extern HHEAP hHeap;
extern FATTRS ttyfat;
#endif
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "ascii.h"
#include "ttycls.h"

/* local functions */
static void NEAR DoLF(PTTYWND pTTYWnd);
static void NEAR DoCR(PTTYWND pTTYWnd);
static void NEAR DoBS(PTTYWND pTTYWnd);
static void NEAR DoTab(PTTYWND pTTYWnd);

void NEAR TTYClear(PTTYWND pTTYWnd)
{
    memset(pTTYWnd->pVidBuf, NUL, pTTYWnd->MaxLines * pTTYWnd->MaxLineLength);
    DoCR(pTTYWnd);
#ifdef WINDOWS
    InvalidateRect(pTTYWnd->hWnd, (LPRECT)NULL, TRUE);
    UpdateWindow(pTTYWnd->hWnd);
#else
    WinInvalidateRect(pTTYWnd->hWnd, (PRECTL)NULL, TRUE);
    WinUpdateWindow(pTTYWnd->hWnd);    
#endif
}

/* 
 * Called at create window time.  Creates a text buffer based
 * on the passed in width, height, and the character width and height.
 * Other parameters such as wrap, character mask, etc. are also set.
 */
InitTTYWindow(pTTYWnd,left,top,width,height,charwidth,charheight,LFonCR,CRonLF,
				wrap, fontindex, mask)
PTTYWND pTTYWnd;
short left, top, width, height, charwidth, charheight;
BOOL LFonCR, CRonLF, wrap;
unsigned short fontindex;
BYTE mask; 
{
    
    int bufsize;

    pTTYWnd->Left = left;
    pTTYWnd->Top = top;
    pTTYWnd->Width = width;
    pTTYWnd->Height = height;
    pTTYWnd->CWidth = charwidth;
    pTTYWnd->CHeight = charheight;
    pTTYWnd->MaxLines = height / charheight;
    pTTYWnd->MaxCols = width / charwidth;
    pTTYWnd->MaxLineLength = pTTYWnd->MaxCols + 1;
    bufsize = pTTYWnd->MaxLines * pTTYWnd->MaxLineLength;

  /* allocate space for the display characters */
  /* if allocation successful, initialize the remaining tty data */

#ifdef WINDOWS
    if (pTTYWnd->hVidBuf = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, bufsize)) {
	pTTYWnd->pVidBuf = LocalLock(pTTYWnd->hVidBuf); /* lock the buffer */
    	pTTYWnd->Pos.y = (pTTYWnd->MaxLines - 1) * charheight - 1;
	pTTYWnd->hFont = (HFONT)fontindex;
#else
    if (pTTYWnd->pVidBuf = WinAllocMem(hHeap, bufsize)) {
	memset(pTTYWnd->pVidBuf, NUL, bufsize);
    	pTTYWnd->Pos.y = 1;
	pTTYWnd->FontIndex = fontindex;    	
#endif
    	pTTYWnd->Pos.x = 0;
	pTTYWnd->oCurrentLine = pTTYWnd->CurLineOffset = 0;
	pTTYWnd->oVidLastLine = (pTTYWnd->MaxLines-1) * pTTYWnd->MaxLineLength;
	pTTYWnd->LFonCR = LFonCR;
	pTTYWnd->CRonLF = CRonLF;
        pTTYWnd->Wrap = wrap;
	pTTYWnd->ebitmask = mask;
  	return TRUE;
    }
    return FALSE;
}

/* display the characters contained in str */
int NEAR TTYDisplay(PTTYWND pTTYWnd, short len, BYTE *str)
{

    HPS hPS;
    register BYTE *ptr;
    register short ctr;
    short toff, txpos;
    BYTE *tbuf;
    short cols = pTTYWnd->MaxCols;
    short cwidth = pTTYWnd->CWidth;
    BYTE mask = pTTYWnd->ebitmask;
    
    while (len) {
   	ptr = str;
	ctr = 0;
 	txpos = (short)pTTYWnd->Pos.x;
 	tbuf = pTTYWnd->pVidBuf + pTTYWnd->oCurrentLine;
	toff = pTTYWnd->CurLineOffset;

	while((*ptr &= mask) >= SP) {
	    if ((len) && (toff < cols)) {
		ctr += 1;
		*(tbuf + toff++) = *ptr++;
		txpos += cwidth;
		len -= 1;
	    }
	    else
		break;
	}
	if (ctr) {
#ifdef WINDOWS
	    HFONT hfont = pTTYWnd->hFont;
	    hPS = GetDC(pTTYWnd->hWnd);
	    SetBkColor(hPS, GetSysColor(COLOR_WINDOW));
	    SetTextColor(hPS, GetSysColor(COLOR_WINDOWTEXT));
	    if (hfont)
	        SelectObject(hPS, hfont);
	    TextOut(hPS, pTTYWnd->Pos.x, pTTYWnd->Pos.y, (LPSTR)str, ctr);
	    ReleaseDC(pTTYWnd->hWnd, hPS);
#else
	    USHORT findex = pTTYWnd->FontIndex;
	    hPS = WinGetPS(pTTYWnd->hWnd);
	    if (findex) {
    	        GpiCreateLogFont(hPS, (PSTR8)"TTYCLS",(LONG)findex, &ttyfat);
	        GpiSetCharSet(hPS,(LONG)findex);
	    }
	    GpiSetBackMix(hPS, BM_OVERPAINT);
	    GpiCharStringAt(hPS,(PPOINTL)&pTTYWnd->Pos,(LONG)ctr,(PCH)str);
	    GpiRestorePS(hPS, -1L);
	    WinReleasePS (hPS);
#endif
	    if (toff < cols) {
	        pTTYWnd->CurLineOffset = toff;
#ifdef WINDOWS
	        pTTYWnd->Pos.x = txpos;
#else
	        pTTYWnd->Pos.x = (LONG)txpos;
#endif
	    }
	    else
		if (pTTYWnd->Wrap) {
		    DoCR(pTTYWnd);
		    DoLF(pTTYWnd);
	        }	    
	}
	while ((*ptr &= mask) < SP) {
	    if (len) {
		switch(*ptr) {
		    case BEL:
#ifdef WINDOWS
			MessageBeep(0);
#else
			WinAlarm(HWND_DESKTOP, WA_ERROR);
#endif
			break;
		    case HT:
			DoTab(pTTYWnd);
			break;			
		    case CR:
			DoCR(pTTYWnd);
			if (pTTYWnd->LFonCR)
			    DoLF(pTTYWnd);
			break;
		    case LF:
			if (pTTYWnd->CRonLF)
			    DoCR(pTTYWnd);
			DoLF(pTTYWnd);
			break;
		    case BS:
			DoBS(pTTYWnd);
			break;
		}
		len -= 1;
		ptr++;
	    }
	    else
		break;
	}
        str = ptr;
    }
    return (len);
}

/* perform a carriage return */
static void NEAR DoCR(PTTYWND pTTYWnd)
{
    pTTYWnd->CurLineOffset = 0;
    pTTYWnd->Pos.x = 0;
}

/* perform a line feed */
static void NEAR DoLF(PTTYWND pTTYWnd)
{
    BYTE *pCurr;
    short offset, cols;
    int i;
    HWND hWnd = pTTYWnd->hWnd;
    cols = pTTYWnd->MaxCols;

    if ((pTTYWnd->oCurrentLine+=pTTYWnd->MaxLineLength) > pTTYWnd->oVidLastLine)
	pTTYWnd->oCurrentLine = 0;
    pCurr = pTTYWnd->pVidBuf + pTTYWnd->oCurrentLine;
    offset = pTTYWnd->CurLineOffset;

    for (i = 0; i < offset; i++)
	*(pCurr + i) = SP;
    for (i = offset; i < cols; i++)
	*(pCurr + i) = NUL;

#ifdef WINDOWS
    ScrollWindow(hWnd,0,-pTTYWnd->CHeight,(LPRECT)NULL,(LPRECT)NULL);
    UpdateWindow(hWnd);
#else
    WinScrollWindow(hWnd,0,pTTYWnd->CHeight,
				NULL,NULL,NULL,NULL,SW_INVALIDATERGN);
    WinUpdateWindow(hWnd);    
#endif
}

/* perform a backspace */
static void NEAR DoBS(PTTYWND pTTYWnd)
{
    if (pTTYWnd->CurLineOffset > 0) {
#ifdef WINDOWS
	pTTYWnd->Pos.x -= pTTYWnd->CWidth;
#else
	pTTYWnd->Pos.x -= (LONG)pTTYWnd->CWidth;
#endif
	pTTYWnd->CurLineOffset -= 1;
    }
}

/* Horizontal tab */
static void NEAR DoTab(pTTYWnd)
PTTYWND pTTYWnd;
{

    short curoffset, xpos, ypos, cols, cwidth;
    BYTE *pCurr;
#ifdef WINDOWS
    RECT myrect;
#else
    RECTL myrect;
#endif
    cols = pTTYWnd->MaxCols;
    curoffset = pTTYWnd->CurLineOffset;
    cwidth = pTTYWnd->CWidth;

    if (curoffset < (cols - 1)) {
        xpos = (short)pTTYWnd->Pos.x;		/* initialize variables */
        ypos = (short)pTTYWnd->Pos.y;
        pCurr = pTTYWnd->pVidBuf + pTTYWnd->oCurrentLine;
        do {
	    if (*(pCurr + curoffset) == NUL)  /* if null, replace with space */
	        *(pCurr + curoffset) = SP;
	    curoffset += 1;		/* update offsets */
	    xpos += cwidth;
        } while ((curoffset % 8 != 0) && (curoffset < (cols - 1)));

    /* now invalidate the part of the screen affected */
#ifdef WINDOWS
	SetRect((LPRECT)&myrect, xpos, ypos, pTTYWnd->Width, pTTYWnd->Height);
	InvalidateRect(pTTYWnd->hWnd, (LPRECT)&myrect, TRUE);
#else
	WinSetRect(hAB,&myrect,xpos,ypos,pTTYWnd->Width,ypos+pTTYWnd->CHeight);
	WinInvalidateRect(pTTYWnd->hWnd, &myrect, TRUE);
#endif
    /* finally, reset the tty window variables */
        pTTYWnd->Pos.x = xpos;
        pTTYWnd->CurLineOffset = curoffset;
    }
}

/* repaint the window */
void NEAR TTYWndPaint(PTTYWND pTTYWnd, HPS hPS, short top, short bottom)
{

#ifdef WINDOWS
    POINT pt;
    short wtop;
    HFONT hfont = pTTYWnd->hFont;
#else
    USHORT findex = pTTYWnd->FontIndex;
    POINTL pt;
#endif
    BYTE *lineptr;
    BYTE *pBuf, *pEnd;
    short lines, length;
    short cheight;
    int i;

    pt.x = 0;
    pBuf = pTTYWnd->pVidBuf;
    pEnd = pBuf + pTTYWnd->oVidLastLine;

    lineptr = pBuf + pTTYWnd->oCurrentLine;
    pt.y = pTTYWnd->Pos.y;

    length = pTTYWnd->MaxLineLength;
    cheight = pTTYWnd->CHeight;
#ifdef WINDOWS
    wtop = pTTYWnd->Height - top;
    lines = wtop / cheight;
    if (wtop % cheight)
	lines += 1;
    lines = min(pTTYWnd->MaxLines, lines);
    SetBkColor(hPS, GetSysColor(COLOR_WINDOW));
    SetTextColor(hPS, GetSysColor(COLOR_WINDOWTEXT));
    if (hfont)
        SelectObject(hPS, hfont);
#else
    lines = top / cheight;
    if (top % cheight)
	lines += 1;
    lines = min(pTTYWnd->MaxLines, lines);
    GpiErase (hPS);
    GpiSetBackMix(hPS, BM_OVERPAINT);
    if (findex) {
        GpiCreateLogFont(hPS, (PSTR8)"TTYCLS",(LONG)findex, &ttyfat);
        GpiSetCharSet(hPS,(LONG)findex);
    }
#endif

    for (i = 0; i < lines; i++) {
#ifdef WINDOWS
        TextOut(hPS, pt.x, pt.y, (LPSTR)lineptr, strlen(lineptr));
	pt.y -= cheight;
#else
        GpiCharStringAt(hPS,&pt,(LONG)strlen(lineptr),(PCH)lineptr);
	pt.y += (LONG)cheight;
#endif
        if ((lineptr -= length) < pBuf)
	    lineptr = pEnd;
    }
}
