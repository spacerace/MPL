/*************************************************************

 This module contains subroutines for nead.c that specifically
 deal with the manipulation of EAs.

 Procedures in this file:
   AddEA()             Handles the Add button press
   QueryEAs()          Reads in all the EAs associated with a file
   CheckEAIntegrity()  Checks an EA buffer to see if it is valid
   Free_FEAList()      Deallocates memory associated with EA list
   LookupEAType()      Gets an offset into table for an EA type
   DeleteCurEA()       Deletes the highlighted EA
   CurEAType()         Returns EA Type given a HoldFEA ptr
   GetUSHORT()         Returns nth USHORT in ->aValue
   WriteEAs()          Updates EAs state on the disk
   EditEAValues()      Handles editing a given EA (also m-m EAs)
   EAExists()          Determines if the given EA Name exists
   ChangeName()        Handles the change of an EA's name
   MultiTypeIndex()    Gets a specific field in a m-m structure
   EAValueString()     Returns a representational string for an EA
   MultiAdd()          Handles addition of a field for m-m

**************************************************************/

#include "nead.h"

/************ External GLOBALS *******************************/

extern CHAR szFileName[CCHMAXPATH];
extern CHAR szEAName[MAXEANAME+1];
extern USHORT usRetEAType;
extern BOOL FILE_ISOPEN;
extern BOOL FILE_CHANGED;
extern BOOL COMMAND_LINE_FILE;
extern HHEAP hhp;
extern CHAR *pAlloc,*szEditBuf,*szAscii,*szScratch;
extern HOLDFEA *pHoldFEA;
extern DELETELIST *pDelList;
extern EADATA ConvTable[EATABLESIZE];

/*************************************************************/


/*
 * Function name: AddEA()
 *
 * Parameters:  hwnd which is the current window handle.
 *
 * Returns: TRUE iff the EA is successfully added.
 *
 * Purpose: This routine handles the addition of a new EA to the linked list.
 *
 * Usage/Warnings:  Routine does NOT do full memory error trapping and the
 *                  insert message to the l-box is not error checked.
 *
 * Calls: EditEAValue()
 */

BOOL AddEA(HWND hwnd)
{
   HOLDFEA  *pFEA=pHoldFEA;  /* Points to the beginning of the EA list */
   HOLDFEA  *pNewFEA;        /* Used to temporarily hold the new EA    */
   PASSDATA PData;

   if(!FILE_ISOPEN)
      return(FALSE);

   if(!WinDlgBox(HWND_DESKTOP,         /* get new EA name and type */
                 hwnd,
                 AddEAProc,
                 (HMODULE)NULL,
                 IDD_ADDEA,
                 NULL))
      return(FALSE);                   /* they said cancel */

   GetMem(pNewFEA, sizeof(HOLDFEA));   /* Allocate space for new EA struct */

   pNewFEA->cbName = (CHAR) strlen(szEAName);  /* Fill in new structure */
   pNewFEA->cbValue= 0;
   pNewFEA->fEA    = 0; /* Need bit NOT set */

   GetMem(pNewFEA->szName,pNewFEA->cbName+1);  /* Name returned in szEAName */
   strcpy(pNewFEA->szName,strupr(szEAName));
   pNewFEA->aValue = NULL;
   pNewFEA->next   = NULL;

   if(pHoldFEA == NULL)           /* It's the first EA for the file */
   {
      pHoldFEA = pNewFEA;
   }
   else                           /* Add EA to the end of the linked list */
   {
      while(pFEA->next)
         pFEA = pFEA->next;
      pFEA->next = pNewFEA;
   }
   PData.Point         = (CHAR *) pNewFEA;  /* Setup user data for call */
   PData.cbMulti       = 0;                 /* to edit of the name and  */
   PData.usMultiOffset = 0;                 /* EA Value                 */

   if(!EditEAValue(hwnd,&PData)) /* They canceled the edit */
   {
      if(pFEA)               /* It's not the only EA          */
         pFEA->next = NULL;  /* Disconnect the partial new EA */
      else
         pHoldFEA = NULL;    /* The new EA was the first one  */

      FreeMem(pNewFEA->szName,pNewFEA->cbName+1);
      FreeMem(pNewFEA,sizeof(HOLDFEA));

      return(FALSE);
   }

   WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_INSERTITEM, /* Insert name in L-Box */
                     MPFROM2SHORT(LIT_END,0),
                     MPFROMP(pNewFEA->szName));

   return(TRUE);
}


/*
 * Function name: QueryEAs()
 *
 * Parameters:  hwnd which is the current window handle.
 *              pszPath points to the path of the file to grab EAs from.
 *
 * Returns: TRUE iff it successfully reads in the EAs.  Upon exit, global
 *          pHoldFEA points to a linked list of the EAs for the current file.
 *
 * Purpose: Call DOS to Query a file's EA names and values.
 *
 * Usage/Warnings:  Routine does NOT do full memory error trapping.
 *                  NOTE:  This routine does NOT prevent other processes
 *                  from accessing the file's EAs while it is reading them
 *                  in, or while the program is editing them.
 *
 * Calls: Free_FEAList(), CheckEAIntegrity()
 */

BOOL QueryEAs(HWND hwnd,CHAR *pszPath)
{
   CHAR *pAlloc;       /* Holds the FEA struct returned by DosEnumAttribute */
                       /* also used to create the GEALIST for DosQPathInfo  */
   CHAR *pBigAlloc;    /* Temp buffer to hold each EA as it is read in      */
   USHORT cbBigAlloc;  /* Size of buffer                                    */

   ULONG ulEntryNum = 1; /* count of current EA to read (1-relative)        */
   ULONG ulEnumCnt;      /* Number of EAs for Enum to return, always 1      */

   HOLDFEA *pLastIn;     /* Points to last EA added, so new EA can link     */
   HOLDFEA *pNewFEA;     /* Struct to build the new EA in                   */

   FEA *pFEA;            /* Used to read from Enum's return buffer          */
   GEALIST *pGEAList;    /* Ptr used to set up buffer for DosQPathInfo call */
   EAOP eaopGet;         /* Used to call DosQPathInfo                       */

   GetMem((HHEAP) pAlloc,(USHORT) MAX_GEA);     /* Allocate enough room for any GEA List     */
   pFEA = (FEA *) pAlloc;      /* pFEA always uses pAlloc buffer            */

   pHoldFEA = NULL;            /* Reset the pointer for the EA linked list  */

   while(TRUE) /* Loop continues until there are no more EAs */
   {
      ulEnumCnt = 1;                 /* Only want to get one EA at a time */
      if(DosEnumAttribute(Ref_ASCIIZ,          /* Read into pAlloc Buffer */
                          pszPath,             /* Note that this does not */
                          ulEntryNum,          /* get the aValue field,   */
                          pAlloc,              /* so DosQPathInfo must be */
                          MAX_GEA,             /* called to get it.       */
                          &ulEnumCnt,
                          (LONG) GetInfoLevel1,
                          0L))
        break;                         /* There was some sort of error    */

      if(ulEnumCnt != 1)               /* All the EAs have been read      */
         break;

      ulEntryNum++;

      GetMem(pNewFEA,sizeof(HOLDFEA));

      if (pNewFEA == NULL)             /* Out of memory */
      {
         FreeMem(pAlloc,MAX_GEA);
         Free_FEAList(pHoldFEA,pDelList);
         return (FALSE);
      }

      pNewFEA->cbName = pFEA->cbName;  /* Fill in the HoldFEA structure   */
      pNewFEA->cbValue= pFEA->cbValue;
      pNewFEA->fEA    = pFEA->fEA;
      pNewFEA->next = NULL;

      GetMem(pNewFEA->szName,pFEA->cbName +1); /* Allocate for 2 arrays   */
      GetMem(pNewFEA->aValue,pFEA->cbValue);

      if (!pNewFEA->szName || !pNewFEA->aValue) /* Out of memory */
      {
         if(pNewFEA->szName)
            FreeMem(pNewFEA->szName,pFEA->cbName+1);
         if(pNewFEA->aValue)
            FreeMem(pNewFEA->aValue,pFEA->cbValue);

         FreeMem(pAlloc, MAX_GEA);
         FreeMem(pNewFEA,sizeof(HOLDFEA));

         Free_FEAList(pHoldFEA,pDelList);
         return (FALSE);
      }
      strcpy(pNewFEA->szName,pAlloc+sizeof(FEA));      /* Copy in EA Name */

      cbBigAlloc = sizeof(FEALIST) + pNewFEA->cbName+1 + pNewFEA->cbValue;
      GetMem(pBigAlloc,cbBigAlloc);
      if (pBigAlloc == NULL)
      {
         FreeMem(pNewFEA->szName,pFEA->cbName+1);
         FreeMem(pNewFEA->aValue,pFEA->cbValue);
         FreeMem(pAlloc, MAX_GEA);
         FreeMem(pNewFEA,sizeof(HOLDFEA));
         Free_FEAList(pHoldFEA,pDelList);
         return (FALSE);
      }

      pGEAList = (GEALIST *) pAlloc;   /* Set up GEAList structure */

      pGEAList->cbList = sizeof(GEALIST) + pNewFEA->cbName; /* +1 for NULL */
      pGEAList->list[0].cbName = pNewFEA->cbName;
      strcpy(pGEAList->list[0].szName,pNewFEA->szName);

      eaopGet.fpGEAList = (GEALIST far *) pAlloc;
      eaopGet.fpFEAList = (FEALIST far *) pBigAlloc;

      eaopGet.fpFEAList->cbList = cbBigAlloc;

      DosQPathInfo(pszPath,            /* Get the complete EA info        */
                   GetInfoLevel3,
                   (PVOID) &eaopGet,
                   sizeof(EAOP),
                   0L);
      memcpy(pNewFEA->aValue,          /* Copy the value to HoldFEA       */
             pBigAlloc+sizeof(FEALIST)+pNewFEA->cbName+1,
             pNewFEA->cbValue);

      FreeMem(pBigAlloc,cbBigAlloc);   /* Release the temp Enum buffer    */

      if(!CheckEAIntegrity(pNewFEA->aValue,pNewFEA->cbValue)) /* Bad EA   */
      {
         FreeMem(pNewFEA->szName,pFEA->cbName+1);
         FreeMem(pNewFEA->aValue,pFEA->cbValue);
         FreeMem(pNewFEA,sizeof(HOLDFEA));
         continue;                    /* Don't add this EA to linked list */
      }

      if(pHoldFEA == NULL)             /* If first EA, set pHoldFEA       */
         pHoldFEA = pNewFEA;
      else                             /* Otherwise, add to end of list   */
         pLastIn->next = pNewFEA;

      pLastIn = pNewFEA;               /* Update the end of the list      */
   }
   FreeMem(pAlloc, MAX_GEA);  /* Free up the GEA buf for DosEnum */
   return (TRUE);
}


/*
 * Function name: CheckEAIntegrity()
 *
 * Parameters:  aBuf points to the buffer to check for a valid EA.
 *              cbBuf is the allocated length of aBuf.
 *
 * Returns: TRUE iff the buffer is a valid EA structure.
 *
 * Purpose: This routine checks the integrity of the passed EA buffer by
 *          seeing if there are any non-standard EA types, or bad data that
 *          isn't sized correctly.
 *
 * Usage/Warnings:  Routine uses MultiTypeIndex() to check m-m type EAs
 *                  since they are potentially recursive.  However, this
 *                  may not be a good idea if the m-m EA is severly
 *                  corrupted because it may cause MTI() to attempt a read
 *                  from protected memory.  Routine does NOT modify the
 *                  buffer under any circumstance.
 *
 * Calls: MultiTypeIndex()
 */

BOOL CheckEAIntegrity(CHAR *aBuf,USHORT cbBuf)
{
   USHORT *pusPtr = (USHORT *) aBuf;
   USHORT usOffset;
   CHAR   *aEndPtr;

   usOffset = LookupEAType(*pusPtr);  /* Get the EA type */

   switch(ConvTable[usOffset].usFldType)
   {
      case IDD_LPDATA:
         pusPtr++;
         if(*pusPtr + 2*sizeof(USHORT) == cbBuf)
            return TRUE;
         else
            return FALSE;

      case IDD_MULTILIST:
         if(*pusPtr == EA_MVMT)
         {
            pusPtr += 2;

            /* This checks where the end of the m-m list ends to determine
               the size of the EA.  This is probably not good if the EA is
               badly corrupted and it points to protected memory */

            aEndPtr = MultiTypeIndex(aBuf,*pusPtr);
            if(aEndPtr - aBuf == (SHORT) cbBuf)
               return TRUE;
            else
               return FALSE;
         }
         else /* Single type, multi-value is not yet implemented */
         {
            return TRUE;
         }
      default:
         return FALSE;
   }
   return TRUE;
}


/*
 * Function name: Free_FEAList()
 *
 * Parameters:  pFEA points to the beginning of the linked list to be freed.
 *              pDList points to the beginning of the deleted linked list.
 *
 * Returns: VOID.  The two linked lists passed in are cleaned out though.
 *
 * Purpose: This routine frees up the current list of EAs by deallocating
 *          the space used by the szName and aValue fields, then deallocating
 *          the HoldFEA struct.  Next, it deletes the EAName space, then the
 *          DeleteList structure.
 *
 * Usage/Warnings:  Note that NEAD always passes in pHoldFEA and pDelList
 *                  which is unnecessary since they are global pointers;
 *                  however, this is done to make the routine more flexible
 *                  by allowing multiple linked lists to exist.
 *
 * Calls:
 */

VOID Free_FEAList(HOLDFEA *pFEA,DELETELIST *pDList)
{
   HOLDFEA *next;  /* Holds the next field since we free the structure */
                   /* before reading the current next field            */
   DELETELIST *Dnext; /* Same purpose as *next */

   while(pFEA)
   {
      next = pFEA->next;
      if(pFEA->szName)                         /* Free if non-NULL name  */
         FreeMem(pFEA->szName,pFEA->cbName+1);
      if(pFEA->aValue)                         /* Free if non-NULL value */
         FreeMem(pFEA->aValue,pFEA->cbValue);

      FreeMem(pFEA,sizeof(HOLDFEA));           /* Free HoldFEA struct    */
      pFEA = next;
   }

   while(pDList)
   {
      Dnext = pDList->next;
      if(pDList->EAName)
         FreeMem(pDList->EAName,strlen(pDList->EAName)+1);
      FreeMem(pDList,sizeof(DELETELIST));
      pDList = Dnext;
   }
}


/*
 * Function name: LookupEAType()
 *
 * Parameters:  usType is tye EA type to be looked up.
 *
 * Returns: An offset into the ConvTable to the appropriate entry.  If no
 *          match is found, the return value points to the last entry,
 *          non-conventional format.
 *
 * Purpose: This routine takes EA type and returns an offset into ConvTable
 *          which points to an entry that describes the type passed in.
 *
 * Usage/Warnings:
 *
 * Calls:
 */

USHORT LookupEAType(USHORT usType)
{
   USHORT cnt;

   for(cnt=0;cnt<EATABLESIZE-1;cnt++)
      if(ConvTable[cnt].usPrefix == usType)
         return(cnt);
   return(cnt);
}


/*
 * Function name: DeleteCurEA()
 *
 * Parameters:  hwnd is the current window handle.
 *
 * Returns: VOID.  Removes one item from global pHoldFEA list and adds one
 *          to global pDelList.
 *
 * Purpose: This routine removes in memory the currently highlighted EA from
 *          the EA list.  It places the deleted EA in the global pDelList
 *          linked list.
 *
 * Usage/Warnings:  The memory allocation routines are NOT fully error trapped.
 *
 * Calls: GetCurFEA()
 */

VOID DeleteCurEA(HWND hwnd)
{
   HOLDFEA *pFEA,*pFEAPrev;
   DELETELIST *pDL,*pDLcnt;  /* Utility ptrs for manipulating the Del list */
   LONG lOffset;

   pFEA = GetCurFEA(hwnd,pHoldFEA);    /* Gets a pointer to item to delete */
   if (pFEA == NULL)
      return;

   /* These two allocations should be checked for out of memory */
   GetMem(pDL,sizeof(DELETELIST));     /* Add Name to Delete List */
   GetMem(pDL->EAName,pFEA->cbName+1);
   strcpy(pDL->EAName,pFEA->szName);
   pDL->next = NULL;

   if(pDelList == NULL)        /* The del list was previously empty  */
      pDelList = pDL;
   else                        /* tack name onto the end of the list */
   {
      pDLcnt = pDelList;
      while(pDLcnt->next)
         pDLcnt = pDLcnt->next;
      pDLcnt->next = pDL;
   }

   lOffset = (LONG) WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                     LM_QUERYSELECTION,0,0);
   WinSendDlgItemMsg(hwnd, IDD_LBOX,
                     LM_DELETEITEM,MPFROMSHORT((SHORT) lOffset),0L);

   if(lOffset<1)               /* Remove pFEA from the linked list */
   {
      pHoldFEA = pFEA->next;
   }
   else
   {
      pFEAPrev = pHoldFEA;

      while(--lOffset)              /* Find previous EA */
         pFEAPrev = pFEAPrev->next;

      pFEAPrev->next = pFEA->next;
   }

   FreeMem(pFEA->szName,pFEA->cbName+1);  /* Release the memory */
   FreeMem(pFEA->aValue,pFEA->cbValue);
   FreeMem(pFEA,sizeof(HOLDFEA));

   FILE_CHANGED = TRUE;
}


/*
 * Function name: CurEAType()
 *
 * Parameters:  pFEA points to the FEA struct to use.
 *
 * Returns: The EA type.
 *
 * Purpose: Given an EA structure, this routine returns the Type of the EA
 *          which resides in the first USHORT of the aValue member.
 *          This function is the same as GetUSHORT(pFEA,0)
 *
 * Usage/Warnings:  Assumes a valid HoldFEA struct.
 *
 * Calls:
 */

USHORT CurEAType(HOLDFEA *pFEA)   /* Same as GetUSHORT(,0); */
{
   USHORT *pusType;    /* EA Type is stored in first USHORT of aValue field */

   pusType = (USHORT *) pFEA->aValue;
   return(*pusType);
}


/*
 * Function name: GetUSHORT()
 *
 * Parameters:  pFEA points to the FEA struct to use.
 *              index is an offset of the USHORT to be returned
 *
 * Returns: The appropriate USHORT from the aValue field
 *
 * Purpose: This routine returns the nth USHORT value in the aValue member
 *          of pFEA using index as the offset.
 *
 * Usage/Warnings:  Assumes a valid HoldFEA struct and that index doesn't
 *                  point outside the aValue buffer.
 *
 * Calls:
 */

USHORT GetUSHORT(HOLDFEA *pFEA,USHORT index)
{
   USHORT *pusType;

   pusType = (USHORT *) pFEA->aValue;
   while(index-- > 0)
      pusType++;
   return(*pusType);
}


/*
 * Function name: WriteEAs()
 *
 * Parameters:  hwnd is the current window handle used only by memory
 *              allocation error handling.
 *
 * Returns: VOID.  But cleans out the pDelList linked list.
 *
 * Purpose: This routine updates the EAs on disk to reflect their current
 *          condition in memory.  First, all EAs in the delete list are
 *          removed from the disk, then all EAs in the pHoldFEA list are
 *          written out to disk.
 *
 * Usage/Warnings:  NOTE:  This routine is not bulletproof as it does not get
 *                  exclusive access to the file EAs, nor does it handle out
 *                  of disk space sort of errors. Also, memory fetches are
 *                  not fully error trapped.
 *
 * Calls:
 */

VOID WriteEAs(HWND hwnd)
{
   DELETELIST *pDL = pDelList,*pDLnext;
   HOLDFEA    *pHFEA= pHoldFEA;
   EAOP       eaopWrite;
   CHAR       aBuf[MAX_GEA],*aPtr;
   FEA        *pFEA = (FEA *) &aBuf[sizeof(ULONG)];
   USHORT     usRet,usMemNeeded;
   ULONG      *pulPtr=(ULONG *) aBuf; /* Initally points to top of FEALIST */

   if(!FILE_ISOPEN || !FILE_CHANGED) /* Don't write unless it's necessary */
      return;

   eaopWrite.fpFEAList = (FEALIST far *) aBuf; /* Setup fields that won't */
   pFEA->fEA     = 0;                          /* change for the delete   */
   pFEA->cbValue = 0;                          /* calls to DosSetPathInfo */

   while(pDL)                       /* Clean out all the deleted EA names */
   {
      pFEA->cbName = (UCHAR) strlen(pDL->EAName);
      *pulPtr      = sizeof(FEALIST) + pFEA->cbName+1; /* +1 for NULL */
      strcpy(aBuf+sizeof(FEALIST),pDL->EAName);

      usRet=DosSetPathInfo(szFileName,    /* Delete EA's by saying cbValue=0 */
                           SetInfoLevel2,
                           (PVOID) &eaopWrite,
                           (USHORT) sizeof(EAOP),
                           DSPI_WRTTHRU,
                           0L);

      pDLnext = pDL->next;                   /* Temp hold next pDL         */
      FreeMem(pDL->EAName, pFEA->cbName+1);  /* Free up current Del struct */
      FreeMem(pDL, sizeof(DELETELIST));
      pDL = pDLnext;                         /* Set pDL to saved value     */
   }
   pDelList = NULL;                          /* DelList is now empty       */

   while(pHFEA)   /* Go through each HoldFEA */
   {
      usMemNeeded = sizeof(FEALIST) + pHFEA->cbName+1 + pHFEA->cbValue;
      GetMem(aPtr,usMemNeeded);

      eaopWrite.fpFEAList = (FEALIST far *) aPtr; /* Fill in eaop struct */
      eaopWrite.fpFEAList->cbList = usMemNeeded;

      eaopWrite.fpFEAList->list[0].fEA     = pHFEA->fEA;
      eaopWrite.fpFEAList->list[0].cbName  = pHFEA->cbName;
      eaopWrite.fpFEAList->list[0].cbValue = pHFEA->cbValue;

      strcpy(aPtr + sizeof(FEALIST),  pHFEA->szName);
      memcpy(aPtr + sizeof(FEALIST) + pHFEA->cbName+1,
             pHFEA->aValue, pHFEA->cbValue);

      usRet=DosSetPathInfo(szFileName,             /* Write out the EA */
                           SetInfoLevel2,
                           (PVOID) &eaopWrite,
                           (USHORT) sizeof(EAOP),
                           DSPI_WRTTHRU,0L);

      FreeMem(aPtr,usMemNeeded);       /* Free up the FEALIST struct */

      pHFEA = pHFEA->next;
   }

   FILE_CHANGED = FALSE;
}


/*
 * Function name: EditEAValue()
 *
 * Parameters:  hwnd is the current window handle.
 *              pPDat is a pointer to PassData which contains Edit EA info.
 *
 * Returns: TRUE iff the edit was successful.
 *
 * Purpose: This routine allows the entry/edit of an EA value.
 *          condition in memory.  First, all EAs in the delete list are
 *          removed from the disk, then all EAs in the pHoldFEA list are
 *          written out to disk.
 *
 * Usage/Warnings:  Expects the PassData structure to tell it the HoldFEA
 *                  to edit, and if it is a subfield of a multi-multi EA,
 *                  the rest of the PassData structure will be filled in
 *                  to indicated which m-m is being edited.  Note that if
 *                  this is a new edit, usRetEAType is expected to be set
 *                  to the proper EA type upon entry.  NOTE:  memory sizing
 *                  requests are not fully error trapped.
 *
 * Calls: MultiTypeIndex(), ChangeName(), AsciiEditProc() (thru PM),
 *        MultiTypeProc() (thru PM).
 */

BOOL EditEAValue(HWND hwnd, PASSDATA *pPDat)
{
   USHORT usEAType; /* Holds the field type to be edited */
   USHORT *pusPtr;
   USHORT usSize;   /* Holds the delta difference of the old and new buffers */
   CHAR   *szNew,*szTrash;     /* Temporary pointers */
   PASSDATA PDat;
   HOLDFEA *pFEA = (HOLDFEA *) pPDat->Point;   /* The EA to be edited */

   /* Determine the type of EA that will be edited */
   if(pPDat->cbMulti)                  /* It's a multi-type job  */
   {
      pusPtr = (USHORT *) MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset,
                                         pPDat->usIndex);
      usEAType = *pusPtr;
   }
   else if(pFEA->cbValue) /* It isn't a new EA name */
   {
      pusPtr   = (USHORT *) pFEA->aValue;
      usEAType = *pusPtr;
   }
   else    /* It's a new EA */
   {
      usEAType = ConvTable[usRetEAType].usPrefix;
   }

   PDat.Point   = pFEA->szName;        /* General setup for AsciiEditProc */
   PDat.usIndex = pPDat->cbMulti ? 1 : 0;   /* =1 if there is a multi  */
   PDat.fFlag   = (BYTE) ((pFEA->fEA & 0x80) ? TRUE : FALSE);

   switch(usEAType)
   {
      case EA_ASCIIZ:
      case EA_ASCIIZFN:
      case EA_ASCIIZEA:
      case EA_ASN1:
         if(pPDat->cbMulti)            /* It is a multi-type field */
            szAscii=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                   pPDat->usIndex)
                    +sizeof(USHORT);
         else if(pFEA->cbValue)        /* There is a current value */
            szAscii=pFEA->aValue+sizeof(USHORT);
         else                                     /* It's a new EA */
            szAscii=NULL;


         if(!WinDlgBox(HWND_DESKTOP,        /* Do an ascii text edit */
                       hwnd,
                       AsciiEditProc,
                       (HMODULE) NULL,
                       IDD_ASCIIEDIT,
                       &PDat))
            return(FALSE);                  /* They said cancel */

         if(PDat.fFlag)            /* Handle the need/nice bit */
            PDat.fFlag = 0x80;
         if(PDat.fFlag != (BYTE) (PDat.fFlag & 0x80))
            FILE_CHANGED = TRUE;
         pFEA->fEA = (BYTE) (pFEA->fEA & 0x7f) | PDat.fFlag;

         if(stricmp(strupr(szEAName),pFEA->szName)) /* The name changed */
            ChangeName(hwnd,pFEA,szEAName);

         if(pFEA->cbValue) /* There is a current value */
         {
            if(!strcmp(szAscii,szScratch))  /* It hasn't changed */
               return(TRUE);

            if(pPDat->cbMulti) /* Do the whole thing here if m-m */
            {
               usSize = strlen(szScratch)-strlen(szAscii); /* Change in size */

               if(usSize > 0) /* The new string is longer */
               {
                  ResizeMem(pFEA->aValue,          /* Enlarge the EA size */
                            pFEA->cbValue,
                            pFEA->cbValue+usSize);
                  szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                         pPDat->usIndex+1);
                  memmove(szTrash+usSize,  /* Move end of EA to make room */
                          szTrash,
                          pFEA->cbValue-(szTrash-pFEA->aValue));
               }
               else /* The new string is shorter */
               {
                  szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                         pPDat->usIndex+1);
                  memmove(szTrash+usSize, /* Move back the end of the EA */
                          szTrash,
                          pFEA->cbValue-(szTrash-pFEA->aValue));
                  ResizeMem(pFEA->aValue,  /* Shrink the EA buffer */
                            pFEA->cbValue,
                            pFEA->cbValue+usSize);
               }
               szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                      pPDat->usIndex);
               strcpy(szTrash+sizeof(USHORT),szScratch); /* Copy in new val */
               pFEA->cbValue+=usSize;              /* Change buffer count   */

               return(FILE_CHANGED = TRUE);  /* Done with m-m edit */
            }
            else
            {
               FreeMem(pFEA->aValue,pFEA->cbValue); /* Release old Value mem */
            }
         }
         GetMem(szNew,strlen(szScratch)+3);  /* +3 for Type & NULL */
         pusPtr = (USHORT *) szNew;
         *pusPtr= usEAType;                  /* Set type in new buffer       */
         strcpy(szNew+2,szScratch);          /* Copy in the new value        */
         pFEA->aValue = szNew;               /* Fix up the structure         */
         pFEA->cbValue= strlen(szScratch)+3;

         return(FILE_CHANGED = TRUE);

      case EA_LPBINARY:
      case EA_LPASCII:
      case EA_LPMETAFILE:
         if(pPDat->cbMulti)            /* It is a multi-type field */
         {  /* szTrash points to field to edit, pusPtr to the field length */
            szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                   pPDat->usIndex);
            pusPtr = (USHORT *) ((CHAR *) szTrash + sizeof(USHORT));
            usSize = *pusPtr;
            if(usSize)  /* It isn't a new EA */
            {
               GetMem(szAscii,usSize+1); /* Set up inital value for edit */
               memcpy(szAscii,szTrash+2*sizeof(USHORT),usSize);
               szAscii[usSize]=0;
            }
            else                       /* No inital value */
               szAscii = NULL;
         }
         else if(pFEA->cbValue)
         {
            usSize=GetUSHORT(pFEA,1);  /* Get size and set inital value */
            if(usSize)
            {
               GetMem(szTrash,usSize+1); /* +1 for null */
               memcpy(szTrash,pFEA->aValue+4,usSize);
               szTrash[usSize]=0;
               szAscii=szTrash;
            }
            else
               szAscii = NULL;
         }
         else
            szAscii = NULL;

         if(!WinDlgBox(HWND_DESKTOP,        /* Do an ascii text edit */
                       hwnd,
                       AsciiEditProc,
                       (HMODULE) NULL,
                       IDD_ASCIIEDIT,
                       &PDat))
         {  /* Cancel, but check if memory needs to be freed before exit */
            if(pPDat->cbMulti || pFEA->cbValue)
               if(szAscii) /* It's not NULL */
                  FreeMem(szAscii,strlen(szAscii)+1); /* +1 for NULL */

            return(FALSE);
         }

         if(PDat.fFlag)              /* Handle the need/nice bit */
            PDat.fFlag = 0x80;
         if(PDat.fFlag != (BYTE) (PDat.fFlag & 0x80))
            FILE_CHANGED = TRUE;
         pFEA->fEA = (BYTE) (pFEA->fEA & 0x7f) | PDat.fFlag;

         if(stricmp(strupr(szEAName),pFEA->szName)) /* The name changed */
            ChangeName(hwnd,pFEA,szEAName);

         if(pFEA->cbValue) /* There is a current value */
         {
            if(!strcmp(szAscii,szScratch))  /* It hasn't changed */
            {
               if(szAscii)
                  FreeMem(szAscii,usSize+1);
               return(TRUE);
            }
            if(szAscii)                  /* Free default value buffer */
               FreeMem(szAscii,usSize+1);

            if(pPDat->cbMulti)   /* Do the whole thing here is multi-type */
            {
               USHORT usDelta = strlen(szScratch) - usSize; /* Change in len */
               if(usDelta > 0) /* The new string is longer, resize first */
               {
                  ResizeMem(pFEA->aValue,pFEA->cbValue,pFEA->cbValue+usDelta);
                  szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                         pPDat->usIndex+1);
                  memmove(szTrash+usDelta,szTrash,
                          pFEA->cbValue-(szTrash-pFEA->aValue));
               }
               else  /* move first, resize afterwards */
               {
                  szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                         pPDat->usIndex+1);
                  memmove(szTrash+usDelta,szTrash,
                          pFEA->cbValue-(szTrash-pFEA->aValue));
                  ResizeMem(pFEA->aValue,pFEA->cbValue,pFEA->cbValue+usDelta);
               }
               szTrash=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                      pPDat->usIndex);
               memmove(szTrash+2*sizeof(USHORT),szScratch,strlen(szScratch));
               pusPtr = (USHORT *) ((CHAR *) szTrash + sizeof(USHORT));
               *pusPtr= strlen(szScratch);   /* Set the length field */


               pFEA->cbValue += usDelta;     /* Adjust struct len field */

              return(FILE_CHANGED = TRUE);
            }

            FreeMem(pFEA->aValue,pFEA->cbValue); /* Free up old value */
         }
         GetMem(szNew,strlen(szScratch)+4);    /* Get space for new value */
         pusPtr = (USHORT *) szNew;
         *pusPtr= usEAType;                    /* Set type field */
         pusPtr++;
         *pusPtr= strlen(szScratch);           /* Set length field */
         memcpy(szNew+4,szScratch,*pusPtr);    /* Copy in new value */
         pFEA->aValue = szNew;                 /* Adjust pointers */
         pFEA->cbValue= strlen(szScratch)+4;   /* +4 for type and LP cnt */

         return(FILE_CHANGED = TRUE);

      case EA_MVMT:                    /* It's multi-value multi-type */
         if(pFEA->cbValue == 0) /* It's a new EA */
         {
            GetMem(pFEA->aValue,3*sizeof(USHORT)); /* Allocate empty m-m EA */
            pFEA->cbValue = 3*sizeof(USHORT);
            pusPtr      = (USHORT *) pFEA->aValue;
            *pusPtr = 0xffdf;                 /* Multi-value, multi-type */
            pusPtr+=2;                        /* Skip type, codepage */
            *pusPtr = 0;                      /* No fields initially     */
            FILE_CHANGED = TRUE;
         }

         /* Set up passed in data */
         if(pPDat->cbMulti) /* It's a multi-type job  */
         {
            szNew   = MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                     pPDat->usIndex);
            szTrash = MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                     pPDat->usIndex+1);
            PDat.usMultiOffset = szNew - pFEA->aValue;
            PDat.cbMulti       = szTrash - szNew;
         }
         else
         {
            PDat.usMultiOffset = 0;
            PDat.cbMulti       = pFEA->cbValue;
         }
         PDat.Point         = (CHAR *) pFEA;

         WinDlgBox(HWND_DESKTOP,           /* Do the Multi-type edit */
                   hwnd,
                   MultiTypeProc,
                   (HMODULE) NULL,
                   IDD_MULTIBOX,
                   &PDat);
         return(TRUE);

   }
}


/*
 * Function name: EAExists()
 *
 * Parameters:  szEAName points to the EA Name to check for.
 *
 * Returns: TRUE iff an EA with a name matching szEAName exists.
 *
 * Purpose: This routine goes through the linked list pointed to by global
 *          pHoldFEA and determines whether or not an EA of the passed name
 *          already exists.
 *
 * Usage/Warnings:  The comparison is NOT case sensitive.
 *
 * Calls:
 *
 */

BOOL EAExists(CHAR *szEAName)
{
   HOLDFEA *phFEA=pHoldFEA;

   while(phFEA)
   {
      if(!stricmp(szEAName,phFEA->szName))
         return(TRUE);
      phFEA=phFEA->next;
   }
   return(FALSE);
}


/*
 * Function name: ChangeName()
 *
 * Parameters:  hwnd is the current window used for error messages.
 *              pFEA points to the current FEA.
 *              szName points to the new EA name.
 *
 * Returns: VOID.  Fixes up the pFEA structure and global pDelList.
 *
 * Purpose: This routine copies the current EA Name to the delete list, then
 *          allocates a new space, copies the new name into it, and sets the
 *          FEA pointer to it.
 *
 * Usage/Warnings:  NOTE:  Not all the memory allocations are fully error
 *                         trapped.
 *
 * Calls:
 *
 */

VOID ChangeName(HWND hwnd,HOLDFEA *pFEA,CHAR *szName)
{
   CHAR *szTemp;
   DELETELIST *pDL;

   GetMem(szTemp,strlen(szName+1));  /* Allocate space for new name */
   if(!szTemp)
      return;

   GetMem(pDL,(USHORT) sizeof(DELETELIST)); /* Allocate a new delete struct  */
   pDL->EAName = pFEA->szName;              /* Fill in DeleteList struct     */
   pDL->next   = pDelList;
   pDelList    = pDL;

   strcpy(szTemp,szName);                   /* Copy name to permanent buffer */
   pFEA->szName = szTemp;                   /* Fix up struct                 */
   pFEA->cbName = (CHAR) strlen(szName);

   FILE_CHANGED = TRUE;
}


/*
 * Function name: MultiTypeIndex()
 *
 * Parameters:  pMulti points to the current m-m field.
 *              usIndex is the field the caller is interested in.
 *
 * Returns: a pointer to the field specified by the usIndex param.
 *
 * Purpose: This routine takes a pointer to a Multi-Multi data field and
 *          returns a pointer to the nth data field in this buffer.
 *
 * Usage/Warnings:  NOTE:  Memory bounds are not checked and a corrupt
 *                         EA field could cause unspecified results.
 *                  Recursively calls itself to handle nesting. Does not
 *                  support multi-value single type fields.
 *
 * Calls: LookupEAType, MultiTypeIndex()
 *
 */

CHAR *MultiTypeIndex(CHAR *pMulti, USHORT usIndex)
{
   USHORT *pusPtr;
   USHORT usOffset;

   pMulti += 3*sizeof(USHORT);  /* skip over 0xffdf, codepage, and field cnt */

   while(usIndex--)             /* loop to skip over correct # of flds  */
   {
      pusPtr   = (USHORT *) pMulti;
      usOffset = LookupEAType(*pusPtr);    /* Get offset of field type */

      pMulti += sizeof(USHORT);            /* Skip over the type field */

      switch(ConvTable[usOffset].usFldType)
      {
         case IDD_ASCIIZ:
            while(*pMulti++);              /* Increment to point after NULL */
            break;

         case IDD_LPDATA:
            pusPtr = (USHORT *) pMulti;                /* Get the length */
            pMulti += *pusPtr + sizeof(USHORT);        /* skip to end */
            break;

         case IDD_MULTILIST:
            if(*pusPtr == EA_MVMT) /* m-m, do a recursive call to skip fld */
            {
               pusPtr = (USHORT *) pMulti; /* points to field cnt */
               pMulti = MultiTypeIndex(pMulti-sizeof(USHORT),*pusPtr);
               break;
            }
            /* Not yet implemented for Multi-valued single-type stuff... */
            break;
      }
   }
   return(pMulti);
}


/*
 * Function name: EAValueString()
 *
 * Parameters:  hwnd is the current window handle.
 *              aEAVal is a pointer to an EA value field.
 *
 * Returns: a pointer to an ASCII description of the field value.
 *
 * Purpose: This routine takes a pointer to an EA Value (i.e. starting with
 *          with $ffxx) and returns a pointer to a string representing the
 *          value of the EA.  This string must be Freed by the user when
 *          finished with it.
 *
 * Usage/Warnings:  NOTE:  Not all GetMem's are totally error trapped.
 *                  The string returned is allocated in this procedure,
 *                  but it is the caller's responsibility to free the buffer.
 *
 * Calls:
 *
 */

CHAR *EAValueString(HWND hwnd,CHAR *aEAVal)
{
   USHORT *pusPtr= (USHORT *) aEAVal;  /* Points to EA Type     */
   CHAR *szRet,*szTemp;        /* szRet points to return string */

   switch(*pusPtr)
   {
      case EA_ASCIIZ:    /* For asciiz strings, return MAXSHOWSIZE-1 chars */
      case EA_ASCIIZFN:
      case EA_ASCIIZEA:
      case EA_ASN1:
         aEAVal += sizeof(USHORT);
         if(strlen(aEAVal)<MAXSHOWSIZE)
         {
            GetMem(szRet,strlen(aEAVal)+1);
            strcpy(szRet,aEAVal);
         }
         else
         {
            GetMem(szRet,MAXSHOWSIZE);
            strncpy(szRet,aEAVal,MAXSHOWSIZE-4);
            strcpy (szRet+MAXSHOWSIZE-4,"...");
            szRet[MAXSHOWSIZE-1]=0;
         }
         return(szRet);

      case EA_LPASCII:   /* Display up to first MAXSHOWSIZE-1 chars */
      case EA_LPMETAFILE:
         pusPtr++;
         aEAVal += 2*sizeof(USHORT);
         if(*pusPtr < MAXSHOWSIZE)
         {
            GetMem(szRet,*pusPtr +1);
            strncpy(szRet,aEAVal,*pusPtr);
            szRet[*pusPtr]=0;
         }
         else
         {
            GetMem(szRet,MAXSHOWSIZE);
            strncpy(szRet,aEAVal,MAXSHOWSIZE-4);
            strcpy (szRet+MAXSHOWSIZE-4,"...");
            szRet[MAXSHOWSIZE-1]=0;
         }
         return(szRet);

      /* For the rest of the types, just display the field type */

      case EA_LPBINARY:
         szTemp = "*** LP Binary ***";
         break;

      case EA_LPBITMAP:
         szTemp = "*** LP Bitmap ***";
         break;

      case EA_LPICON:
         szTemp = "*** LP Icon ***";
         break;

      case EA_MVMT:
         szTemp = "*** Multi-value Multi-type ***";
         break;

      case EA_MVST:
         szTemp = "*** Multi-value Single-type ***";
         break;

      default:
         szTemp = "*** Unknown EA type ***";
         break;

   }
   GetMem(szRet,strlen(szTemp)+1); /* Copy string from static to dynamic */
   strcpy(szRet,szTemp);
   return(szRet);
}


/*
 * Function name: MultiAdd()
 *
 * Parameters:  hwnd is the current window handle.
 *              pFEA points to the current FEA.
 *              pPDat gives the current m-m data.
 *
 * Returns: VOID.  Modifies the current pFEA.
 *
 * Purpose: This routine is called by MultiTypeProc and handles the addition
 *          of a subvalue to a multi-value, multi-type EA.
 *
 * Usage/Warnings:  NOTE:  Not all GetMem's are totally error trapped.
 *                  It is also possible that the add to the listbox could fail.
 *
 * Calls: AddEAProc() (thru PM), MultiTypeIndex()
 *
 */

VOID MultiAdd(HWND hwnd, HOLDFEA *pFEA,PASSDATA FAR *pPDat)
{
   USHORT   usSize;
   USHORT   *pusPtr;
   CHAR     aUtility[6];      /* Used to hold the header for all EA types */
   CHAR     *pInsert,*pValue;
   PASSDATA PDat;

   PDat.Point = pFEA->szName;

   if(!WinDlgBox(HWND_DESKTOP,        /* Get the name and type */
                 hwnd,
                 AddEAProc,
                 (HMODULE) NULL,
                 IDD_ADDEA,
                 &PDat))
      return;                         /* They said cancel */

   pusPtr = (USHORT *) aUtility;
   *pusPtr= ConvTable[usRetEAType].usPrefix;   /* Set the type in header buf */

   switch(ConvTable[usRetEAType].usFldType)
   {
      case IDD_ASCIIZ:     /* make buffer look like: xx FF 00, size 3 */
         usSize = 3;
         aUtility[2]=0;
         break;

      case IDD_LPDATA:     /* make the buffer look like: xx FF 00 00, size 4 */
         usSize = 4;
         pusPtr = (USHORT *) &aUtility[2];
         *pusPtr= 0;
         break;

      case IDD_MULTILIST:
         usSize = 6;
         pusPtr = (USHORT *) &aUtility[2];
         *pusPtr= 0; /* Zero out codepage */
         pusPtr++;
         *pusPtr= 0; /* Zero out fld cnt */
         break;
   }
   /* Increase EA size to accomodate the header */
   ResizeMem(pFEA->aValue,pFEA->cbValue,pFEA->cbValue+usSize);

   pusPtr  = (USHORT *) ((CHAR *) pFEA->aValue + pPDat->usMultiOffset);
   pusPtr+=2;    /* Point to the current number of m-m fields */

   /* Get ptr to beginning of current EA, scoot the rest down and insert
      the 3-4 byte header at the end of the list.                        */
   pInsert = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset, *pusPtr);
   memmove(pInsert+usSize,pInsert, pFEA->cbValue-(pInsert-pFEA->aValue));
   memcpy(pInsert,aUtility,usSize);

   pFEA->cbValue += usSize;   /* Fix up the counts */
   pPDat->cbMulti+= usSize;
                                                  /* Set the PDat for call */
   PDat.Point         = (CHAR *) pFEA;
   PDat.cbMulti       = pPDat->cbMulti;
   PDat.usMultiOffset = pPDat->usMultiOffset;
   PDat.usIndex       = *pusPtr;

   if(!EditEAValue(hwnd,&PDat)) /* They canceled the edit */
   {  /* Move the EA's back to effectively kill the inserted header */
      memmove(pInsert,pInsert+usSize,pFEA->cbValue-(pInsert-pFEA->aValue));
      ResizeMem(pFEA->aValue,pFEA->cbValue,pFEA->cbValue-usSize);
      pFEA->cbValue -= usSize;   /* Adjust counters */
      pPDat->cbMulti-= usSize;

      return;
   }

   /* Reset pusPtr since EditEAValue could have moved the base address */
   pusPtr  = (USHORT *) ((CHAR *) pFEA->aValue + pPDat->usMultiOffset);
   pusPtr+=2;

   pInsert = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset, *pusPtr);

   *pusPtr += 1; /* Field cnt incremented AFTER call to Edit */

   pValue = EAValueString(hwnd,pInsert);   /* Add new field to the list box */

   WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_INSERTITEM,
                     MPFROM2SHORT(LIT_END,0),
                     MPFROMP(pValue));
   FreeMem(pValue,strlen(pValue)+1);

   FILE_CHANGED = TRUE;
}


