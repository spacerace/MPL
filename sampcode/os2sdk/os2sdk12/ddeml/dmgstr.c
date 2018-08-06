/****************************** Module Header ******************************\
* Module Name: DMGSTR.C
*
* DDE manager string handling routines
*
* Created: 1/31/88 Sanford Staab
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddemlp.h"
#include "ctype.h"



/***************************** Private Function ****************************\
*
* returns string length not counting null terminator.
*
* History:  1/1/89  created     sanfords
\***************************************************************************/
int lstrlen(psz)
PSZ psz;
{
    int c = 0;

    while (*psz != 0) {
        psz++;
        c++;
    }
    return(c);
}

/***************************** Public  Function ****************************\
* Concatonates psz1 and psz2 into psz1.
* returns psz pointing to end of concatonated string.
* pszLast marks point at which copying must stop.  This makes this operation
* safe for limited buffer sizes.
*
* History:  1/1/89  created sanfords
\***************************************************************************/
PSZ lstrcat(psz1, psz2, pszLast)
PSZ psz1, psz2, pszLast;
{
    psz1 += lstrlen(psz1);
    while (*psz2 != '\0' && psz1 < pszLast) {
        *psz1++ = *psz2++;
    }
    *psz1 = '\0';
    return(psz1);
}

/***************************** Private Function ****************************\
* DESCRIPTION: ASCII dependent converter of DDE structure data to a string.
* returns psz pointing to end of copy.
* During monitoring we allocate segments as gettable so we can monitor them.
*
* History:      Created 1/31/89         sanfords
\***************************************************************************/
PSZ pddesToPsz(msg, pddes, psz, pszLast)
USHORT msg;
PDDESTRUCT pddes;
PSZ psz;
PSZ pszLast;
{
    USHORT cb;
    PBYTE pData;
#define pDdeInit ((PDDEINIT)pddes)

    *psz = '\0';
    switch (msg) {
    case WM_DDE_REQUEST:
    case WM_DDE_ACK:
    case WM_DDE_DATA:
    case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE:
    case WM_DDE_POKE:
    case WM_DDE_EXECUTE:
        psz = lstrcat(psz, "S:", pszLast);
        psz = Status(pddes->fsStatus, psz, pszLast);
        psz = lstrcat(psz, " F:", pszLast);
        psz = Format(pddes->usFormat, psz, pszLast);
        psz = lstrcat(psz, " I:", pszLast);
        psz = lstrcat(psz, DDES_PSZITEMNAME(pddes), pszLast);
        if (pddes->cbData)
            psz = lstrcat(psz, "\n\r  Data:", pszLast);
        pData = DDES_PABDATA(pddes);

        for (cb = 0; (ULONG)cb < pddes->cbData && psz < pszLast; cb++, pData++) {
            /*
             * new line every 64 chars
             */
            if ((cb & 0x3F) == 0) {
                *psz = '\0';
                psz = lstrcat(psz, "\n\r    ", pszLast);
            }
            if (*pData > 0x20)
                *psz = *pData;
            else
                *psz = '.';
                
            *psz++ = *psz & 0x7f;
        }
        CopyBlock("\n\r", pszLast - 3, 3L);
        break;

    case WM_DDE_INITIATEACK:
    case WM_DDE_INITIATE:
        if (CheckSel(SELECTOROF(pDdeInit))) {
            psz = lstrcat(psz, "A:", pszLast);
            psz = lstrcat(psz, pDdeInit->pszAppName, pszLast);
            psz = lstrcat(psz, " T:", pszLast);
            psz = lstrcat(psz, pDdeInit->pszTopic, pszLast);
        }
        break;

    case WM_DDE_TERMINATE:
        break;
    }
    *psz = '\0';
    return(psz);
    
#undef pDdeInit

}


PSZ Status(fs, psz, pszLast)
USHORT fs;
PSZ psz;
PSZ pszLast;
{
    if (fs & DDE_FACK) {
        psz = lstrcat(psz, "ACK ", pszLast);
    }
    if (fs & DDE_FBUSY) {
        psz = lstrcat(psz, "BUSY ", pszLast);
    }
    if (fs & DDE_FNODATA) {
        psz = lstrcat(psz, "NODATA ", pszLast);
    }
    if (fs & DDE_FACKREQ) {
        psz = lstrcat(psz, "ACKREQ ", pszLast);
    }
    if (fs & DDE_FRESPONSE) {
        psz = lstrcat(psz, "RESPONSE ", pszLast);
    }
    if (fs & DDE_NOTPROCESSED) {
        psz = lstrcat(psz, "NOTPROCESSED ", pszLast);
    }
    if (fs & DDE_FAPPSTATUS) {
        psz = lstrcat(psz, "APPSTAT=", pszLast);
        psz = itoa(fs & DDE_FAPPSTATUS, psz, pszLast);
        *psz++ = ' ';
        *psz++ = '\0';
    }
    if (fs & DDE_FRESERVED) {
        psz = lstrcat(psz, "RESERVED=", pszLast);
        psz = itoa(fs & DDE_FRESERVED, psz, pszLast);
    }
    return(psz);
}


PSZ Format(fmt, psz, pszLast)
USHORT fmt;
PSZ psz;
PSZ pszLast;
{
    if (fmt > 0xbfff) {
        *psz++ = '"';
        psz += WinQueryAtomName(WinQuerySystemAtomTable(), fmt, psz, pszLast - psz);
        *psz++ = '"';
        *psz = '\0';
    } else if (fmt == DDEFMT_TEXT) {
        psz = lstrcat(psz, "TEXT", pszLast);
    } else {
        psz = itoa(fmt, psz, pszLast);
    }
    return(psz);
}




/***************************** Private Function ****************************\
* DESCRIPTION: puts an apropriate string for a DDE message into psz. pszLast
* specifies the last spot to copy.  Returns a psz pointing to the end of
* the copyed data.
*
* History:      Created 1/31/89         sanfords
\***************************************************************************/
PSZ ddeMsgToPsz(msg, psz, pszLast)
USHORT msg;
PSZ psz;
PSZ pszLast;
{
    psz = lstrcat(psz, " ", pszLast);
    if (msg < WM_DDE_FIRST || msg > WM_DDE_LAST) {
        psz = itoa(msg, psz, pszLast);
    } else {
        WinLoadString(DMGHAB, hmodDmg, msg, pszLast - psz + 1, psz);
        psz += lstrlen(psz);
    }
    return(lstrcat(psz, "(", pszLast));
}

/***************************** Private Function ****************************\
* DESCRIPTION:
*   fills psz with a hex string "0xdddd" and returns psz pointing to the 0
*   terminator at the end.  copying will never go beyond pszLast.
*
* History:      Created 1/31/89        sanfords
\***************************************************************************/
PSZ itoa(us, psz, pszLast)
USHORT us;
PSZ psz;
PSZ pszLast;
{
    if (psz > pszLast - 7)
        return(psz);
    *psz++ = '0';
    *psz++ = 'x';
    return(stoa(psz, us));
}

/***************************** Private Function ****************************\
* DESCRIPTION:
*   fills psz with a hex string "0xdddddddd" and returns psz pointing to the 0
*   terminator at the end.  copying will never go beyond pszLast.
*
* History:      Created 1/31/89        sanfords
\***************************************************************************/
PSZ ltoa(ul, psz, pszLast)
ULONG ul;
PSZ psz;
PSZ pszLast;
{
    if (psz > pszLast - 11)
        return(psz);
    *psz++ = '0';
    *psz++ = 'x';
    psz = stoa(psz, HIUSHORT(ul));
    return(stoa(psz, LOUSHORT(ul)));
}


/***************************** Private Function ****************************\
* DESCRIPTION:
*   fills psz with a hex string "dddd" and returns psz pointing to the 0
*   terminator at the end.
*
* History:      Created 1/31/89        sanfords
\***************************************************************************/
PSZ stoa(psz, us)
PSZ psz;
USHORT us;
{
    static char dtoa[] = "0123456789abcdef";

    *psz++ = dtoa[(us & 0xf000) >> 12];
    *psz++ = dtoa[(us & 0xf00) >> 8];
    *psz++ = dtoa[(us & 0xf0) >> 4];
    *psz++ = dtoa[us & 0xf];
    *psz = '\0';
    return(psz);
}


/*
 * Decimal to ascii
 */
PSZ dtoa(psz, us, fRecurse)
PSZ psz;
USHORT us;
BOOL fRecurse;
{
    if (us > 9) {
        psz = dtoa(psz, us / 10, TRUE);
        *psz++ = (UCHAR)(us % 10) + '0';
    } else if (us > 0)
        *psz++ = (UCHAR)us + '0';
    else if (!fRecurse)
        *psz++ = '0';
    *psz = '\000';
    return(psz);
}
   

/***************************** Private Function ****************************\
* DESCRIPTION:
*   fills psz with a hex time stamp and returns psz pointing to the 0
*   terminator at the end.
*
* History:      Created 5/9/89        sanfords
\***************************************************************************/
PSZ timestamp(psz, pszLast)
PSZ psz;
PSZ pszLast;
{
    DATETIME dt;
    static USHORT prevTime = 0;
    USHORT Time;

    DosGetDateTime(&dt);
    Time = MAKESHORT(dt.hundredths, dt.seconds);
    psz = lstrcat(psz, "----------- dTime=", pszLast);
    psz = itoa(Time - prevTime, psz, pszLast);
    psz = lstrcat(psz, " ", pszLast);
    prevTime = Time;
    return(psz + lstrlen(psz));
}
