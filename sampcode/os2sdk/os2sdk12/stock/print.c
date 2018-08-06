/***	print.c - printing support
 *
 *	This file presents code to make if VERY EASY for an application
 *	to select a printer, manipulate job properties, and open a DC
 *	for printing.
 *
 *	Author:
 *	    Benjamin W. Slivka
 *
 *	History:
 *	    12-May-1990 bens	Initial version
 *	    16-May-1990 bens	Skeletal job property routines
 *	    31-May-1990 bens	Final job property routines
 *
 *	APIs Offered:
 *	    PrintQueryPrinterList    - Get printer list
 *	    PrintQueryNextPrinter    - Enumerate members of printer list
 *	    PrintQueryDefaultPrinter - Get default printer
 *	    PrintQueryPrinterInfo    - Get information about a printer
 *
 *	    PrintOpenDC 	     - Open a printer DC
 *
 *	    PrintQueryJobProperties  - Get JP from a printer
 *	    PrintChangeJobProperties - Interact with user to change JP
 *	    PrintMatchPrinter	     - Find printer which matches JP
 *
 *	Key points:
 *
 *	1)  [PM_SPOOLER,PRINTER] in OS2SYS.INI specifies the default printer.
 *		This entry is of use *only* if [PM_SPOOLER,QUEUE] is empty.
 *
 *	2)  If a [PM_SPOOLER_PRINTER,xxx] entry in OS2SYS.INI entry refers
 *		to *more than one* driver, *and* the printer is *not*
 *		associated with a queue, then we always use the first driver
 *		in the list.  There is no information in the system to help
 *		us choose among the drivers, so we make the easy choice.
 */

#define INCL_DEV

#define INCL_WINSHELLDATA

#include <os2.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ids.h"
#include "file.h"
#include "mem.h"
#include "print.h"


//************************************************************************
//
//  PRIVATE Types and Definitions
//
//************************************************************************


// SEARCHP - Search structure for PrinterBegin/Next/EndSearch
//
typedef struct { /* srchp */
    char *  pchName;			// Driver name list
    char *  pchNext;			// Next name in achNames to consume
} SEARCHP;
typedef SEARCHP *PSEARCHP; /* psrchp */


// PRINTER - Structure for printer destination (used for DevOpenDC)
//
typedef struct _printer {  /* prt */
    LONG    lType;			// Printer type (OD_DIRECT/OD_QUEUED)
    char *  pszName;			// Name (either queue or printer name)
					//  NOTE: This pointer always points
					//	  to either pszLogAddr or
					//	  pszPrinter.  This pointer
					//	  should never be freed!
    char *  pszLogAddr; 		// Logical address for DevOpenDC
    char *  pszPrinter; 		// Printer Name
    char *  pszDriver;			// Driver name
    char *  pszModel;			// Driver model name
    char *  pszDescription;		// Description
    BOOL    fMultipleDrivers;		// TRUE => pszDriver/pszModel contain
					//	the first driver in the printer
					//	definition.  If this is a queue
					//	(lType == OD_QUEUED), the
					//	driver/model must be gotten
					//	from PM_SPOOLER_QUEUE_DD.
    USHORT  cbDriverData;		// Length of driver data
    BYTE *  pbDriverData;		// Driver data
    struct _printer *pprtNext;		// Link to next printer
    struct _printer *pprtPrev;		// Link to previous printer
    struct _printerlist *pprtlist;	// Owning list
} PRINTER;
typedef PRINTER *PPRINTER;  /* pprt */


//  PRINTERLIST - Structure for list of printers
//
typedef struct _printerlist { /* prtlist */
    HAB 	hab;			// Anchor block for list
    PPRINTER	pprtHead;		// Head of printer list
    PPRINTER	pprtTail;		// Tail of printer list
    PPRINTER	pprtDefault;		// System default printer
} PRINTERLIST;
typedef PRINTERLIST *PPRINTERLIST; /* pprtlist */


//  JOBPROP - Structure for exporting/importing job properties
//
typedef struct {    /* jp */
    USHORT  cb; 			// Total buffer size
    USHORT  cbName;			// Size of printer name (counting NUL)
    USHORT  cbDriver;			// Size of driver name (counting NUL)
    USHORT  cbModel;			// Size of model name (counting NUL)
    USHORT  cbData;			// Size of driver data
 //
 // The "fields" below show the order in which the strings/data follow the
 // structure "header":
 //
 // CHAR    achName[];			// Printer name (asciiz)
 // CHAR    achDriver[];		// Driver name (asciiz)
 // CHAR    achModel[]; 		// Model name (asciiz)
 // BYTE    abData[];			// Driver data (binary)
 //
} JOBPROP;
typedef JOBPROP *PJOBPROP;  /* pjp */


//************************************************************************
//
//  PRIVATE Global Variables
//
//************************************************************************


static	char	achPath[CCHMAXPATH];


//************************************************************************
//
//  PRIVATE Function Prototypes
//
//************************************************************************


BOOL	  AddPrinter(PPRINTERLIST pprtlist,PPRINTER pprtNew);
PPRINTER  ClonePrinter(PPRINTER pprtOriginal,char *pszLogAddr);
PPRINTER  CreatePrinter(LONG  lType,
			char *pszLogAddr,
			char *Printer,
			char *pszDriver,
			char *pszModel,
			BOOL  fMultipleDrivers);
VOID	  DestroyPrinter(PPRINTER pprt);
VOID	  GetDriverData(PPRINTER pprt);
VOID	  GetDriverModel(PPRINTER pprt);
VOID	  GetDescription(PPRINTER pprt);
PPRINTER  GetPrinterData(char *pszPrinter);
PPRINTER  MatchPrinter(PPRINTERLIST pprtlist,char *pszQueue,char *pszPrinter);
PSEARCHP  PrinterBeginSearch(PPRINTER *ppprt);
BOOL	  PrinterEndSearch(PSEARCHP psrchp);
PPRINTER  PrinterNextSearch(PSEARCHP psrchp);
VOID	  QueryDefaultPrinterQueue(char **ppszQueue,char **ppszPrinter);
PPRINTER  SplitPrinter(PPRINTERLIST pprtlist,PPRINTER pprtOld);
void	  TrimTrailingSemicolon(char *psz);


//************************************************************************
//
//  PUBLIC Functions
//
//************************************************************************


/***	PrintCreatePrinterList - Get list of printers
 *
 *	Use PrintQueryNextPrinter to enumerate printers.
 *	Use PrintQueryPrinterInfo to get information on a printer.
 *
 *	Entry
 *	    hab - anchor block handle
 *
 *	Exit-Success
 *	    Returns HPRINTERLIST
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HPRINTERLIST PrintCreatePrinterList(HAB hab)
{
    PSEARCHP	    psrchp=NULL;
    static PPRINTER pprt;
    PPRINTER	    pprtQueue=NULL;
    PPRINTER	    pprtPrinter=NULL;
    PPRINTERLIST    pprtlist;
    static char *   pszPrinter;
    static char *   pszQueue;

    // Build initial printer list

    psrchp = PrinterBeginSearch(&pprt); // Get first printer
    if (psrchp == NULL) 		// No printers installed
	return NULL;			//  Fail

    pprtlist = MemAlloc(sizeof(PRINTERLIST)); // Create list
    if (pprtlist == NULL) {		// No memory
	PrinterEndSearch(psrchp);	// Done searching
	return NULL;			// Fail
    }
    pprtlist->pprtHead = NULL;
    pprtlist->pprtTail = NULL;

    // Add all printers

    while (pprt != NULL) {		// Get all printers
	AddPrinter(pprtlist,pprt);	// Add to list
	pprt = PrinterNextSearch(psrchp); // Get another printer
    }
    PrinterEndSearch(psrchp);		// Done searching

    // Now we have a list of printers that may have multiple queues
    // associated with them, and may have multiple drivers.
    //
    // Here, we split entries to get one per queue
    // It may still be the case that the driver.model are incorrect
    // for the Queue.  However, we do a "lazy evaluation" of this,
    // using the fMultipleDrivers flag.  See PrintQueryPrinterInfo
    // and GetDriverModel for more details.

    // Split any multi-queue entries

    for (pprt=pprtlist->pprtHead; pprt!=NULL; ) {
	pprt = SplitPrinter(pprtlist,pprt);	 // Split, if necessary
    }

    // Find default printer

    QueryDefaultPrinterQueue(&pszQueue,&pszPrinter);
    pprt = MatchPrinter(pprtlist,pszQueue,pszPrinter);
    if (pprt == NULL)			// No default printer
	pprt = pprtlist->pprtHead;	//  Choose first printer
    pprtlist->pprtDefault = pprt;	// Remember default printer

    if (pszQueue != NULL)		// Free default printer/queue names
	MemFree(pszQueue);
    if (pszPrinter != NULL)
	MemFree(pszPrinter);

    pprtlist->hab = hab;		// Remember hab
    return pprtlist;			// Return list to caller
}


/***	PrintDestroyPrinterList - Destroy printer list
 *
 *	Entry
 *	    hprtlist - printer list
 *
 *	Exit-Success
 *	    Returns TRUE, list destroyed
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL	  PrintDestroyPrinterList(HPRINTERLIST hprtlist)
{
    PPRINTER	    pprt;
    PPRINTERLIST    pprtlist = hprtlist;
    PPRINTER	    pprtNext;

    if (pprtlist == NULL)
	return FALSE;

    MemAssert(pprtlist);

    pprt = pprtlist->pprtHead;		// Start with first printer
    while (pprt != NULL) {		// Loop through list
	pprtNext = pprt->pprtNext;	// Next printer
	DestroyPrinter(pprt);		// Destroy this one
	pprt = pprtNext;		// Advance
    }
    return TRUE;
}


/***	PrintQueryNextPrinter - Get a printer from a list of printers
 *
 *	Use PrintQueryPrinterInfo to get information on a printer.
 *
 *	Entry
 *	    hprtlist - printer list
 *	    hprt     - starting printer
 *			  NULL => return first printer in list
 *			 !NULL => return printer after hprt
 *
 *	Exit-Success
 *	    Returns HPRT
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HPRINTER PrintQueryNextPrinter(HPRINTERLIST hprtlist,HPRINTER hprt)
{
    PPRINTERLIST    pprtlist = hprtlist;
    PPRINTER	    pprt = hprt;

    MemAssert(pprtlist);
    if (pprt == NULL)			// Get first printer
	return pprtlist->pprtHead;
    else {				// Get next printer
	MemAssert(pprt);
	if (pprt->pprtlist != pprtlist) // Verify hprt is on specified list
	    MemAssert(NULL);
	return pprt->pprtNext;
    }
}


/***	PrintQueryDefaultPrinter - Return default printer
 *
 *	Entry
 *	    hprtlist - printer list
 *
 *	Exit
 *	    Returns HPRINTER of default printer.
 */
HPRINTER PrintQueryDefaultPrinter(HPRINTERLIST hprtlist)
{
    PPRINTERLIST    pprtlist = hprtlist;

    MemAssert(pprtlist);
    return pprtlist->pprtDefault;
}


/***	PrintQueryPrinterInfo - Get info on a printer
 *
 *	Entry
 *	    hprt  - printer handle
 *	    index - index of printer info
 *
 *	Exit-Success
 *	    Returns requested information
 *		PQPI_NAME	  - char * - Nice name
 *		PQPI_LOG_ADDRESS  - char * - Logical address
 *		PQPI_DRIVER	  - char * - Driver name
 *		PQPI_MODEL	  - char * - Model name
 *		PQPI_TYPE	  - ULONG  - OD_QUEUED or OD_DIRECT
 *		PQPI_PRINTER	  - char * - Printer name
 *		PQPI_DESCRIPTION  - char * - Description
 *
 *	Exit-Failure
 *	    Returns -1L
 */
ULONG PrintQueryPrinterInfo(HPRINTER hprt,USHORT index)
{
    PPRINTER	pprt = hprt;

    MemAssert(pprt);
    switch (index) {
	case PQPI_NAME:
	    return (ULONG)(VOID FAR *)pprt->pszName;

	case PQPI_LOG_ADDRESS:
	    return (ULONG)(VOID FAR *)pprt->pszLogAddr;

	case PQPI_DRIVER:
	    GetDriverModel(pprt);	// Make sure we get correct data
	    return (ULONG)(VOID FAR *)pprt->pszDriver;

	case PQPI_MODEL:
	    GetDriverModel(pprt);	// Make sure we get correct data
	    return (ULONG)(VOID FAR *)pprt->pszModel;

	case PQPI_TYPE:
	    return pprt->lType;

	case PQPI_PRINTER:
	    return (ULONG)(VOID FAR *)pprt->pszPrinter;

	case PQPI_DESCRIPTION:
	    GetDescription(pprt);	// Get appropriate description
	    return (ULONG)(VOID FAR *)pprt->pszDescription;

	default:
	    return -1L;
    }
}


/***	PrintOpenDC - Open DC on specified printer
 *
 *	Entry
 *	    hab 	- Anchor block
 *	    hprt	- Printer handle
 *	    pszDataType - "PM_Q_STD" or "PM_Q_RAW"
 *
 *	Exit-Success
 *	    Returns hdc
 *
 *	Exit-Failure
 *	    Returns NULL
 */
HDC PrintOpenDC(HAB hab,HPRINTER hprt,char *pszDataType)
{
    DEVOPENSTRUC    dop;
    HDC 	    hdc;
    PPRINTER	    pprt = hprt;

    MemAssert(pprt);
    GetDriverData(pprt);		// Make sure we have driver data

    dop.pszLogAddress = pprt->pszLogAddr; // Logical address
    dop.pszDriverName = pprt->pszDriver; // Driver name (PSCRIPT)
    dop.pdriv	      = (VOID *)pprt->pbDriverData; // Driver data
    dop.pszDataType   = pszDataType;	// PM_Q_STD or PM_Q_RAW

 // dop.pszComment    = "stock chart";	// Comment for OD_Q
 // dop.pszQueueProcName   = NULL;	// queue processor; NULL => use default
 // dop.pszQueueProcParams = NULL;	// parms for queue processor
 // dop.pszSpoolerParams   = NULL;	// spooler parms (use NULL!)
 // dop.pszNetworkParams   = NULL;	// network parms (use NULL!)

    hdc = DevOpenDC(
	    hab,			// anchor block
	    pprt->lType,		// DC type: OD_DIRECT or OD_QUEUED
	    "*",			// device info(?) "*"
	    4L, 			// count of info in DEVOPENSTRUC
	    (PDEVOPENDATA)&dop, 	// DEVOPENSTRUC
	    NULL			// Compatible DC (use NULL?)
    );
    return hdc;
}


/***	PrintQueryJobProperties - Get printer job properties
 *
 *	Entry
 *	    hprt    - printer handle
 *	    pcbData - pointer to size of pb buffer in bytes
 *			Pass 0 to determine required buffer size.
 *	    pbData  - pointer to buffer to receive job properties
 *
 *	Exit-Success
 *	    Returns TRUE, job properties retrieved
 *		pb filled in with job property data
 *		pcb filled in with size of pb data
 *
 *	Exit-Failure
 *	    Returns FALSE, pb too small for job properties
 *		pcb filled in with required size
 *
 *	Note
 *	    (1) It is valid for a printer to require no job properties.
 *		In this case, *pcb == 0.
 */
BOOL PrintQueryJobProperties(HPRINTER hprt,USHORT *pcbData,BYTE *pbData)
{
    USHORT	cb;			// Total buffer size
    USHORT	cbData; 		// Size of driver data
    USHORT	cbDriver;		// Size of driver name (counting NUL)
    USHORT	cbModel;		// Size of model name (counting NUL)
    USHORT	cbName; 		// Size of printer name (counting NUL)
    BYTE *	pb;
    PJOBPROP	pjp;
    PPRINTER	pprt = hprt;

    MemAssert(pprt);
    GetDriverData(pprt);		// Make sure we have driver data

    cbName   = strlen(pprt->pszName)+1; // Count NUL
    cbDriver = strlen(pprt->pszDriver)+1; // Count NUL
    cbModel  = strlen(pprt->pszModel)+1; // Count NUL
    cbData   = pprt->cbDriverData;	// Driver data length

    // Check size of return buffer

    cb = sizeof(JOBPROP)+cbName+cbDriver+cbModel+cbData;
    if (*pcbData < cb) {		// Check buffer size
	*pcbData = cb;			//  Too small, return required size
	return FALSE;			// Indicate failure
    }

    // Fill in return buffer

    pjp = (PJOBPROP)pbData;
    pjp->cb	  = cb;
    pjp->cbName   = cbName;
    pjp->cbDriver = cbDriver;
    pjp->cbModel  = cbModel;
    pjp->cbData   = cbData;

    pb = (BYTE *)pjp+sizeof(JOBPROP);	// Start of data space

    pb = memcpy(pb,pprt->pszName,cbName);
    pb += cbName;

    pb = memcpy(pb,pprt->pszDriver,cbDriver);
    pb += cbDriver;

    pb = memcpy(pb,pprt->pszModel,cbModel);
    pb += cbModel;

    memcpy(pb,pprt->pbDriverData,cbData);

    *pcbData = cb;			// Return actual size
    return TRUE;
}


/***	PrintChangeJobProperties - Let user change printer job properties
 *
 *	Entry
 *	    hprt    - printer handle
 *
 *	Exit-Success
 *	    Returns TRUE, job properties changed
 *
 *	Exit-Failure
 *	    Returns FALSE, job properties not changed
 */
BOOL PrintChangeJobProperties(HPRINTER hprt)
{
    HAB 	hab;
    BYTE *	pb;
    PPRINTER	pprt = hprt;
    char *	pszName;
    LONG	rc;

    MemAssert(pprt);
    MemAssert(pprt->pprtlist);
    hab = pprt->pprtlist->hab;

    GetDriverData(pprt);		// Try to get driver data

    if (pprt->pbDriverData != NULL) {	// We do have it
	pb = pprt->pbDriverData;	//  Use our data as starting point
	pszName = NULL; 		// Do not use printer name
    }
    else {
	pb = NULL;			// We have no data
	pszName = pprt->pszName;	// Tell driver to get it from printer
    }

    rc = DevPostDeviceModes(
	    hab,			// Anchor block
	    (VOID *)pb, 		// Buffer for data
	    pprt->pszDriver,		// Driver name
	    pprt->pszModel,		// Device name
	    pszName,			// Printer name
	    DPDM_POSTJOBPROP		// Option
    );
    return (rc == DEV_OK);
}


/***	PrintResetJobProperties - Reset printer to default job properties
 *
 *	Entry
 *	    hprt - printer handle
 *
 *	Exit
 *	    Job properties set to default for printer
 */
VOID PrintResetJobProperties(HPRINTER hprt)
{
    HAB 	hab;
    PPRINTER	pprt = hprt;

    MemAssert(pprt);
    MemAssert(pprt->pprtlist);
    hab = pprt->pprtlist->hab;

    // Free properties, if present

    if (pprt->pbDriverData != NULL) {
	MemFree(pprt->pbDriverData);
	pprt->pbDriverData = NULL;
	pprt->cbDriverData = 0;
    }

    // Get job properties

    GetDriverData(pprt);
}


/***	PrintMatchPrinter - Select printer that matches job properties
 *
 *	Entry
 *	    hprtlist - printer list
 *	    cbData   - size of pb buffer in bytes
 *	    pbData   - buffer with job properties
 *
 *	Exit-Success
 *	    Returns valid HPRINTER that matches Job Properties
 *	    Supplied job properties are set for printer.
 *
 *	Exit-Failure
 *	    Returns NULL, no matching printer
 *		Caller should use default printer (PrintQueryDefaultPrinter)
 */
HPRINTER PrintMatchPrinter(HPRINTERLIST hprtlist,USHORT cbData,BYTE *pbData)
{
    ULONG	    cb;
    HAB 	    hab;
    BYTE *	    pb; 		// Driver data
    PJOBPROP	    pjp;
    PPRINTER	    pprt;
    PPRINTER	    pprtMatch;
    PPRINTERLIST    pprtlist = hprtlist;
    char *	    pszDriver;		// Driver name
    char *	    pszModel;		// Model name
    char *	    pszName;		// Printer name

    MemAssert(pprtlist);
    hab = pprtlist->hab;

    // Buffer must be at least as large as header

    if (cbData < sizeof(JOBPROP))	// Too small
	return NULL;			//  Fail

    // Check internal length

    pjp = (PJOBPROP)pbData;		// Point at header
    if (pjp->cb != cbData)		// Internal length does not match
	return NULL;			//  Fail

    // Check internal counts

    cb = pjp->cbName+pjp->cbDriver+pjp->cbModel+pjp->cbData+sizeof(JOBPROP);
    if (cb > (ULONG)USHRT_MAX)		// Numbers too large
	return NULL;
    if ((USHORT)cb != cbData)		// Lengths do not match
	return NULL;

    // Set string/data pointers

    pszName = (BYTE *)pjp+sizeof(JOBPROP); // Name
    pszDriver = pszName + pjp->cbName;	   // Driver
    pszModel = pszDriver + pjp->cbDriver;  // Model
    pb = pszModel + pjp->cbModel;	   // Data

    // Try to find close match
    //
    // If we find a printer that matches Name, Driver, Model, and driver
    // data size, we have an exact match, and return that.
    //
    // If we find a printer that matches all but the Name, we return the
    // first such printer (after examining all printers to make sure there
    // is no exact match.
    //
    // Otherwise, we return NULL.

    pprtMatch = NULL;			// No match, yet
    for (pprt=pprtlist->pprtHead; pprt!=NULL; pprt=pprt->pprtNext) {
	if ( (stricmp(pprt->pszDriver,pszDriver) == 0) &&
	     (stricmp(pprt->pszModel,pszModel) == 0) ) { // Match Driver.Model

	    cb = DevPostDeviceModes(	// Get driver data size
		    hab,		// Anchor block
		    NULL,		// Buffer for data (NULL returns size)
		    pprt->pszDriver,	// Driver name
		    pprt->pszModel,	// Device name
		    NULL,		// Printer name
		    DPDM_QUERYJOBPROP	// Option
	    );

	    if (cb == pjp->cbData) {	// Match driver data size
		if (stricmp(pprt->pszName,pszName) == 0) { // Exact match?
		    pprtMatch = pprt;	// Exact match
		    break;		// Exit loop
		}
		else if (pprtMatch == NULL) // First close match
		    pprtMatch = pprt;	// Remember close match
	    }
	}
    }

    // Update job properties in printer

    if (pprtMatch != NULL) {		// Found a match
	if (pprt->pbDriverData == NULL) // No buffer, yet
	    pprt->pbDriverData = MemAlloc(pjp->cbData); // Alloc buffer

	if (pprt->pbDriverData != NULL) { // Have buffer
	    memcpy(pprt->pbDriverData,pb,pjp->cbData); // Save job properties
	    pprt->cbDriverData = pjp->cbData; // Save JP size
	}
    }
    return pprtMatch;			// Return best match
}


//************************************************************************
//
//  PRIVATE Functions
//
//************************************************************************


/***	MatchPrinter - Look for printer in list
 *
 *	Entry
 *	    pprtlist   - Printer list
 *	    pszQueue   - Queue name to match (may be NULL)
 *	    pszPrinter - Printer name to match (may be NULL)
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
PPRINTER MatchPrinter(PPRINTERLIST pprtlist,char *pszQueue,char *pszPrinter)
{
    PPRINTER	pprt;

    for (pprt=pprtlist->pprtHead; pprt!=NULL; pprt=pprt->pprtNext) {
	if ((pprt->lType == OD_QUEUED) && (pszQueue != NULL)) { // A queue
	    if (strcmp(pprt->pszLogAddr,pszQueue) == 0)
		return pprt;
	}
	else if (pszPrinter != NULL) {	// A direct printer
	    if (strcmp(pprt->pszPrinter,pszPrinter) == 0)
		return pprt;
	}
    }
    return NULL;
}


/***	AddPrinter - Add printer to printer list (if unique)
 *
 *	NOTE: If the printer is not added, it is freed!
 *
 *	Entry
 *	    pprtlist - printer list
 *	    pprt     - printer to be added
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL AddPrinter(PPRINTERLIST pprtlist,PPRINTER pprtNew)
{
    PPRINTER	pprt;

    // Avoid duplicate entry

    if (pprtNew->lType == OD_QUEUED) {	// Queue, check for duplicate
	pprt = MatchPrinter(pprtlist,pprtNew->pszLogAddr,NULL);
	if (pprt != NULL) {		// Found a match
	    pprt->fMultipleDrivers = TRUE; // Force query of driver/model
	    DestroyPrinter(pprtNew);	// No need for new printer
	    return TRUE;		// Do not add printer
	}
    }
    else {				// Direct printer
	// Do nothing, since Print Manager/Spooler never produce
	// duplicate printers.
	;				// Do nothing
    }

    // Link onto list

    pprtNew->pprtPrev = pprtlist->pprtTail; // Set previous printer
    if (pprtlist->pprtHead == NULL)	// List is empty
	pprtlist->pprtHead = pprtNew;	// Put at head of list
    else
	pprtlist->pprtTail->pprtNext = pprtNew; // Add at end of list
    pprtlist->pprtTail = pprtNew;	// New tail of list
    pprtNew->pprtNext = NULL;		// Set next printer

    pprtNew->pprtlist = pprtlist;	// Point at owning list
    return TRUE;
}


/***	SplitPrinter - Split printer if it contains multiple queue names
 *
 *	Entry
 *	    pprtlist - printer list
 *	    pprt     - printer to be split
 *
 *	Exit
 *	    Returns next PPRINTER
 */
PPRINTER SplitPrinter(PPRINTERLIST pprtlist,PPRINTER pprtOld)
{
    PPRINTER	pprt;
    PPRINTER	pprtNext;
    PPRINTER	pprtPrev;
    char *	psz;
    char *	psz1;
    USHORT	cprtAdded=0;

    if (pprtOld->lType != OD_QUEUED)	// If not a queue
	return pprtOld->pprtNext;	//  Next printer

    psz = strpbrk(pprtOld->pszLogAddr,","); // Find queue name separator
    if (psz == NULL)			// If no separator
	return pprtOld->pprtNext;	//  Next printer

    // Split up the printer

    strcpy(achPath,pprtOld->pszLogAddr); // Get working copy of queue name
    psz = achPath;
    while (psz != NULL) {
	psz1 = strpbrk(psz,",");	// Find separator
	if (psz1 != NULL) {		// Found one
	    *psz1 = '\0';		// Terminate this queue name
	    psz1++;			// Advance to next queue name
	}
	pprt = ClonePrinter(pprtOld,psz); // Clone with different pszLogAddr
	if (pprt == NULL)		// Clone failed
	    psz = NULL; 		// Exit loop
	else {				// Clone successful
	    AddPrinter(pprtlist,pprt);	// Add new printer
	    cprtAdded++;		// Printer (may have been) added
	    psz = psz1; 		// Advance to next queue (if any)
	    // Note: AddPrinter may not have actually added the printer,
	    //	     because it the split printer may have been a duplicate.
	    //	     That is okay, though, because we use cprtAdded only
	    //	     to know if we can delete pprtOld.
	}
    }

    // Delete old printer, if we were able to split it
    //
    // If we were not able to clone the printer at all, then we leave
    // it in the list, since the queue list was truncated to the first
    // queue name!
    //
    // The normal case, however, is that we did split it successfully,
    // so we want to delete the old printer.
    //
    // NOTE: If we were guaranteed that one of the AddPrinter calls actually
    //	     added a printer, we would *know* that pprtNext would never be
    //	     NULL.  However, it is possible that a split could result in
    //	     printers that are *all* already in the list.  Besides, the
    //	     code is more robust this way.

    pprtNext = pprtOld->pprtNext;	// Next printer
    if (cprtAdded != 0) {		// Have to delete old printer
	pprtPrev = pprtOld->pprtPrev;	// Previous printer
	if (pprtNext != NULL)		// There is a following printer
	    pprtNext->pprtPrev = pprtPrev; // Set back link
	if (pprtPrev != NULL)		// There is a preceding printer
	    pprtPrev->pprtNext = pprtNext; // Set forward link
	DestroyPrinter(pprtOld);	// No need for this anymore
    }

    return pprtNext;			// Next printer
}


/***	ClonePrinter - Create a duplicate PRINTER with a different pszLogAddr
 *
 *	Entry
 *	    pprt       - printer to clone
 *	    pszLogAddr - logical address to set (QUEUE1)
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER ClonePrinter(PPRINTER pprtOriginal,char *pszLogAddr)
{
    PPRINTER	pprt;

    pprt = CreatePrinter(
		pprtOriginal->lType,		// Copy type
		pszLogAddr,			// New logical address
		pprtOriginal->pszPrinter,	// Copy printer
		pprtOriginal->pszDriver,	// Copy driver
		pprtOriginal->pszModel, 	// Copy model
		pprtOriginal->fMultipleDrivers	// Copy flag
	   );
    return pprt;
}


/***	GetDriverModel - Get Driver and Model for a printer
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    If printer was a queue, and the existing printer/model may
 *	    not be the one the user setup, get it from the HINI_SYSTEM.
 *
 *	Uses
 *	    achPath
 *
 *	NOTE 1:
 *	    We *need* to query the PM_SPOOLER_QUEUE_DD section *only* if the
 *	    queue might have more than one driver associated with it.  This
 *	    could happen in several cases:
 *
 *		1)  If the queue is associated with a single printer, but
 *			that printer has more than one driver/model pair.
 *
 *		2)  If the queue is associated with more than one printer,
 *			and the set of driver/model pairs supported by
 *			these printers has more than one member.
 *
 *	    (1) Is easy to compute (we keep a flag when we build the PRINTER
 *		object).
 *
 *	    (2) Is more complicated, so we compute a cheaper version:
 *
 *		2a) If the queue is associated with more than one printer.
 *
 *		We compute this in AddPrinter.	If AddPrinter tries to add
 *		a PRINTER that has a pszLogAddr that matches one already
 *		added, then it knows we have a queue pointing to more than
 *		one printer.  So, it sets the fMultipleDrivers flag, and then
 *		the code below can use that single flag to determine if it
 *		has to get the driver/model from HINI_SYSTEM).
 *
 *	NOTE 2:
 *	     For an OD_DIRECT printer (no queue) there is no way to figure
 *	     out which driver/model pair to use, so we take the first one
 *	     in the list!  Since that is what we did when we built the
 *	     printer object initialy, we have nothing to do.
 */
VOID GetDriverModel(PPRINTER pprt)
{
    LONG    cb;
    char *  psz;
    char *  pszDriver;
    char *  pszModel;


    if (pprt->lType != OD_QUEUED)	// Not a queue
	return; 			//  Driver.model are correct

    if (!pprt->fMultipleDrivers)	// Only one driver
	return; 			//  No change needed

    // We have a queue with multiple driver.model choices

    pprt->fMultipleDrivers = FALSE;	// Only do this computation once

    // Get driver.model for this queue

    cb = PrfQueryProfileString(
	    HINI_SYSTEM,		// system ini
	    "PM_SPOOLER_QUEUE_DD",	// app section
	    pprt->pszName,		// printer name
	    "", 			// Default value
	    achPath,			// Buffer to receive value
	    (ULONG)sizeof(achPath)	// Buffer size
    );
    if (cb <= 1)			// No driver.model
	return; 			// Use existing one

    // Parse driver.model string

    psz = achPath;
    // A: "IBM4201;"
    //	   ^
    // B: "LASERJET.HP LaserJet IID;"
    //	   ^

    // Parse the driver and model names

    pszDriver = psz;			// Set driver name
    psz = strpbrk(psz,".;");		// Find separator
    if (psz == NULL)			// Bad format
	return; 			//  Exit without changing
    if (*psz == '.') {			// Found model name separator
	*psz = '\0';			// Null terminate driver name
	pszModel = psz + 1;		// Set model name
	psz = strpbrk(pszModel,";");	// Find end of model name
	if (psz == NULL)		// Bad format
	    return;			//  Exit without changing
    }
    else				// No model name
	pszModel = pszDriver;		// Make model same as driver name
    *psz = '\0';			// Null terminate driver name

    // Create string copies

    pszDriver = MemStrDup(pszDriver);
    if (pszDriver == NULL)
	return;

    pszModel = MemStrDup(pszModel);
    if (pszModel == NULL) {
	MemFree(pszDriver);
	return;
    }

    // Update printer structure

    if (pprt->pszDriver != NULL)
	MemFree(pprt->pszDriver);
    if (pprt->pszModel != NULL)
	MemFree(pprt->pszModel);

    pprt->pszDriver = pszDriver;
    pprt->pszModel = pszModel;
}


/***	GetDescription - Get description from HINI_SYSTEM
 *
 *	Get printer description or queue description, as appropriate.
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    Set description from HINI_SYSTEM.
 *
 *	Uses
 *	    achPath
 */
VOID GetDescription(PPRINTER pprt)
{
    LONG    cb;
    char *  psz;
    char *  pszApp;

    if (pprt->lType == OD_QUEUED)    // Queue
	pszApp = "PM_SPOOLER_QUEUE_DESCR";
    else
	pszApp = "PM_SPOOLER_PRINTER_DESCR";

    cb = PrfQueryProfileString(
	    HINI_SYSTEM,		// System ini
	    pszApp,			// App
	    pprt->pszName,		// Queue/printer name
	    "", 			// Default value
	    achPath,			// Buffer to receive value
	    (ULONG)sizeof(achPath)	// Buffer size
    );
    if (cb <= 1)			// No description
	return; 			// Use existing one

    // Parse description

    psz = achPath;
    psz = strpbrk(psz,";");		// Find end of description
    if (psz == NULL)			// Bad format
	return; 			//  Exit without changing
    *psz = '\0';			// Null terminate desription

    // Duplicate description

    psz = MemStrDup(achPath);
    if (psz == NULL)
	return;

    if (pprt->pszDescription != NULL)
	MemFree(pprt->pszDescription);
    pprt->pszDescription = psz;
}


/***	QueryDefaultPrinterQueue - Get the default printer and queue
 *
 *	Entry
 *	    ppszQueue	- pointer to receive pointer to queue name
 *	    ppszPrinter - pointer to receive pointer to printer name
 *
 *	Exit
 *	    One or both of ppszQueue/ppszPrinter may be set to NULL!
 *	    Caller must call MemFree on ppszQueue/Printer!
 *
 *	Uses
 *	    achPath
 */
VOID QueryDefaultPrinterQueue(char **ppszQueue,char **ppszPrinter)
{
    ULONG   cb;

    // Get default printer and queue

    cb = PrfQueryProfileString(
	    HINI_USER,			// OS2.INI
	    "PM_SPOOLER",		// Application
	    "PRINTER",			// Variable
	    "", 			// Default value
	    achPath,			// Buffer to receive value
	    (ULONG)sizeof(achPath)	// Buffer size
	 );
    if (cb == 0)			// Couldn't get value
	*ppszPrinter = NULL;
    else {
	TrimTrailingSemicolon(achPath); // Get rid of trailing ";"
	*ppszPrinter = MemStrDup(achPath);
    }

    cb = PrfQueryProfileString(
	    HINI_USER,			// OS2.INI
	    "PM_SPOOLER",		// Application
	    "QUEUE",			// Variable
	    "", 			// Default value
	    achPath,			// Buffer to receive value
	    (ULONG)sizeof(achPath)	// Buffer size
	 );
    if (cb == 0)
	*ppszQueue = NULL;		// Couldn't get value
    else {
	TrimTrailingSemicolon(achPath); // Get rid of trailing ";"
	*ppszQueue = MemStrDup(achPath);
    }
}


/***	TrimTrailingSemicolon - truncate string at first semicolon
 *
 *	Entry
 *	    psz - string to truncate
 *	Exit
 *	    String truncated at first semicolon (if any)
 */
void TrimTrailingSemicolon(char *psz)
{
    while ((*psz != '\0') && (*psz != ';'))
	psz++;
    *psz = '\0';
}


/***	PrinterBeginSearch - Start enumeration of installed printers
 *
 *	Entry
 *	    ppprt - pointer to receive pointer to printer
 *
 *	Exit-Success
 *	    Returns search handle, ppprt filled in
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PSEARCHP PrinterBeginSearch(PPRINTER *ppprt)
{
    ULONG	cb;
    ULONG	cb1;
    BOOL	f;
    PSEARCHP	psrchp;

    // Get size of buffer for printer names

    f = PrfQueryProfileSize(
	    HINI_SYSTEM,		// Only look in system profile
	    "PM_SPOOLER_PRINTER",	// Pointer to application name
	    NULL,			// Pointer to keyname
	    &cb 			// Pointer to return buffer size
	    );
    if (!f)
	return NULL;			// Failed

    // Create search structure

    psrchp = MemAlloc(sizeof(SEARCHP));
    if (psrchp == NULL)
	return NULL;
    psrchp->pchNext = NULL;

    // Allocate buffer for names

    psrchp->pchName = MemAlloc((USHORT)cb);
    if (psrchp->pchName == NULL) {
	PrinterEndSearch(psrchp);	// Frees search structure
	return NULL;			// Failed
    }

    // Get all printer names

    cb1 = PrfQueryProfileString(
	    HINI_SYSTEM,		// Only look in system profile
	    "PM_SPOOLER_PRINTER",	// Application name
	    NULL,			// Keyname
	    "", 			// Default value
	    psrchp->pchName,		// Name list to be returned
	    cb				// Buffer size
	    );

    if (cb1 == 0) {			// No printers installed
	PrinterEndSearch(psrchp);
	return NULL;
    }

    // Get first printer

    psrchp->pchNext = psrchp->pchName;	// Start at first printer
    *ppprt = PrinterNextSearch(psrchp); // Get a printer
    if (*ppprt != NULL) 		// Found one
	return psrchp;			//  Return success
    else {
	PrinterEndSearch(psrchp);	// No printer found
	return NULL;			//  Return failure
    }
}


/***	PrinterNextSearch - Continue enumeration of installed printers
 *
 *	Entry
 *	    psrchp - search handle from PrinterBeginSearch
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER PrinterNextSearch(PSEARCHP psrchp)
{
    PPRINTER	pprt=NULL;

    // Scan printers until we find a valid one, or the list is exhausted
    while ((*psrchp->pchNext != '\0') && (pprt == NULL)) {
	pprt = GetPrinterData(psrchp->pchNext); // Get printer
	psrchp->pchNext += strlen(psrchp->pchNext)+1; // Next printer
    }
    return pprt;
}


/***	PrinterEndSearch - End enumeration of installed printer drivers
 *
 *	Entry
 *	    psearchp - search handle from PrinterBeginSearch
 *
 *	Exit-Success
 *	    Returns TRUE
 *
 *	Exit-Failure
 *	    Returns FALSE
 */
BOOL PrinterEndSearch(PSEARCHP psrchp)
{
    if (psrchp == NULL)
	return TRUE;

    if (psrchp->pchName != NULL)
	MemFree(psrchp->pchName);
    MemFree(psrchp);
    return TRUE;
}


/***	GetPrinterData - Get printer data
 *
 *	Entry
 *	    achPrinter - printer name
 *
 *	Exit-Success
 *	    returns PPRINTER
 *
 *	    NOTE 1: if lType == OD_QUEUED, then pprt->pszLogAddr is one or
 *		    more queue names (separated by ",").
 *
 *		    The caller should clone the PRINTER structure as many
 *		    times as necessary so that there is one for each queue.
 *
 *	    NOTE 2: pprt->pszDescription is not filled in.
 *
 *		    After the caller solves Note 1, the description should
 *		    be retrieved from OS2.INI.	Use PM_SPOOLER_QUEUE_DESCR
 *		    for a queue, and PM_SPOOLER_PRINTER_DESCR for a port
 *		    (both in HINI_SYSTEM)
 *
 *	    NOTE 3: pprt->pszDriver/pszModel may not be correct
 *
 *		    After the caller solves Note 1, for each queue the
 *		    driver/model should be retrieved from PM_SPOOLER_QUEUE_DD
 *		    (in HINI_SYSTEM).
 *
 *	Exit-Failure
 *	    Returns NULL
 *
 *	Uses
 *	    achPath
 */
PPRINTER GetPrinterData(char *pszPrinter)
{
    USHORT	cb;
    BOOL	fComma; 		// True if multiple drivers
    ULONG	lType;
    PPRINTER	pprt;
    char *	psz;
    char *	pszDriver;
    char *	pszLogAddr;
    char *	pszModel;
    char *	pszQueue;

    // Get the printer details
    cb = (USHORT)PrfQueryProfileString(
	    HINI_SYSTEM,		// Only look in system profile
	    "PM_SPOOLER_PRINTER",	// Application name
	    pszPrinter, 		// Keyname
	    "", 			// Default value
	    achPath,			// Buffer to receive value
	    (ULONG)sizeof(achPath)	// Buffer size
	 );

    if (cb == 0)
	return NULL;

    // Get the logical address name, driver name, model name, and queue name

    psz = achPath;

    // Parse off logical address (usually a port name)
    //
    // A: "LPT1;LASERJET.HP LaserJet IID;QUEUE1;;"
    //	   ^
    // B: "LPT1;LASERJET.HP LaserJet IID,PSCRIPT;QUEUE1,QUEUE2;;"
    //	   ^

    pszLogAddr = achPath;		// Set logical address
    psz = strchr(psz,';');		// Find end of log addr
    if (psz == NULL)			// Bad format
	return NULL;			//  FAIL

    *psz = '\0';			// Null terminate log addr
    psz++;				// Skip separator

    // Parse off driver name and model
    //
    // NOTE:  These are provisional.  For a print queue, the driver/model
    //	      have to be retrieved (by the caller) from PM_SPOOLER_QUEUE_DD
    //	      in HINI_SYSTEM.
    //
    // A: "LPT1_LASERJET.HP LaserJet IID;QUEUE1;;"
    //		^
    // B: "LPT1_LASERJET.HP LaserJet IID,POSTSCRIPT;QUEUE1,QUEUE2;;"
    //		^

    pszDriver = psz;			// Set driver name
    psz = strpbrk(psz,".,;");		// Find separator
    if (psz == NULL)			// Bad format
	return NULL;			//  FAIL
    fComma = FALSE;			// Assume no comma
    if (*psz == '.') {			// Found model name separator
	*psz = '\0';			// Null terminate driver name
	pszModel = psz + 1;		// Set model name
	psz = strpbrk(pszModel,",;");	// Find end of model name
	if (psz == NULL)		// Bad format
	    return NULL;		//  FAIL
    }
    else if (*psz == ',' || *psz == ';') // No model name
	pszModel = pszDriver;		// Make model same as driver name

    // A: "LPT1_LASERJET_HP LaserJet IID;QUEUE1;;"
    //					^
    // B: "LPT1_LASERJET_HP LaserJet IID,POSTSCRIPT;QUEUE1,QUEUE2;;"
    //					^
    if (*psz == ',')			// More drivers, remember to skip them
	fComma = TRUE;
    *psz = '\0';			// Null terminate driver name
    psz++;				// Skip separator

    // A: "LPT1_LASERJET_HP LaserJet IID_QUEUE1;;"
    //					 ^
    // B: "LPT1_LASERJET_HP LaserJet IID_POSTSCRIPT;QUEUE1,QUEUE2;;"
    //					 ^
    if (fComma) {			// Need to skip rest of field
	psz = strpbrk(psz,";"); 	// Skip to end of driver field
	if (psz == NULL)		// Bad format
	    return NULL;		//  FAIL
	psz++;				// Skip separator
    }

    // A: "LPT1_LASERJET_HP LaserJet IID_QUEUE1;;"
    //						^
    // B: "LPT1_LASERJET_HP LaserJet IID_POSTSCRIPT;QUEUE1,QUEUE2;;"
    //						    ^
    // NOTE: We leave the list of queues, so that caller can enumerate
    //	     all queues!
    //
    if (*psz == ';')			// No queue
	pszQueue = NULL;
    else {				// Queue present
	pszQueue = psz; 		// Set queue name
	psz = strpbrk(psz,";"); 	// Find end of queue name(s)!
	if (psz == NULL)		// Bad format
	    return NULL;		//  FAIL
	*psz = '\0';			// Null terminate queue name
    }

    // Figure out whether this printer is queued or not

    if (pszQueue != NULL) {		// Printer is queued
	lType = OD_QUEUED;
	psz = pszQueue;
    }
    else {				// Printer is direct
	lType = OD_DIRECT;
	psz = pszLogAddr;
    }

    // Create printer structure

    pprt = CreatePrinter(
	    lType,			// Set type
	    psz,			// Set logical address
	    pszPrinter, 		// Set printer
	    pszDriver,			// Set driver
	    pszModel,			// Set model
	    fComma			// Set flag
       );
    return pprt;
}


/***	CreatePrinter - Create a PRINTER
 *
 *	Entry
 *	    lType      - printer type (OD_DIRECT or OD_QUEUED)
 *	    pszLogAddr - logical address (LPT1 or QUEUE1)
 *	    pszPrinter - printer name
 *	    pszDriver  - driver name
 *	    pszModel   - model name
 *
 *	Exit-Success
 *	    Returns PPRINTER
 *
 *	Exit-Failure
 *	    Returns NULL
 */
PPRINTER CreatePrinter(LONG lType,
		       char *pszLogAddr,
		       char *pszPrinter,
		       char *pszDriver,
		       char *pszModel,
		       BOOL fMultipleDrivers)
{
    PPRINTER	pprt;

    pprt = MemAlloc(sizeof(PRINTER));
    if (pprt == NULL)
	return NULL;

    // Create copies of strings to complete printer structure

    pprt->pszLogAddr = MemStrDup(pszLogAddr); // Duplicate logical address
    pprt->pszPrinter = MemStrDup(pszPrinter); // Duplicate printer name
    pprt->pszDriver  = MemStrDup(pszDriver);  // Duplicate driver
    pprt->pszModel   = MemStrDup(pszModel);  // Duplicate Model

    // Set other fields

    pprt->fMultipleDrivers = fMultipleDrivers; // Set flag
    pprt->lType = lType;		// Set type
    pprt->pszDescription = NULL;	// No description, yet
    pprt->cbDriverData = 0;		// No job properites
    pprt->pbDriverData = NULL;		// No job properites

    // Set consistent printer "name"
    if (lType == OD_QUEUED)
	pprt->pszName = pprt->pszLogAddr; // Use queue name
    else
	pprt->pszName = pprt->pszPrinter; // Use printer name

    // Make sure copies succeeded

    if ((pprt->pszLogAddr     == NULL) || // One or more dups failed
	(pprt->pszDriver      == NULL) ||
	(pprt->pszModel       == NULL) ||
	(pprt->pszPrinter     == NULL)) {
	DestroyPrinter(pprt);		// Destroy printer
	pprt = NULL;
    }
    return pprt;
}


/***	DestroyPrinter - Destroy a PRINTER
 *
 *	Entry
 *	    pprt - printer to destroy
 *
 *	Exit
 *	    NONE
 */
VOID DestroyPrinter(PPRINTER pprt)
{
    MemAssert(pprt);
    if (pprt->pszLogAddr != NULL)
	MemFree(pprt->pszLogAddr);
    if (pprt->pszDriver != NULL)
	MemFree(pprt->pszDriver);
    if (pprt->pszModel != NULL)
	MemFree(pprt->pszModel);
    if (pprt->pszPrinter != NULL)
	MemFree(pprt->pszPrinter);
    if (pprt->pszDescription != NULL)
	MemFree(pprt->pszDescription);
    if (pprt->pbDriverData != NULL)
	MemFree(pprt->pbDriverData);
    MemFree(pprt);
}


/***	GetDriverData - Get driver data for printer (if not already set)
 *
 *	This function attempts to set the Job Properties for a printer
 *	by querying the driver.  If successful, the Job Properties are
 *	stored in the printer.	Otherwise, they are left as NULL, and
 *	the driver will use its own default job properties.
 *
 *	Entry
 *	    pprt - printer
 *
 *	Exit
 *	    pprt->pbDriverData set to driver data, if at all possible.
 */
VOID GetDriverData(PPRINTER pprt)
{
    HAB     hab;
    LONG    cb;
    BYTE *  pb;
    LONG    rc;

    MemAssert(pprt);
    MemAssert(pprt->pprtlist);
    hab = pprt->pprtlist->hab;

    // See if job properties already set

    if (pprt->pbDriverData != NULL)	// They are
	return; 			//  Nothing to do

    // Assume no job properties

    pprt->cbDriverData = 0;
    pprt->pbDriverData = NULL;

    // Get size

    cb =  DevPostDeviceModes(
	    hab,			// Anchor block
	    NULL,			// Buffer for data (NULL returns size)
	    pprt->pszDriver,		// Driver name
	    pprt->pszModel,		// Device name
	    pprt->pszPrinter,		// Printer name
	    DPDM_QUERYJOBPROP		// Option
    );

    if (cb == DPDM_NONE)		// No job properties available
	return; 			//  Do nothing

    if (cb == DPDM_ERROR)		// Could not get job properties
	return; 			//  Do nothing

    pb = MemAlloc((USHORT)cb);		// Allocate buffer
    if (pb == NULL)			// Could not allocate it
	return; 			//  Nothing to do

    // Made it this far, get driver data

    rc = DevPostDeviceModes(
	    hab,			// Anchor block
	    (VOID *)pb, 		// Buffer for data
	    pprt->pszDriver,		// Driver name
	    pprt->pszModel,		// Device name
	    pprt->pszPrinter,		// Printer name
	    DPDM_QUERYJOBPROP		// Option
    );

    if (rc == DEV_OK) {
	pprt->cbDriverData = (USHORT)cb;
	pprt->pbDriverData = pb;
    }
    return;
}
