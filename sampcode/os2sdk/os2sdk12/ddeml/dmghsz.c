/****************************** Module Header ******************************\
* Module Name: DMGLATOM.C
*
* This module contains functions used for HSZ control.
*
* Created:  8/2/88    sanfords
* Added case preservation/insensitive   1/22/90       Sanfords
* 6/12/90 sanfords  Fixed HSZ local string allocation size errors.
*                   Added latom validation checks
*
* Copyright (c) 1988, 1989  Microsoft Corporation
\***************************************************************************/
#include "ddemlp.h"


/*
 * since the top 12 bits of any latom is always 0 (unless we have > 16
 * atom tables!) we can encode any ulong into only 5 bytes.
 */
#define ENCODEBYTES         5
#define MAX_LATOMSTRSIZE    255 - ENCODEBYTES - 2
char szT[MAX_LATOMSTRSIZE + 1 + ENCODEBYTES];  /* used for HSZ expansion */

extern BOOL APIENTRY WinSetAtomTableOwner( HATOMTBL, PID );


/*********************** LATOM management functions *************************\
* An HSZ is a long atom which holds an encoded reference to two other long
* atoms.  One long atom is for the actual string, the other is for its
* uppercase version.  Two HSZs are ranked by their uppercase latoms.
* This makes HSZs case insensitive, case preserving.  An latom
* is an atom with an atom table index tacked onto the HIUSHORT part of 
* of the latom.  Strings too long for the atom manager are split up and
* each piece is prepended with a coded string that represents the
* LATOM of the rest of the string.  LATOM strings thus may be of any length.
* (up to 64K in this version)
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/

/***************************** Private Function ****************************\
* Allocates space in the DDE manager heap for a string queried from the DDE
* manager latomtable.  The case sensitive string is returned.
*
* This function should be serialized.  Memory allocation or latom table failure
* results in a 0 return value.
*
* 0 latoms result in a NULL terminated empty string.
*
* Note that *pcch is set to the length of the string INCLUDING the null
* terminator.  This way a wild string has a cch=1.
*
* History:
*   created     12/22/88        sanfords
*   11/10/89    sanfords        modified to return '\0' on invalid atom.
\***************************************************************************/
PSZ pszFromHsz(hsz, pcch)
HSZ hsz;
USHORT FAR *pcch;
{
    PSZ psz;
    LATOM latom;
    char sz[ENCODEBYTES + 1];
    register USHORT cch;

    SemCheckIn();

    if (hsz == 0)
        cch = 1;
    else {
        QuerylatomName((LATOM)hsz, sz, ENCODEBYTES + 1);
        latom = Decode(sz);     /* take origonal case version */
        cch = QuerylatomLength(latom) + 1;
    }
     
    psz = (PSZ)FarAllocMem(hheapDmg, cch);
    if (psz == 0) {
        *pcch = '\000';
        return(0);
    }

    if (hsz == 0) {
        *pcch = 1;
        *psz = '\0';
    } else {
        *pcch = cch;
        if (QuerylatomName(latom, psz, cch) == 0) {
            AssertF(FALSE, "pszFromHsz - bad latom");
            *psz = '\0';        /* invalid case - never expected */
        }
    }
    return(psz);
}


/***************************** Private Function ****************************\
* HSZ GetHsz(psz, cc, cp, fAdd)
* PSZ psz;
* USHORT cc;
* USHORT cp;
* BOOL fAdd;
*
* The goal of this routine is to convert a psz to an hsz.  This uses the
* atom manager for its dirty work.  This call has the side effect of
* incrementing the use count for the hsz returned and its associated latoms
* if fAdd is set.
*
* if fAdd is FALSE, NULL is returned if the hsz doesn't exist.
*
* History:
*   created     12/23/88    sanfords
\***************************************************************************/
HSZ GetHsz(psz, cc, cp, fAdd)
PSZ psz;
USHORT cc;
USHORT cp;
BOOL fAdd;
{
    LATOM latom1, latom2;
    USHORT cb;
    PSZ pszT;
    BOOL fNew = FALSE;
    HSZ hsz;

    /*
     * NULL or 0 length pszs are considered wild HSZs.
     */
    if (psz == NULL || *psz == '\0')
        return(0L);

    SemEnter();
    
    if (!(latom1 = FindAddlatom(psz, fAdd))) {
        AssertF(!fAdd, "GetHsz - Atom Add failed");
        SemLeave();
        return(0L);
    }
        
    cb = lstrlen(psz) + 1;
        
    if (!(pszT = FarAllocMem(hheapDmg, max(cb, ENCODEBYTES * 2 + 1)))) {
        SemLeave();
        return(0L);
    }
    
    CopyBlock((PBYTE)psz, (PBYTE)pszT, cb);
    WinUpper(DMGHAB, cp ? cp : syscc.codepage, cc ? cc : syscc.country, pszT);
    latom2 = FindAddlatom(pszT, fAdd);

    if (!latom2) {
        AssertF(!fAdd, "GetHsz - Atom Add(2) failed");
        hsz = 0;
    } else {
        *Encode(latom2, Encode(latom1, pszT)) = '\000';
        hsz = (HSZ)FindAddlatom(pszT, fAdd);
    }
    FarFreeMem(hheapDmg, pszT, max(cb, ENCODEBYTES * 2 + 1));
    SemLeave();
    return(hsz);
}



/*
 * Note that all three associated latoms are freed.
 */
BOOL FreeHsz(hsz)
HSZ hsz;
{
    char sz[ENCODEBYTES * 2 + 1];

    SemEnter();    
    if (hsz && QuerylatomName((LATOM)hsz, sz, ENCODEBYTES * 2 + 1)) {
        Freelatom(Decode((PBYTE)sz));
        Freelatom(Decode((PBYTE)&sz[ENCODEBYTES]));
        Freelatom((LATOM)hsz);
    }
    SemLeave();
    return(TRUE);
}
    


/*
 * Note that all three associated latoms are incremented.  
 */
BOOL IncHszCount(hsz)
HSZ hsz;
{
    char sz[ENCODEBYTES * 2 + 1];
    register BOOL fRet;

    if (hsz == 0)
        return(TRUE);
        
    SemEnter();
    
    QuerylatomName((LATOM)hsz, sz, ENCODEBYTES * 2 + 1);
    fRet = InclatomCount(Decode((PBYTE)sz)) &&
                InclatomCount(Decode((PBYTE)&sz[ENCODEBYTES])) &&
                InclatomCount((LATOM)hsz);
    SemLeave();
    return(fRet);
}



/***************************** Private Function ****************************\
* This routine adds an atom table and returns its handle.  Returns fSuccess.
*
* Effects cAtbls, aAtbls, iAtblCurrent;
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
BOOL AddAtomTable(fInit)
BOOL fInit;
{
    PHATOMTBL pat;

    SemEnter();    

    if (!(pat = (PHATOMTBL)FarAllocMem(hheapDmg,
            sizeof(HATOMTBL) * (cAtbls + 1)))) {
        SemLeave();
        return(FALSE);
    }
    
    if (!fInit) {
        CopyBlock((PBYTE)aAtbls, (PBYTE)pat, sizeof(HATOMTBL) * cAtbls);
        FarFreeMem(hheapDmg, aAtbls, sizeof(HATOMTBL) * cAtbls);
    }
    
    aAtbls = pat;

    if (!(aAtbls[cAtbls] = WinCreateAtomTable(0, 0))) 
        return(FALSE);
    /*
     * Share our atom tables with all processes...
     */
    if (!WinSetAtomTableOwner(aAtbls[cAtbls], NULL)) {
        AssertF(FALSE, "AddAtomTable - WinSetAtomTable failed");
        return(FALSE);
    }
    iAtblCurrent = cAtbls++;
    SemLeave();
    return(TRUE);
}



USHORT QueryHszLength(hsz)
HSZ hsz;
{
    char sz[ENCODEBYTES + 1];
    USHORT us;

    if (!hsz) 
        return(0);
    SemEnter();
    QuerylatomName((LATOM)hsz, sz, ENCODEBYTES + 1);
    us = QuerylatomLength(Decode(sz));
    SemLeave();
    return(us);
}



USHORT QueryHszName(hsz, psz, cchMax)
HSZ hsz;
PSZ psz;
USHORT cchMax;
{
    char sz[ENCODEBYTES + 1];
    register USHORT usRet;
    
    if (hsz == 0) {
        if (psz)
            *psz = '\000';
        return(1);
    } else {
        usRet = 0;
        SemEnter();
        if (QuerylatomName((LATOM)hsz, sz, ENCODEBYTES + 1))
            usRet = QuerylatomName(Decode(sz), psz, cchMax);
        SemLeave();
        return(usRet);
    }
}
     


/*
 * returns 0 if ==, -1 if hsz1 < hsz2, 1 if hsz1 > hsz2, 2 on error
 */
SHORT CmpHsz(hsz1, hsz2)
HSZ hsz1, hsz2;
{
    char sz[ENCODEBYTES * 2 + 1];
    LATOM latom;
    SHORT usRet;
    
    if (hsz1 == hsz2)
        return(0);
    if (!hsz1) 
        return(-1);
    if (!hsz2)
        return(1);

    usRet = 2;
    SemEnter();
    if (QuerylatomName((LATOM)hsz1, sz, ENCODEBYTES * 2 + 1)) {
        latom = Decode(&sz[ENCODEBYTES]);   /* use UPPERCASE form for comparison. */
        if (QuerylatomName((LATOM)hsz2, sz, ENCODEBYTES * 2 + 1)) {
            latom = latom - Decode(&sz[ENCODEBYTES]);
            usRet = latom == 0 ? 0 : (latom > 0 ? 1 : -1);
        }
    }
    SemLeave();
    return(usRet);
}


    

/***************************** Private Function ****************************\
* Returns the length of the latom given without NULL terminator.
* Wild LATOMs have a length of 0.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
USHORT QuerylatomLength(latom)
LATOM latom;
{
    USHORT cb;
    USHORT cbT = 0;
    BYTE ab[ENCODEBYTES + 1];

    AssertF(HIUSHORT(latom) < cAtbls, "Invalid latom");
    if (latom == 0)
        return(0);
    SemCheckIn();
    while (TRUE) {
        if (!(cb = WinQueryAtomLength(aAtbls[HIUSHORT(latom)],
                LOUSHORT(latom)))) {
            AssertF(cbT == 0, "QuerylatomLength - failed on continued latom");
            return(0);
        }
            
        cbT += cb;
        
        if (cb <= MAX_LATOMSTRSIZE) {
            return(cbT);
        }
            
        /*
         * it MUST be a huge latom.
         */
        if (!(WinQueryAtomName(aAtbls[HIUSHORT(latom)], LOUSHORT(latom),
                (PSZ)ab, ENCODEBYTES + 1))) {
            AssertF(FALSE, "QuerylatomLength - Length but no name");
            return(0);
        }
            
        latom = Decode(ab);
        cbT -= ENCODEBYTES;
    }
}



USHORT QuerylatomName(latom, psz, cchMax)
LATOM latom;
PSZ psz;
USHORT cchMax;
{
    USHORT cb;
    extern char szT[];

    if (HIUSHORT(latom) >= cAtbls) {
        AssertF(FALSE, "Invalid latom");
        psz[0] = '\0';
        return(0);
    }
    
    AssertF(latom != 0, "QuerylatomName - 0 latom");
    SemCheckIn();
    cb = WinQueryAtomLength(aAtbls[HIUSHORT(latom)], LOUSHORT(latom));
    if (cb > MAX_LATOMSTRSIZE) {
        if (!WinQueryAtomName(aAtbls[HIUSHORT(latom)], LOUSHORT(latom), szT,
                MAX_LATOMSTRSIZE + ENCODEBYTES + 1)) {
            AssertF(FALSE, "QuerylatomName - length but no name");
            return(0);
        }
        CopyBlock(szT + ENCODEBYTES, psz, min(MAX_LATOMSTRSIZE, cchMax));
        latom = Decode((PBYTE)szT);
        cb = MAX_LATOMSTRSIZE + QuerylatomName(latom, psz + MAX_LATOMSTRSIZE,
                cchMax > MAX_LATOMSTRSIZE ? cchMax - MAX_LATOMSTRSIZE : 0);
        
    } else {
        WinQueryAtomName(aAtbls[HIUSHORT(latom)], LOUSHORT(latom), psz, cchMax);
    }
    psz[cchMax - 1] = '\0';     /* add NULL terminator */
    return(min(cb, cchMax - 1));
}



/***************************** Private Function ****************************\
* This uses globals szT, aAtbls, cAtbls, and iAtblCurrent to add or
* find the latom for psz depending on fAdd.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
LATOM FindAddlatom(psz, fAdd)
PSZ psz;
BOOL fAdd;
{
    LATOM latom;

    AssertF(psz != NULL, "FindAddlatom - NULL psz");
    AssertF(*psz != '\0', "FindAddlatom - NULL psz string");
    SemCheckIn();
    if (lstrlen(psz) > MAX_LATOMSTRSIZE) {
        latom = FindAddlatom(psz + MAX_LATOMSTRSIZE, fAdd);
        CopyBlock((PBYTE)psz, Encode((ULONG)latom, szT),
                (ULONG)MAX_LATOMSTRSIZE - ENCODEBYTES + 1);
        szT[MAX_LATOMSTRSIZE + ENCODEBYTES] = '\0';
        latom = FindAddlatomHelper(szT, fAdd);
        return(latom);
    } else {
        return(FindAddlatomHelper(psz, fAdd));
    }
}




LATOM FindAddlatomHelper(psz, fAdd)
PSZ psz;
BOOL fAdd;
{
    int i;
    ATOM atom;
    ATOM (APIENTRY *lpfn)(HATOMTBL, PSZ);
    
    SemCheckIn();
    if (fAdd) {
        AssertF(++cAtoms, "Possible atom count overflow");
        lpfn = WinAddAtom;
    } else 
        lpfn = WinFindAtom;

    if (!(atom = (*lpfn)(aAtbls[i = iAtblCurrent], psz))) {
        /*
         * Must be full/not found, try all the existing tables
         */
        for (i = 0; i < cAtbls; i++) {
            if (i != iAtblCurrent) {
                if (atom = (*lpfn)(aAtbls[i], psz)) {
                    if (fAdd)
                        iAtblCurrent = i;
                    break;
                }
            }
        }
         
        if (!atom) {
            if (fAdd) {
                /*
                 * they're all full, make another table.
                 */
                if (!AddAtomTable(FALSE)) {
                    return(0L);
                }
                if (!(atom = (*lpfn)(aAtbls[iAtblCurrent], psz))) {
                    return(0L); 
                }
            } else {
                return(0L);
            }
        }
    }
    return((LATOM)MAKEP(i, atom));
}


    
    

BOOL InclatomCount(latom)
LATOM latom;
{
    AssertF(HIUSHORT(latom) < cAtbls, "Invalid latom");
    AssertF(latom != 0, "InclatomCount - 0 latom");
    SemCheckIn();
    AssertF(++cAtoms, "Possible atom count overflow");
    return(WinAddAtom(aAtbls[HIUSHORT(latom)], MAKEP(0XFFFF, LOUSHORT(latom))));
}



BOOL Freelatom(latom)
LATOM latom;
{
    AssertF(HIUSHORT(latom) < cAtbls, "Invalid latom");
    AssertF(latom != 0, "Freelatom - 0 latom");
    AssertF(WinQueryAtomUsage(aAtbls[HIUSHORT(latom)], LOUSHORT(latom)),
            "Freelatom - Freeing Non-existing atom");
    SemCheckIn();
        
    if (WinDeleteAtom(aAtbls[HIUSHORT(latom)], LOUSHORT(latom))) {
        AssertF(FALSE, "Freelatom - WinDeleteAtom failed");
        return(FALSE);
    }
    AssertF(--cAtoms >= 0, "Freelatom - negative atom count");
    return(TRUE);
}




#ifdef DEBUG
#define BASEVAL '0'     /* more readable for debugging */
#else
#define BASEVAL 1       /* less likely to conflict with a string */
#endif

/***************************** Private Function ****************************\
* Converts an latom into a ENCODEBYTES character string apropriate for
* atomization. (NULL terminator must be added)
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
PBYTE Encode(latom, pb)
ULONG latom;
PBYTE pb;
{
    int i;

    
    AssertF(HIUSHORT(latom) < cAtbls, "Invalid latom");
    for (i = 0; i < ENCODEBYTES; i++) {
        *pb++ = ((BYTE)latom & 0x0F) + BASEVAL;
        latom >>= 4;
    }
    return(pb);
}



/***************************** Private Function ****************************\
* This takes a pointer to a buffer of 8 bytes which is a coded LATOM and
* returns the LATOM.
*
* History:
*   Created     9/12/89    Sanfords
\***************************************************************************/
LATOM Decode(pb)
PBYTE pb;
{
    ULONG ul = 0;
    int i;

    for (i = ENCODEBYTES - 1; i >= 0; i--) {
        ul <<= 4;
        ul += (ULONG)(pb[i] - BASEVAL);
    }
    return((LATOM)ul);
}


/*
 * This routine extracts the hszItem out of an existing hData handle.
 * local conversations can use the hsz directly out of the handle while
 * non-dll conversations will have to generate the hsz from the string.
 */
HSZ GetHszItem(
PMYDDES pmyddes,
PCONVCONTEXT pCC,
BOOL fAdd)
{
    if (CheckSel(SELECTOROF(pmyddes)) >= sizeof(MYDDES) &&
            pmyddes->magic == MYDDESMAGIC) {
        if (fAdd) 
            IncHszCount(pmyddes->hszItem);
        return(pmyddes->hszItem);
    } else {
        return(GetHsz(DDES_PSZITEMNAME(pmyddes), pCC->idCountry,
                pCC->usCodepage, fAdd));
    }
}
