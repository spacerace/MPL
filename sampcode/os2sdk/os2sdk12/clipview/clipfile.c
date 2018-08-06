/*
 * CLIPFILE.C -- File handling for ClipView
 * Created by Microsoft Corporation, 1989
 *
 * This file contains one routine:  SaveClipboard(), which uses
 * the OPENDLG library to put up a File...Save... dialog box.
 *
 * After getting a file name, it tries to save the current rendered format.
 */
#define INCL_BITMAPFILEFORMAT
#define	INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_GPIBITMAPS
#define	INCL_GPIMETAFILES
#define	INCL_WINCLIPBOARD
#define	INCL_WINERRORS
#include <os2.h>
#include <opendlg.h>
#include <string.h>
#include "clipview.h"
/*
 * Globals
 */
extern HAB	vhab;			/* Anchor block		*/
extern HWND	vhwndClient;		/* Main client area	*/
/*
    Macros
*/
#define CHK(f) fSuccess = fSuccess && (f)
#define LOADSTRING(id, sz) WinLoadString(vhab, (HMODULE) NULL, id, MAXLEN, sz)
/*
    Private function prototypes
*/
BOOL SaveText(HFILE hf, PSZ pszText);

BOOL SaveClipboard(HWND hwnd, USHORT usFormat) {
/*
    Save the clipboard contents in several formats.
    The "Save BITMAP" code is similar to that in the LINEFRAC sample.
*/
    BOOL		fSuccess = TRUE;     /* Did we succeed in saving? */
    ULONG		hItem;		     /* Handle from QueryClipbrdData */
    /*
	Variables needed for File...Save... dialog.
    */
    DLF 		dlf;		     /* Dialog file */
    HFILE		hf;		     /* Handle to output file */
    UCHAR		szExt[8];	     /* Default extension */
    UCHAR		szInst[MAXLEN];      /* Instructions */
    UCHAR		szMessage[MAXLEN];   /* Various messages */
    UCHAR		szTitle[MAXTITLELEN];/* Application title */
    /*
	Variables needed for saving Metafiles
    */
    HMF 		hmfCopy;	     /* Clipboard metafile copy */
    /*
	Variables needed for saving BITMAPs
    */
    BITMAPINFOHEADER	bmp;		/* Header to be queried */
    HDC 		hdcMemory;	/* Memory DC for the BITMAP */
    HPS 		hpsMemory;	/* ...and it's associated PS */
    PBITMAPFILEHEADER	pbfh;		/* bmp + color table */
    POINTL		ptlOrigin;	/* Bitmap origin */
    SEL 		selBuffer;	/* Selector to actual BITMAP */
    SEL 		selHeader;	/* Selector for the BMP header */
    SIZEL		sizl;		/* Used in PS creation */
    ULONG		cbBuffer;	/* No. of bytes in buffer */
    USHORT		cbExtra;	/* No. of bytes in "final" segment */
    USHORT		cbHeader;	/* No. of bytes in header */
    USHORT		cbWrite1;	/* No. of bytes to be written... */
    USHORT		cbWrite2;	/* ...in the two-part sel writes */
    USHORT		cbWritten;	/* No. of bytes actually written */
    USHORT		cSegs;		/* No. of segments to write */
    USHORT		i;		/* Which segment is being written? */
    USHORT		usHugeShift;
    /*
	Open the clipboard
    */
    if (!WinOpenClipbrd(vhab))
	return FALSE;
    /*
	Get the clipboard data
    */
    if (hItem = WinQueryClipbrdData(vhab, usFormat)) {
	/*
	    Put up the Save... file dialog with the appropriate extensions
	*/
	switch (usFormat) {
	    case CF_TEXT:
	    case CF_DSPTEXT:	 strcpy(szExt, "\\*.TXT");  break;

	    case CF_BITMAP:
	    case CF_DSPBITMAP:	 strcpy(szExt, "\\*.BMP");  break;

	    case CF_METAFILE:
	    case CF_DSPMETAFILE: strcpy(szExt, "\\*.MET");  break;

	    default:		 strcpy(szExt, "\\*.*");    break;
	}
	/*
	    Put the string "Saving Format:  <format>" in the Save dialog box
	*/
	GetFormatName(usFormat, szMessage);
	LOADSTRING(IDS_SAVETITLE, szTitle);
	strcat(szTitle, szMessage);

	LOADSTRING(IDS_APPNAME, szMessage);
	LOADSTRING(IDS_INST, szInst);

	SetupDLF(&dlf, DLG_SAVEDLG, &hf,
		 (PSZ) szExt, (PSZ) szMessage, (PSZ) szTitle, (PSZ) szInst);

	dlf.szFileName[0] = dlf.szOpenFile[0] = '\0';
	/*
	    Put up a Save file dialog, and respond appropriately to
	    the return status.
	*/
	switch (DlgFile(hwnd, &dlf)) {
	    case TDF_ERRMEM:
	    case TDF_INVALID:
	    case TDF_NOSAVE:
		fSuccess = FALSE;

		/* fall through... */
	    default:
		break;
	}

	if (fSuccess) {
	  switch (usFormat) {

	    case CF_TEXT:
	    case CF_DSPTEXT:
		CHK(SaveText(hf, MAKEP((SEL) hItem, 0)));
		DosClose(hf);
		break;

	    case CF_BITMAP:
	    case CF_DSPBITMAP:
		/*
		    Initialize the Memory DC and its PS
		*/
		sizl.cx = sizl.cy = 0L;
		hdcMemory = DevOpenDC(vhab, OD_MEMORY, "*", 0L, NULL, NULL);
		hpsMemory = GpiCreatePS(vhab, hdcMemory, &sizl,
				  GPIA_ASSOC | GPIT_MICRO | PU_PELS);
		/*
		    Draw the BITMAP into the Memory DC
		*/
		CHK(GpiSetBitmap(hpsMemory, (HBITMAP) hItem) != HBM_ERROR);
		ptlOrigin.x = ptlOrigin.y = 0L;
		CHK(WinDrawBitmap(hpsMemory, (HBITMAP) hItem, NULL,
			   &ptlOrigin, CLR_BLACK, CLR_BACKGROUND, DBM_NORMAL));
		/*
		    Get information about the BITMAP
		*/
		CHK(GpiQueryBitmapParameters((HBITMAP) hItem, &bmp) == GPI_OK);
		/*
		    Compute the size of the buffer, and allocate
		    Make sure that > 64K BITMAPs are handled
		    (this code is from LFFILE.C)
		*/
		cbBuffer = ( ((((ULONG)bmp.cBitCount*(ULONG) bmp.cx)+31L)/32L)
				* 4L * (ULONG) bmp.cy * (ULONG) bmp.cPlanes );
		cSegs   = (USHORT) (cbBuffer >> 16);
		cbExtra = (USHORT) (cbBuffer & 0xFFFFL);
		CHK(!DosAllocHuge(cSegs, cbExtra, &selBuffer, 0, 0));
		CHK(!DosGetHugeShift(&usHugeShift));
		/*
		    Compute the size of the BITMAPFILEHEADER + color table...
		    ...then allocate it.
		*/
		cbHeader = (USHORT) (sizeof(BITMAPFILEHEADER)
				+ (sizeof(RGB) << bmp.cBitCount));
		CHK(!DosAllocSeg(cbHeader, &selHeader, SEG_NONSHARED));
		pbfh = MAKEP(selHeader, 0);
		/*
		    Copy the BITMAP information from the BITMAPINFOHEADER
		*/
		pbfh->bmp.cbFix     = 12;
		pbfh->bmp.cx	    = bmp.cx;
		pbfh->bmp.cy	    = bmp.cy;
		pbfh->bmp.cPlanes   = bmp.cPlanes;
		pbfh->bmp.cBitCount = bmp.cBitCount;
		/*
		    Get the actual BITMAP bits
		*/
		CHK(GpiQueryBitmapBits(hpsMemory, 0L, (LONG) bmp.cy,
		       MAKEP(selBuffer, 0), (PBITMAPINFO) &(pbfh->bmp))
		    != GPI_ALTERROR);
		/*
		    Set up the file header
		*/
		pbfh->usType	    = BFT_BMAP;
		pbfh->cbSize	    = cbHeader + cbBuffer;
		pbfh->xHotspot	    = bmp.cx / 2;	/* Anywhere will do */
		pbfh->yHotspot	    = bmp.cy / 2;
		pbfh->offBits	    = cbHeader;
	    /*
		Blast the BITMAP to a file...
	    */
		/*
		    ...first, the header...
		*/
		CHK(!DosWrite(hf, pbfh, cbHeader, &cbWritten));
		/*
		    ...then, the possibly large BITMAP itself
		*/
		for (i = 0; i <= cSegs; ++i) {
		    if (i < cSegs) {
		    /*
			If we a 64K segment, write it in two
			parts.	 This must be done because
			DosWrite() can only write 64K - 1
			characters at once.
		    */
			cbWrite1 = cbWrite2 = 0x8000;
		    } else {
		    /*
			The last segment is always small enough
			to write entirely.
		    */
			cbWrite1 = cbExtra; cbWrite2 = 0;
		    }

		    if (cbWrite1) {
			CHK(!DosWrite(hf,
				MAKEP((selBuffer + (i << usHugeShift)), 0),
				cbWrite1, &cbWritten));
			if (cbWrite2) {
			    CHK(!DosWrite(hf,
				MAKEP((selBuffer + (i<<usHugeShift)),cbWrite1),
				cbWrite2, &cbWritten));
			}
		    }
		}
		/*
		    Clean up

		    Error codes are not checked here because the file has
		    already been saved.
		*/
		DosClose(hf);
		GpiSetBitmap(hpsMemory, NULL);
		GpiDestroyPS(hpsMemory);
		DevCloseDC(hdcMemory);
		break;

	    case CF_METAFILE:
	    case CF_DSPMETAFILE:
		/*
		    Save metafile

		    We close and delete the file, because GpiSaveMetaFile()
		    only allows the user to create a new file.

		    We copy the metafile because GpiSaveMetafile()
		    removes the data from the application's memory.
		*/
		DosClose(hf);
		CHK(!DosDelete(dlf.szFileName, 0L));
		CHK((hmfCopy = GpiCopyMetaFile((HMF) hItem)) != GPI_ERROR);
		CHK(GpiSaveMetaFile(hmfCopy, dlf.szFileName) != GPI_ERROR);
		break;

	    default:
		/*
		    It may be reasonable to add support for other formats
		    here, by saving a bitmap of the current window contents.

		    But for now, close the file and return an error message.
		*/
		DosClose(hf);
		fSuccess = FALSE;
		break;
	  }
	}
    } else
	fSuccess = FALSE;	/* Couldn't query the clipboard format! */
    /*
	Clean up
    */
    WinCloseClipbrd(vhab);
    return fSuccess;
}

BOOL SaveText(HFILE hf, PSZ pszText) {
/*
    Save text format

    Count the number of characters, then write them.
*/
    PSZ     pszCounter;     /* Temporary to count chars in sel */
    ULONG   ulcch = 0;	    /* The number of characters */
    USHORT  cbWritten;	    /* No. of bytes actually written */

    pszCounter = pszText;
    while (*pszCounter++) ulcch++;

    return(!DosWrite(hf, pszText, (USHORT) ulcch, &cbWritten));
}
