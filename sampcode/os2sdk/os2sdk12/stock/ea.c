/***	ea.c - layer for EA support
 *
 *	Author:
 *	    Benjamin W. Slivka
 *	    (c) 1990
 *	    Microsoft Corporation
 *
 *	History:
 *	    08-Feb-1990 bens	Initial version (Subset from EA.EXE sources)
 *	    02-May-1990 bens	Added SetEAValue (copied from ea.exe)
 *	    01-Jun-1990 bens	Support binary EAs
 */

#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_NOPM

#include <os2.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "ea.h"
#include "mem.h"


#ifdef CHECKASSERTS
#define dbg(a)	a
#else
#define dbg(a)
#endif

// Buffer sizes for EA API calls
#define CB_GEAL       400		// Enough for one GEA in list
#define CB_FEAL      2000		// Enough for large file list


char *	    TranslateValue(char *pbValue,USHORT cbValue,USHORT *pcbValue);


/***	EAQueryValue - Get text EA value from file
 *
 *	Entry
 *	    pszFile - File path
 *	    pszName - EA name
 *	    pcbValue - USHORT to receive value length
 *
 *	Exit-Success
 *	    returns non-zero pointer to value; Caller must free this!
 *		If value is ASCII
 *		    *pcbValue == 0;
 *		If value is BINARY
 *		    *pcbValue == length of value;
 *
 *	Exit-Failure
 *	    returns NULL
 */
char *EAQueryValue(char *pszFile,char *pszName,USHORT *pcbValue)
{
    USHORT	cb;
    EAOP	eaop;
    FEA *	pfea;
    FEA *	pfeaEnd;
    FEALIST *	pFEAList;
    GEA *	pgea;
    GEALIST *	pGEAList;
    char *	psz;
    char *	pszValue;
    USHORT	rc;

    //
    // Alloc GEAList and FEAList
    //
    pGEAList = MemAlloc(CB_GEAL);
    if (pGEAList == NULL) {
	return NULL;
    }

    pFEAList = MemAlloc(CB_FEAL);
    if (pFEAList == NULL) {
	MemFree(pGEAList);
	return NULL;
    }

    // Build GEA List with one GEA

    pgea = pGEAList->list;		// Point at first GEA
    cb = strlen(pszName);
    pgea->cbName = (UCHAR)cb;		// Set length
    memcpy(pgea->szName,pszName,cb+1);	// Copy name and NUL
    pgea = (GEA *)((char *)pgea + cb + sizeof(GEA));
    pGEAList->cbList = (char *)pgea - (char *)pGEAList; // Set buffer size

    // Get attribute value

    pFEAList->cbList = CB_FEAL; 	// Set size of FEA list
    eaop.fpGEAList = pGEAList;
    eaop.fpFEAList = pFEAList;

    rc = DosQPathInfo(pszFile,		// File path
		      FIL_QUERYEASFROMLIST, // info level
		      (PBYTE)&eaop,	// EAOP structure
		      sizeof(eaop),	// Size of EAOP
		      0L);		// Reserved
    pfea = (FEA *)pFEAList->list;	// Point at FEA

    //	NOTE: DosQPathInfo only fails if there is an inconsistency in
    //	      one of its parameters.  It DOES NOT fail if the EA is
    //	      not present.  Rather, on a file system that does not
    //	      support EAs, it appears to return pFEAList->cbList ==
    //	      sizeof(pFEAList->cbList), indicating no FEAs are present.
    //	      If the file system *does* support EAs, but the particular
    //	      EA is not present, pFEA->cbValue == 0.

    if ((rc == 0) &&			// Call succeeded,...
	((pFEAList->cbList) > sizeof(pFEAList->cbList)) && // FEA is there,...
	(pfea->cbValue > 0)) {		// and file has EA value!
	// Parse EA value
	cb = pfea->cbName;
	psz = (char *)pfea + sizeof(FEA); // Point at name
	pszValue = psz + cb + 1;	// Point at value
	psz = TranslateValue(pszValue,pfea->cbValue,pcbValue);
    }
    else
       psz = NULL;			// EA not present, or too big

    MemFree(pFEAList);
    MemFree(pGEAList);
    return psz;
}


/***	TranslateValue - produce printable representation of EA value
 *
 *	Entry
 *	    pbValue  - Value buffer
 *	    cbValue  - Length of value buffer
 *	    pcbValue - USHORT to receive actual value length
 *
 *	Exit-Success
 *	    Returns non-zero pointer to value; caller MUST free!
 *		If value is ASCII
 *		    *pcbValue == 0;
 *		If value is BINARY
 *		    *pcbValue == length of value;
 *
 *	Exit-Failure
 *	    Returns NULL
 *
 *
 *  EAT_MVMT - Multi-value, Multi-type
 *
 *	+------+----------+-------+------+--------+-------+---+---+---+---+
 *	| Type | Codepage | Count | Type | Length | Value |...| T | L | V |
 *	+------+----------+-------+------+--------+-------+---+---+---+---+
 *	   us	    us	     us      us      us       ?
 *	\________________________/ \_____________________/     \_________/
 *	   MVMT header			  Value 1		 Value N
 *
 */
char * TranslateValue(char *pbValue,USHORT cbValue,USHORT *pcbValue)
{
    USHORT cb=cbValue;
    USHORT codePage;
    USHORT cValue;
    char * pbDst;
    char * pbSrc;
    char * pszNew;
    USHORT type;

    // Parse MVMT header, if present

    pbSrc = pbValue;

    type = *(USHORT *)pbSrc;		// Get EA value type
    if (type == EAT_MVMT) {
	pbSrc += sizeof(USHORT);	// Skip type
	codePage = *((USHORT*)pbSrc)++; // Get code page
	cValue = *((USHORT*)pbSrc)++;	// Get count of values
	if (cValue != 1)		// Not exactly one value
	    return NULL;		//  Fail
	type = *(USHORT *)pbSrc;	// Get EA value type
    }


    // Parse value

    if ( (type == EAT_ASCII) || (type == EAT_BINARY) ) {
	pbSrc += sizeof(USHORT);	// Skip type
	cb = *((USHORT *)pbSrc)++;	// Get data length

	// Allocate buffer for data

	pszNew = MemAlloc(cb+1);	// Leave room for NUL, in ASCII case
	if (pszNew == NULL)
	    return NULL;
	pbDst = pszNew;

	// Copy data

	memcpy(pbDst,pbSrc,cb); 	// Copy value
	pbDst += cb;			// Advance destination pointer

	if (type == EAT_ASCII) {
	    *pbDst++ = '\0';		// Terminate ASCIIZ string
	    *pcbValue = 0;		// Indicate value is ASCIIZ
	}
	else
	    *pcbValue = cb;		// Indicate value is binary
	return pszNew;			// Return value
    }
    else
	return NULL;			//  Fail
}


/***	EASetValue - Create/Change/Delete an EA
 *
 *	Entry
 *	    pszFile  - file path
 *	    pszName  - EA name
 *	    cbValue  - EA length; 0 => pszValue is ASCIIZ
 *	    pszValue - EA value; NULL to delete EA
 *
 *	Exit-Success
 *	    returns TRUE
 *
 *	Exit-Failure
 *	    returns FALSE
 *
 */
BOOL EASetValue(char *pszFile,char *pszName,USHORT cbValue,char *pszValue)
{
    USHORT	cbName;
    EAOP	eaop;
    FEA *	pfea;
    FEALIST *	pFEAList;
    char *	psz;
    USHORT	rc;
    USHORT	type;

    // Determine operation

    if (pszValue == NULL) {		// Delete this EA
	type = EAT_ASCII;
	cbValue = 0;
    }
    else if (cbValue == 0) {		// Create/Change value
	type = EAT_ASCII;
	cbValue = strlen(pszValue);	// Compute length (do not count NUL!)
    }
    else {				// Create/Change Binary value
	type = EAT_BINARY;
    }

    //
    // Alloc FEA List
    //
    pFEAList = MemAlloc(CB_FEAL);
    if (pFEAList == NULL)
	return FALSE;

    cbName = strlen(pszName);

    //
    // Build EA structure
    //
    pfea = (FEA *)pFEAList->list;	// Point at first FEA
    pfea->fEA = 0;			// No flag settings
    pfea->cbName = (UCHAR)cbName;	// Set name length
    pfea->cbValue = cbValue;		// Set value length

    psz = (char *)pfea + sizeof(FEA);	// Point at location for name
    memcpy(psz,pszName,cbName+1);	// Copy Name *and* NUL
    psz += cbName+1;			// Point at location for value
    if (cbValue > 0) {			// Edit/Create EA
	*((USHORT *)psz)++ = EAT_MVMT;	// Set MVMT type (to record code page!)
	*((USHORT *)psz)++ = NULL;	// Set codepage
	*((USHORT *)psz)++ = 1; 	// Only one TLV record
	*((USHORT *)psz)++ = type;	// Set EA type
	*((USHORT *)psz)++ = cbValue;	// Set ASCII length

	pfea->cbValue += 5*sizeof(USHORT); // MVMT header and type and length
	memcpy(psz,pszValue,cbValue);	// Copy Value
    }
    pfea = (FEA *)(psz + cbValue);	// Point at byte after FEA

    //
    // Set size of FEA List (only one FEA)
    //

    pFEAList->cbList = (char *)pfea - (char *)pFEAList;

    eaop.fpGEAList = NULL;
    eaop.fpFEAList = pFEAList;

    rc = DosSetPathInfo(pszFile,	    // File path
			FIL_QUERYEASIZE,    // Set EA
			(PBYTE)&eaop,	    // EAOP structure
			sizeof(eaop),	    // Size of EAOP
			0,		    // Options
			0L);		    // Reserved
    MemFree(pFEAList);
    return (rc == 0);
}
