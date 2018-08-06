/*
 * WNKINT.C
 *
 * Kermit initialization module
 *
 * Copyright (c) by
 * William S. Hall
 * 3665 Benton Street #66
 * Santa Clara, CA 95051
 *
 */

#define NOKANJI
#define NOMINMAX
#define NOATOM
#define NOSOUND
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "ascii.h"
#include "wnkerm.h"

/*
 * krmInit
 *
 * This function must be called during terminal initialization,
 * preferably in response to WM_CREATE.
 * At this time, Kermit receives the main Window handle,
 * a pointer to a linear buffer which will be filled during file transfer
 * by reading the communications port, a pointer to the buffer's
 * current length, and a pointer to the comm port id number.
 * After these values have been recorded, then WIN.INI is
 * read for the various protocol and packet parameters.
 */
BOOL FAR krmInit(HWND hWnd, BYTE *bufptr, int *buflen, int *cid)
{

    char szAppStr[80];
    char szKeyStr[80];

    Kermit.hWnd = hWnd;
    Kermit.hInst = GetWindowWord(hWnd, GWW_HINSTANCE);
    krmBufptr = bufptr;
    krmBuflen = buflen;
    krmcid = cid;

    KermParams.ebquote = KRM_DEFEBQUOTE;
    KermParams.rpquote = KRM_DEFRPTQUOTE;

    LoadString(Kermit.hInst,IDS_KRM_KERMITMENU,(LPSTR)szAppStr,sizeof(szAppStr));
    ChangeMenu(GetMenu(hWnd), 0, 
	       szAppStr, LoadMenu(Kermit.hInst, "KRMMENU"), 
	       MF_APPEND | MF_BYPOSITION | MF_POPUP);

    LoadString(Kermit.hInst,IDS_KRM_KERMIT,(LPSTR)szAppStr,sizeof(szAppStr));
    krmIcon = LoadIcon(Kermit.hInst, szAppStr);
    
    LoadString(Kermit.hInst, IDS_KRM_TIMER, szKeyStr, sizeof(szKeyStr));
    KermParams.Timer = GetProfileInt(szAppStr, szKeyStr, KRM_DEFTIMER);

    LoadString(Kermit.hInst, IDS_KRM_FILEWARN, szKeyStr, sizeof(szKeyStr));
    KermParams.FileWarning = GetProfileInt(szAppStr, szKeyStr, KRM_DEFFILEWARN);

    LoadString(Kermit.hInst, IDS_KRM_BELL, szKeyStr, sizeof(szKeyStr));
    KermParams.Bell = GetProfileInt(szAppStr, szKeyStr, KRM_DEFBELL);

    LoadString(Kermit.hInst, IDS_KRM_VERBOSE, szKeyStr, sizeof(szKeyStr));
    KermParams.verbose = GetProfileInt(szAppStr,szKeyStr,KRM_DEFVERBOSE);

    LoadString(Kermit.hInst, IDS_KRM_DISCARD, szKeyStr, sizeof(szKeyStr));
    KermParams.DiscardPartialFile = 
				GetProfileInt(szAppStr,szKeyStr,KRM_DEFDISCARD);

    LoadString(Kermit.hInst, IDS_KRM_BLOCKCHECKTYPE, szKeyStr, sizeof(szKeyStr));
    KermParams.BlockCheckType = GetProfileInt(szAppStr,szKeyStr,KRM_DEFBLOCKCHECK);
    if ((KermParams.BlockCheckType < 1) || (KermParams.BlockCheckType > 3))
	    KermParams.BlockCheckType = 1;

    LoadString(Kermit.hInst, IDS_KRM_RETRYLIMIT, szKeyStr, sizeof(szKeyStr));
    KermParams.RetryLimit = GetProfileInt(szAppStr,szKeyStr,KRM_DEFRETRYLIMIT);
    if ((KermParams.RetryLimit < KRM_MINRETRYLIMIT) 
			|| (KermParams.RetryLimit > KRM_MAXRETRYLIMIT))
        KermParams.RetryLimit = KRM_DEFRETRYLIMIT;

    LoadString(Kermit.hInst, IDS_KRM_SENDDELAY, szKeyStr, sizeof(szKeyStr));
    KermParams.SendDelay = GetProfileInt(szAppStr,szKeyStr,KRM_DEFSENDDELAY);
    if (KermParams.SendDelay > KRM_MAXSENDDELAY) 
        KermParams.SendDelay = KRM_DEFSENDDELAY;

    LoadString(Kermit.hInst, IDS_KRM_SENDMARK, szKeyStr, sizeof(szKeyStr));
    sndparams.mark = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFMARK);
    if (sndparams.mark >= ' ') 
        sndparams.mark = KRM_DEFMARK;

    LoadString(Kermit.hInst, IDS_KRM_RCVMARK, szKeyStr, sizeof(szKeyStr));
    rcvparams.mark = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFMARK);
    if (rcvparams.mark >= ' ') 
        rcvparams.mark = KRM_DEFMARK;

    LoadString(Kermit.hInst, IDS_KRM_SENDMAXLEN, szKeyStr, sizeof(szKeyStr));
    sndparams.maxpktsize = GetProfileInt(szAppStr,szKeyStr,KRM_DEFPACKETSIZE);
    if ((sndparams.maxpktsize > KRM_MAXPACKETSIZE) ||
			(sndparams.maxpktsize < KRM_MINPACKETSIZE))
        sndparams.maxpktsize = KRM_DEFPACKETSIZE;

    LoadString(Kermit.hInst, IDS_KRM_RCVMAXLEN, szKeyStr, sizeof(szKeyStr));
    rcvparams.maxpktsize = GetProfileInt(szAppStr,szKeyStr,KRM_DEFPACKETSIZE);
    if ((rcvparams.maxpktsize > KRM_MAXPACKETSIZE) ||
			(rcvparams.maxpktsize < KRM_MINPACKETSIZE))
        rcvparams.maxpktsize = KRM_DEFPACKETSIZE;

    LoadString(Kermit.hInst, IDS_KRM_SENDTIMEOUT, szKeyStr, sizeof(szKeyStr));
    sndparams.timeout = GetProfileInt(szAppStr,szKeyStr,KRM_DEFTIMEOUT);
    if ((sndparams.timeout > KRM_MAXTIMEOUT) ||
			(sndparams.timeout < KRM_MINTIMEOUT))
        sndparams.timeout = KRM_DEFTIMEOUT;

    LoadString(Kermit.hInst, IDS_KRM_RCVTIMEOUT, szKeyStr, sizeof(szKeyStr));
    rcvparams.timeout = GetProfileInt(szAppStr,szKeyStr,KRM_DEFTIMEOUT);
    if ((rcvparams.timeout > KRM_MAXTIMEOUT) ||
			(rcvparams.timeout < KRM_MINTIMEOUT))
        rcvparams.timeout = KRM_DEFTIMEOUT;

    LoadString(Kermit.hInst, IDS_KRM_SENDPADCOUNT, szKeyStr, sizeof(szKeyStr));
    sndparams.padcount = GetProfileInt(szAppStr,szKeyStr,KRM_DEFPADCOUNT);
    if (sndparams.padcount > KRM_MAXPADCOUNT)
        sndparams.padcount = KRM_DEFPADCOUNT;

    LoadString(Kermit.hInst, IDS_KRM_RCVPADCOUNT, szKeyStr, sizeof(szKeyStr));
    rcvparams.padcount = GetProfileInt(szAppStr,szKeyStr,KRM_DEFPADCOUNT);
    if (rcvparams.padcount > KRM_MAXPADCOUNT)
        rcvparams.padcount = KRM_DEFPADCOUNT;

    LoadString(Kermit.hInst, IDS_KRM_SENDPADCHAR, szKeyStr, sizeof(szKeyStr));
    sndparams.padchar = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFPADCHAR);
    if ((sndparams.padchar >= ' ') && (sndparams.padchar < DEL))
        sndparams.padchar = KRM_DEFPADCHAR;

    LoadString(Kermit.hInst, IDS_KRM_RCVPADCHAR, szKeyStr, sizeof(szKeyStr));
    rcvparams.padchar = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFPADCHAR);
    if ((rcvparams.padchar >= ' ') && (rcvparams.padchar != DEL))
        rcvparams.padchar = KRM_DEFPADCHAR;

    LoadString(Kermit.hInst, IDS_KRM_SENDEOL, szKeyStr, sizeof(szKeyStr));
    sndparams.eol = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFEOL);
    if (sndparams.eol >= ' ')
	sndparams.eol = KRM_DEFEOL;

    LoadString(Kermit.hInst, IDS_KRM_RCVEOL, szKeyStr, sizeof(szKeyStr));
    rcvparams.eol = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFEOL);
    if (rcvparams.eol >= ' ')
	rcvparams.eol = KRM_DEFEOL;
    
    LoadString(Kermit.hInst, IDS_KRM_SENDQUOTE, szKeyStr, sizeof(szKeyStr));
    sndparams.quote = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFQUOTE);
    if (((sndparams.quote <= 32) || (sndparams.quote >= 63)) &&
	((sndparams.quote <= 95) || (sndparams.quote >= 127)))
	sndparams.quote = KRM_DEFQUOTE;

    LoadString(Kermit.hInst, IDS_KRM_RCVQUOTE, szKeyStr, sizeof(szKeyStr));
    rcvparams.quote = (BYTE)GetProfileInt(szAppStr,szKeyStr,KRM_DEFQUOTE);
    if (((rcvparams.quote <= 32) || (rcvparams.quote >= 63)) &&
	((rcvparams.quote <= 95) || (rcvparams.quote >= 127)))
	rcvparams.quote = KRM_DEFQUOTE;

    Kermit.fpTimer = MakeProcInstance((FARPROC)krmDoTimeout, Kermit.hInst);
    Kermit.fpXfer = MakeProcInstance((FARPROC)krmXferDlgBox, Kermit.hInst);

    if  (!(Kermit.fpTimer && Kermit.fpXfer)) {
	krmShowMessage(IDS_KRM_CANNOTINIT);
	return FALSE;
    }
    return TRUE;
}

