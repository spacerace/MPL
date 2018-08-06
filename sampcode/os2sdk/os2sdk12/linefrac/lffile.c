/************************************************************************
*
*   lffile.c -- File handling subroutines for LineFrac.
*
*   Created by Microsoft Corporation, 1989
*
************************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#define INCL_BITMAPFILEFORMAT
#include <os2.h>

#include <stdio.h>

#undef  RC_INVOKED
#include "opendlg.h"

#define INCL_GLOBALS
#define INCL_THREADS
#include "linefrac.h"

#define INCL_LFFILE
#define INCL_LFPS
#define INCL_LFUTIL
#include "lffuncs.h"




/*
 * this is the bitmap resource file format structure
 */
typedef struct {
    USHORT    wType;
    ULONG     dwSize;
    int       xHotspot;
    int       yHotspot;
    ULONG     dwBitsOffset;
    USHORT    bmWidth;       /* from here this is a BitmapInfo table */
    USHORT    bmHeight;
    USHORT    bmPlanes;
    USHORT    bmBitcount;
} RCBITMAP;
typedef RCBITMAP FAR *PRCBITMAP;




/************************************************************************
*
*   Global Variables
*
************************************************************************/

extern GLOBALDATA global;




/************************************************************************
*
*   LfReadFile
*
*   Calls the OpenDlg function to ask the user what file name to
*   read from.
*
************************************************************************/

VOID
LfReadFile(hwnd, pthr)
HWND hwnd;
PTHR pthr;
{
    HFILE hFile;
    DLF dlf;

    dlf.rgbAction	= DLG_OPENDLG;
    dlf.rgbFlags	= ATTRDIRLIST;
    dlf.phFile		= &hFile;
    dlf.pszExt		= (PSZ)"\\*.bmp";
    dlf.pszAppName	= "LineFrac";
    dlf.pszTitle	= "Load Bitmap";
    dlf.pszInstructions = NULL;
    dlf.szFileName[0]	= '\0';
    dlf.szOpenFile[0]	= '\0';

    switch (DlgFile(hwnd,&dlf))
    {
    case TDF_ERRMEM:
    case TDF_INVALID:
	MyMessageBox(hwnd, "Error reading file.");
	break;

    case TDF_NOOPEN:
	break;

    default:
	if (!LfReadBMP(pthr, &dlf))
	    MyMessageBox(hwnd, "Error reading file.\nIs AutoResize enabled?");
    }
}




/************************************************************************
*
*   LfReadBMP
*
*   Read a bitmap in from a BMP format file.  Prepare the DC for the
*   given thread to accept it.	The user can have the DC resized to
*   fit exactly the bitmap, or fit the bits in as best as we can.
*   If we're not resizing and the bitmap is larger than the thread's
*   DC, then load the bits flush with the lower left.
*
*   Both old-style (PRCBITMAP) and new-style (PBITMAPFILEHEADER)
*   bitmaps can be read.
*
*   Free up memory and close the file before leaving.  The file
*   will have been opened by the time this function is called,
*   and the file handle will be in the *pdlf structure.
*
************************************************************************/

BOOL
LfReadBMP(pthr, pdlf)
PTHR pthr;
PDLF pdlf;		/* File information filled by DlgFile. */
{
    HFILE hfile;
    ULONG cScans;
    ULONG ulSize;	 /* Number of bytes occupied by bitmap bits.	     */
    USHORT cSegs;	 /* Number of 64K segments in ulSize.		     */
    USHORT cbExtra;	 /* Bytes in last segment of ulSize.		     */
    SEL sel;		 /* Base selector to file data. 		     */
    USHORT hugeshift;	 /* Segment index shift value.			     */
    USHORT cbRead1;	 /* Number of bytes to read first call to DosRead    */
    USHORT cbRead2;	 /* Number of bytes to read second call to DosRead   */
    USHORT cbRead;	 /* Number of bytes read by DosRead.		     */
    BOOL fRet = FALSE;	 /* Function return code.			     */
    USHORT i;		 /* Generic loop index. 			     */
    FILESTATUS fsts;
    PBITMAPFILEHEADER pbfh;
    PRCBITMAP  rb;
    PBYTE pImage;


    /*******************************************************************
    * Find out how big the file is so we can read the whole thing in.
    *******************************************************************/

    hfile = *(pdlf->phFile);
    if( DosQFileInfo( hfile, 1, &fsts, sizeof(FILESTATUS)) != 0)
	goto lfread_error_close_file;

    ulSize  = fsts.cbFile;
    cSegs   = (USHORT)(ulSize/0x10000L);
    cbExtra = (USHORT)(ulSize%0x10000L);
    if (DosAllocHuge(cSegs, cbExtra, (PSEL)&sel, 0, 0))
	goto lfread_error_close_file;
    if (DosGetHugeShift(&hugeshift))
	goto lfread_error_free_bits;

    pImage = (PBYTE)MAKEP(sel, 0);
    rb	   = (PRCBITMAP)pImage;
    pbfh   = (PBITMAPFILEHEADER)pImage;


    /*******************************************************************
    * Read the bits in from the file. The DosRead function allows a
    * maximum of 64K-1 bytes read at a time.  We get around this
    * by reading two 32K chunks for each 64K segment, and reading the
    * last segment in one piece.
    *******************************************************************/

    for (i = 0; i <= cSegs; ++i)
    {
	if (i < cSegs)
	{
	    /* This segment is 64K bytes long, so split it up. */
	    cbRead1 = 0x8000;
	    cbRead2 = 0x8000;
	}
	else
	{
	    /* This segment is less than 64K bytes long, so read it all. */
	    cbRead1 = cbExtra;
	    cbRead2 = 0;
	}

	/* There's a possibility that cbExtra will be 0, so check
	 * to avoid an unnecessary system call.
	 */
	if (cbRead1 > 0)
	{
	    if (DosRead( hfile
		       , (PVOID)MAKEP(sel+(i<<hugeshift), 0)
		       , cbRead1
		       , &cbRead))
		goto lfread_error_free_bits;
	    if (cbRead1 != cbRead)
		goto lfread_error_free_bits;
	}

	/* This will always be skipped on the last partial segment. */
	if (cbRead2 > 0)
	{
	    if (DosRead( hfile
		       , (PVOID)MAKEP(sel+(i<<hugeshift), cbRead1)
		       , cbRead2
		       , &cbRead))
		goto lfread_error_free_bits;
	    if (cbRead2 != cbRead)
		goto lfread_error_free_bits;
	}
    }


    /*******************************************************************
    * At this point we have the bitmap completely in memory.  Now we
    * look at how the user wants them set into the thread's PS.  If
    * the thread has fAutoResizePS set, then make the PS fit the size
    * of the bitmap (the easy case).  If the flag is not set, then
    * figure out how to place it.
    *******************************************************************/

    if (pthr->fAutoSizePS)
    {
	if (pbfh->bmp.cbFix != sizeof(BITMAPINFOHEADER))
	{
	    global.bm.cx	= rb->bmWidth;
	    global.bm.cy	= rb->bmHeight;
	    global.bm.cPlanes	= rb->bmPlanes;
	    global.bm.cBitCount = rb->bmBitcount;
	}
	else
	{
	    global.bm.cx	= pbfh->bmp.cx;
	    global.bm.cy	= pbfh->bmp.cy;
	    global.bm.cPlanes	= pbfh->bmp.cPlanes;
	    global.bm.cBitCount = pbfh->bmp.cBitCount;
	}

	LfResizePS(pthr);
    }
    else
	goto lfread_error_free_bits;


    /*******************************************************************
    * Tell GPI to put the bits into the thread's PS. The function returns
    * the number of scan lines of the bitmap that were copied.	We want
    * all of them at once.
    *******************************************************************/

    if (pbfh->bmp.cbFix != sizeof(BITMAPINFOHEADER))
    {
        pImage += rb->dwBitsOffset;
        rb->dwBitsOffset = sizeof(BITMAPINFOHEADER);
	cScans = GpiSetBitmapBits( pthr->hps
				 , 0L
				 , (LONG)rb->bmHeight
				 , pImage
				 , (PBITMAPINFO)&(rb->dwBitsOffset));
	if (cScans != (ULONG)rb->bmHeight)  /* compare with original number of scans */
	    goto lfread_error_free_bits;
    }
    else
    {
	cScans = GpiSetBitmapBits( pthr->hps
				 , 0L
				 , (LONG)pbfh->bmp.cy
				 , pImage + pbfh->offBits
				 , (PBITMAPINFO)&(pbfh->bmp));
	if (cScans != (ULONG)pbfh->bmp.cy)  /* compare with original number of scans */
	    goto lfread_error_free_bits;
    }
    fRet = TRUE;     /* We made it!  The bits are in the thread's PS. */


    /*******************************************************************
    * Close the file, free the buffer space and leave.	This is a
    * common exit point from the function.  Since the same cleanup
    * operations need to be performed for such a large number of
    * possible error conditions, this is a concise way to do the right
    * thing.
    *******************************************************************/

lfread_error_free_bits:
    DosFreeSeg(sel);
lfread_error_close_file:
    DosClose(hfile);
    return fRet;
}




/************************************************************************
*
*   LfWriteFile
*
*   Calls the OpenDlg function to ask the user what file name to
*   save under.
*
************************************************************************/

VOID
LfWriteFile(hwnd, pthr)
HWND hwnd;
PTHR pthr;
{
    HFILE hFile;
    DLF dlf;
    BITMAPINFOHEADER bmih;


    SetupDLF( &dlf
	    , DLG_SAVEDLG
	    , &hFile
	    , (PSZ)"\\*.BMP"
	    , (PSZ)"LineFrac"
	    , (PSZ)"Save Bitmap"
	    , NULL );
    dlf.szFileName[0] = '\0';
    dlf.szOpenFile[0] = '\0';

    switch (DlgFile(hwnd,&dlf))
    {
    case TDF_ERRMEM:
    case TDF_INVALID:
	MyMessageBox(hwnd, "Error opening file.");
	break;

    case TDF_NOSAVE:
	break;

    default:
	bmih.cbFix     = sizeof(BITMAPINFOHEADER);
	bmih.cx        = (USHORT) pthr->rcl.xRight;
	bmih.cy        = (USHORT) pthr->rcl.yTop;
	bmih.cPlanes   = pthr->cPlanes;
	bmih.cBitCount = pthr->cBitCount;

	if (!LfWriteBMP(pthr->hps, &bmih, &dlf))
	    MyMessageBox(hwnd, "Error writing file.");
    }
}




/************************************************************************
*
*   LfWriteBMP
*
*   Write the bitmap out to a BMP format file.	Write the file
*   header first, then the bitmap bits.  Space for the header
*   and the bits is allocated.	Huge bitmaps are supported.
*   Free up memory and close the file before leaving.  The file
*   will have been opened by the time this function is called,
*   and the file handle will be in the *pdlf structure.
*
************************************************************************/

BOOL
LfWriteBMP(hPS, pbmih, pdlf)
HPS hPS;		/* hPS from which to get bitmap bits.	  */
PBITMAPINFOHEADER pbmih;/* Bitmap information.			  */
PDLF pdlf;		/* File information filled by DlgFile. */
{
    HFILE hfile;
    ULONG cScans;
    ULONG ulSize;	 /* Number of bytes occupied by bitmap bits.	     */
    USHORT cSegs;	 /* Number of 64K segments in ulSize.		     */
    USHORT cbExtra;	 /* Bytes in last segment of ulSize.		     */
    SEL selBits;	 /* Base selector to bitmap bits.		     */
    USHORT hugeshift;	 /* Segment index shift value.			     */
    USHORT cbBMHdr;	 /* Size of bitmap header.			     */
    PBITMAPFILEHEADER pbfh; /* Pointer to private copy of bitmap info data.	*/
    USHORT cbWrite1;	 /* Number of bytes to write first call to DosWrite  */
    USHORT cbWrite2;	 /* Number of bytes to write second call to DosWrite */
    USHORT cbWritten;	 /* Number of bytes written by DosWrite.	     */
    BOOL fRet = FALSE;	 /* Function return code.			     */
    USHORT i;		 /* Generic loop index. 			     */
    struct
    {
	LONG cPlanes;
	LONG cBitCount;
    } bmFmt;


    hfile = *(pdlf->phFile);

    /*******************************************************************
    * If the bitmap was created with either 0 planes or 0 bits per
    * pixel, then query the format to write with.  By asking for just
    * one format (two LONGs, or one instance of structure of bmFmt),
    * we'll get the device's favored format.
    *******************************************************************/

    if ((pbmih->cPlanes == 0) || (pbmih->cBitCount))
    {
	if (!GpiQueryDeviceBitmapFormats(hPS, 2L, (PLONG)&bmFmt))
	    goto lfwrite_error_close_file;
    }
    else
    {
	bmFmt.cPlanes	= pbmih->cPlanes;
	bmFmt.cBitCount = pbmih->cBitCount;
    }


    /*******************************************************************
    * Determine size of bitmap header.	The header consists of a
    * a fixed-size part and a variable-length color table.  The
    * latter has  2^cBitCount  entries, each of which is sizeof(RGB)
    * bytes long.  The exception is when cBitCount is 24, in which
    * case the color table is omitted because the pixels are direct
    * rgb values.
    *******************************************************************/

    i = (int) bmFmt.cBitCount;
    if (i == 24)
	cbBMHdr = 0;
    else
	for (cbBMHdr = sizeof(RGB); i > 0; --i)
	    cbBMHdr *= 2;
    cbBMHdr += sizeof(BITMAPFILEHEADER);


    /*******************************************************************
    * Copy structure from input to work buffer.  The call to
    * GpiQueryBitmapBits will have write-access to this, so we won't
    * let it have the user's data.
    *******************************************************************/

    pbfh = 0;
    if (DosAllocSeg(cbBMHdr, ((PUSHORT)&pbfh)+1, 0))
	goto lfwrite_error_close_file;
    pbfh->bmp = *pbmih;
    if ((pbmih->cPlanes == 0) || (pbmih->cBitCount))
    {
	pbfh->bmp.cPlanes   = (USHORT) bmFmt.cPlanes;
	pbfh->bmp.cBitCount = (USHORT) bmFmt.cBitCount;
    }


    /*******************************************************************
    * Allocate space for the bitmap bits -- all of them at once.
    * The extra ULONG casts are there to force all the arithmetic
    * to be done in 32 bits.
    *******************************************************************/

    ulSize = (
	       (
		 (
		   (ULONG)pbfh->bmp.cBitCount
		   * (ULONG)pbfh->bmp.cx
		   + 31L
		 ) / 32L
	       ) * (ULONG)pbfh->bmp.cPlanes * 4L
	     ) * (ULONG)pbfh->bmp.cy;

    cSegs   = (USHORT)(ulSize/0x10000L);
    cbExtra = (USHORT)(ulSize%0x10000L);
    if (DosAllocHuge(cSegs, cbExtra, (PSEL)&selBits, 0, 0))
	goto lfwrite_error_free_header;
    if (DosGetHugeShift(&hugeshift))
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Tell GPI to give us the bits. The function returns the number
    * of scan lines of the bitmap that were copied.  We want all of
    * them at once.
    *******************************************************************/

    cScans = GpiQueryBitmapBits( hPS
			       , 0L
			       , (ULONG)pbfh->bmp.cy
			       , (PBYTE)MAKEP(selBits, 0)
			       , (PBITMAPINFO)&pbfh->bmp);
    if (cScans != pbfh->bmp.cy)  /* compare with original number of scans */
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Fill in the extra header fields and write the header out to
    * the file.
    *******************************************************************/

    pbfh->usType    = 0x4D42;		  /* 'MB' */
    pbfh->cbSize    = ulSize + cbBMHdr;
    pbfh->xHotspot  = pbfh->bmp.cx / 2;    /* why bother ? */
    pbfh->yHotspot  = pbfh->bmp.cy / 2;
    pbfh->offBits   = cbBMHdr;

    if (DosWrite( hfile
		, (PVOID)pbfh
		, cbBMHdr
		, &cbWritten))
	goto lfwrite_error_free_bits;
    if (cbWritten != cbBMHdr)
	goto lfwrite_error_free_bits;


    /*******************************************************************
    * Write the bits out to the file. The DosWrite function allows a
    * maximum of 64K-1 bytes written at a time.  We get around this
    * by writing two 32K chunks for each 64K segment, and writing the
    * last segment in one piece.
    *******************************************************************/

    for (i = 0; i <= cSegs; ++i)
    {
	if (i < cSegs)
	{
	    /* This segment is 64K bytes long, so split it up. */
	    cbWrite1 = 0x8000;
	    cbWrite2 = 0x8000;
	}
	else
	{
	    /* This segment is less than 64K bytes long, so write it all. */
	    cbWrite1 = cbExtra;
	    cbWrite2 = 0;
	}

	/* There's a possibility that cbExtra will be 0, so check
	 * to avoid an unnecessary system call.
	 */
	if (cbWrite1 > 0)
	{
	    if (DosWrite( hfile
			, (PVOID)MAKEP(selBits+(i<<hugeshift), 0)
			, cbWrite1
			, &cbWritten))
		goto lfwrite_error_free_bits;
	    if (cbWrite1 != cbWritten)
		goto lfwrite_error_free_bits;
	}

	/* This will always be skipped on the last partial segment. */
	if (cbWrite2 > 0)
	{
	    if (DosWrite( hfile
			, (PVOID)MAKEP(selBits+(i<<hugeshift), cbWrite1)
			, cbWrite2
			, &cbWritten))
		goto lfwrite_error_free_bits;
	    if (cbWrite2 != cbWritten)
		goto lfwrite_error_free_bits;
	}
    }

    fRet = TRUE;     /* We made it!  The bits are on the disk. */


    /*******************************************************************
    * Close the file, free the buffer space and leave.	This is a
    * common exit point from the function.  Since the same cleanup
    * operations need to be performed for such a large number of
    * possible error conditions, this is concise way to do the right
    * thing.
    *******************************************************************/

lfwrite_error_free_bits:
    DosFreeSeg(selBits);
lfwrite_error_free_header:
    DosFreeSeg(*((PUSHORT)&pbfh+1));
lfwrite_error_close_file:
    DosClose(hfile);
    return fRet;
}
