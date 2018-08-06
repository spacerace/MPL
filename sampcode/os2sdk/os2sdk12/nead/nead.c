/*************************************************************

 This program implements an extended attributes editor.
 Included in this file are all of the PM interface procedures.
 All of these procs represent procedures for dialog boxes.

 This program was written by Jeff Johnson, 7/89, with pieces
 of code pooled from already existing applications.

 This code is not fully implemented as it was written to show
 how to use EAs.  Specifically, the Copy, Copy all, and Paste
 buttons are not implemented, Icons and bitmaps are not
 supported, and Multi-value, single-type EAs are also not
 implemented.

 The EA code revolves around the HoldFEA structure.  This
 structure is a linked list which contains one EA per
 structure.  Each HoldFEA structure is dynamically allocated
 as are the two string pointer members of the structure,
 szName and aValue.  This is done to make handling the EAs
 easier and more flexable.

 Procedures in this file:
   main()          Sets up the PM environment and heap and
                   calls the main dialog procedure MainDlgProc
   MainDlgProc()   Handles the main window messages
   AddEAProc()     Handles new EA name entry and type selection
   AsciiEditProc() Handles editing of EA name/value
   IconDlgProc()   Unimplemented icon procedure handler
   MultiTypeProc() Handles the Multi-type dialog box
   ShowEAType()    Shows Selected EA Type for MainDlgProc
   EditEA()        Handles selection of an EA in the listbox
   GetCurFEA()     Gets a ptr to the hi-lited EA in the listbox

**************************************************************/

#include "nead.h"

/********************* GLOBALS *******************************/

CHAR szFileName[CCHMAXPATH];    /* Holds current EA file */
CHAR szEAName[MAXEANAME+1];     /* Used to return the EA name +1 for NULL */
USHORT usRetEAType;             /* Used to return selected EA Type */
BOOL FILE_ISOPEN = FALSE;       /* File Open flag */
BOOL FILE_CHANGED = FALSE;      /* File Changed flag */
BOOL COMMAND_LINE_FILE = FALSE; /* Flag to determine if a file was on the CL */
HHEAP hhp;                      /* Pointer to the heap */
CHAR *szAscii,*szScratch;       /* 2 Pointer used to return misc. strings */
HOLDFEA *pHoldFEA;              /* Global EA linked-list pointer          */
DELETELIST *pDelList;           /* Global ptr to l-l of deleted EAs       */
EADATA ConvTable[EATABLESIZE] = {    /* Setup the table */
   { EA_LPBINARY  ,"Length preceeded hex binary\0        ",IDD_LPDATA},
   { EA_LPASCII   ,"Length preceeded ascii\0             ",IDD_LPDATA},
   { EA_ASCIIZ    ,"Asciiz\0                             ",IDD_ASCIIZ},
   { EA_LPBITMAP  ,"Length preceeded bitmap\0            ",IDD_LPDATA},
   { EA_LPMETAFILE,"Metafile\0                           ",IDD_LPDATA},
   { EA_LPICON    ,"Length preceeded icon\0              ",IDD_LPDATA},
   { EA_ASCIIZFN  ,"Asciiz file name of associated data\0",IDD_ASCIIZ},
   { EA_ASCIIZEA  ,"Asciiz EA of associated data\0       ",IDD_ASCIIZ},
   { EA_MVMT      ,"Multi value multi type field\0       ",IDD_MULTILIST},
   { EA_MVST      ,"Multi value single type field\0      ",IDD_MULTILIST},
   { EA_ASN1      ,"ASN.1 field\0                        ",IDD_ASCIIZ},
   { 0            ,"Non conventional format\0            ",IDD_ASCIIZ}
   };

/*************************************************************/

/*
 * Function name: main()
 *
 * Parameters:  argc, argv.  If the user places a file name on the command
 *              line, its EAs will be retrieved by default, otherwise
 *              the user will be prompted to select a file.
 *
 * Returns: Always returns 0
 *
 * Purpose: Parses the command line, sets up the PM environment, creates
 *          a global memory heap, calls the main dialog proc, then
 *          cleans up and exits.
 *
 * Usage/Warnings:
 *
 * Calls: ParseFileName, MainDlgProc (thru PM)
 */

int main(int argc, char *argv[])
{
   HAB          hab;
   HMQ          hmq;

   if(argc > 1)  /* If a command line file, get EAs from it */
      if(ParseFileName(szFileName,argv[1]) != FILE_VALID) /* Bad file name */
         *szFileName=0;
      else
         COMMAND_LINE_FILE = TRUE;

   /* Note:  The following 3 setup calls aren't currently checked for errors */
   hab = WinInitialize(0);
   hmq = WinCreateMsgQueue(hab, 0);
   hhp = WinCreateHeap(0, 0, GROWSIZE, 0, 0,
                          HM_MOVEABLE | HM_VALIDSIZE | HM_VALIDFLAGS);

   WinDlgBox(HWND_DESKTOP, HWND_DESKTOP,
                         MainDlgProc,(HMODULE) NULL,IDD_MAIN,NULL);

   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
}


/*
 * Function name: MainDlgProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              No user data is expected in the WM_INITDLG.
 *
 * Returns: Always returns 0, Exits with WinDismissDlg set to TRUE
 *
 * Purpose: Handles all the messages associated with the main window
 *          and calls the appropriate handling procedures.  The initialize
 *          routine sets up the program icon and posts a load file message
 *          to itself.
 *
 * Usage/Warnings: Called only by main()
 *
 * Calls:  OpenFile(), AddEA(), DeleteCurEA, WriteEAs, ShowEAType, EditEA
 */

MRESULT EXPENTRY MainDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   HPOINTER hpointer;
   USHORT   usRet;

   switch(msg)
   {
      case WM_INITDLG:
         hpointer = WinLoadPointer(HWND_DESKTOP,            /* Set Nead Icon */
                                   (HMODULE) NULL,IDR_EAD);
         WinPostMsg(hwnd,WM_SETICON,hpointer,0L);

         /* Get EAs from file on Command line, or prompt for file name */
         WinPostMsg(hwnd,WM_USER,NULL,NULL);
         return 0L;

      case WM_USER:  /* Posted by WM_INITDLG allows main window to open 1st */
         OpenFile(hwnd, COMMAND_LINE_FILE ? ARGFILE : 0);
         return 0L;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case IDD_NEWFILE:     /* Select another file to view/edit EAs */
               OpenFile(hwnd,0);
               return 0L;

            case IDD_ADD:         /* Add an EA to current list in memory  */
               AddEA(hwnd);
               return 0L;

            case IDD_DELETE:      /* Delete an EA from memory             */
               DeleteCurEA(hwnd);
               return 0L;

            case IDD_WRITE:       /* Write EA's out to disk               */
               WriteEAs(hwnd);
               return 0L;

            case IDD_EDIT:        /* Edit the hilited EA                  */
               EditEA(hwnd);
               return 0L;

            case IDD_COPY:        /* Copy and Paste aren't                */
            case IDD_PASTE:       /* currently implemented                */
               return 0L;

            case IDD_QUIT:
               if(FILE_CHANGED)   /* Give user a chance to save changes   */
               {
                  usRet=WinMessageBox(HWND_DESKTOP,hwnd,
                  "The current file has been changed.  Do you \
wish to save the changes before proceeding?",
                  "Warning",0,MB_YESNOCANCEL | MB_ICONQUESTION);
                  switch(usRet)
                  {
                     case MBID_YES:    /* They want to save the EAs       */
                        WriteEAs(hwnd);
                        break;
                     case MBID_CANCEL: /* They don't really want to quit  */
                        return 0L;
                  }
               }
               WinDismissDlg(hwnd,TRUE);
               return 0L;
         }

      case WM_CONTROL:
         switch(SHORT1FROMMP(mp1))
         {
            case IDD_LBOX:
               switch(SHORT2FROMMP(mp1))
               {
                  case LN_SELECT:       /* A new item has been hi-lited */
                     ShowEAType(hwnd);  /* Display appropriate EA Type  */
                     return 0L;

                  case LN_ENTER:        /* Edit the selected EA         */
                     EditEA(hwnd);
                     return 0L;
               }
         }
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: AddEAProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              If user data is passed in, it indicates it is a m-m
 *              add and the EA Name is in ->Point and the user should
 *              not be allowed to edit it.
 *
 * Returns: Exits with WinDismissDlg set to TRUE if the user selects OK,
 *          FALSE if the user selects CANCEL.
 *
 * Purpose: Handles the screen that allows the user to enter a new EA Name
 *          and select the EA type.  It checks to make sure the name is
 *          unique and is not NULL before returning.  The user pointer is
 *          used to determine whether the name is allowed to change or not.
 *          If it points to a PassData structure, the Point member gives the
 *          static EA Name.
 *
 * Usage/Warnings: Called by both the add EA routine and the m-m EA add
 *                 routine.  This routine places the user typed name in
 *                 global szEAName, and the EA type in global usRetEAType.
 *
 * Calls:  EAExists()
 */

MRESULT EXPENTRY AddEAProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static CHAR bPDat = 0; /* flag indicating if the pPDat struct is valid */
   PASSDATA FAR *pPDat;   /* struct used to pass in default EA name       */

   switch(msg)
   {
      case WM_INITDLG:
         bPDat = 0;
         WinSendDlgItemMsg(hwnd, IDD_EANAME,EM_SETTEXTLIMIT,
                                 MPFROM2SHORT(MAXEANAME,0),NULL);
         WinSendDlgItemMsg(hwnd, EA_LPASCII,BM_CLICK,0L,0L);

         pPDat=PVOIDFROMMP(mp2);
         if(pPDat)
         {
            bPDat = 1;
            WinSetDlgItemText(hwnd, IDD_EANAME,pPDat->Point);
            WinSetDlgItemText(hwnd, IDD_TITLE,
                              "Add a Multi-type field to the EA");
            WinEnableWindow(WinWindowFromID(hwnd,IDD_EANAME),
                            FALSE);                /* Disable EA Name field */
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,DID_OK));

            return (MRESULT) TRUE;       /* We don't want default focus setting */
         }
         return 0L;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case DID_OK:
               if(bPDat == 0)          /* It is a new EA (not multi-type)   */
               {
                  WinQueryDlgItemText(hwnd, IDD_EANAME, MAXEANAME+1, szEAName);

                  if(!strlen(szEAName))    /* Don't allow a Null EA Name    */
                  {
                     WinAlarm(HWND_DESKTOP,WA_ERROR);
                     WinMessageBox(HWND_DESKTOP, hwnd,
                                   "Cannot create a NULL EA name.",
                                   NULL, 0, MB_OK);
                     return 0L;
                  }

                  if(EAExists(szEAName))   /* Don't allow duplicate EA Name */
                  {
                     WinAlarm(HWND_DESKTOP,WA_ERROR);
                     WinMessageBox(HWND_DESKTOP, hwnd,
                          "EA name already exists.  Can't create that name.",
                          NULL, 0, MB_OK);
                     return 0L;
                  }
               }
               usRetEAType = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, EA_ASCIIZ,
                                                   BM_QUERYCHECKINDEX, 0L, 0L));
               WinDismissDlg(hwnd, TRUE);
               return 0L;

            case DID_CANCEL:
               WinDismissDlg(hwnd, FALSE);
               return 0L;
         }
         break;
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: AddEAProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              The user pointer passes in a PassData struct.  The Point
 *              member points to the default EA Name and the usIndex
 *              member determines whether or not the user can modify the
 *              EA Name.  The fFlag member determines the status of the
 *              Need/nice bit.  TRUE indicates the Need bit is set.  The
 *              default Value is passed in global szAscii.
 *
 * Returns: Exits with WinDismissDlg set to TRUE if the user selects OK,
 *          FALSE if the user selects CANCEL.  Global szEAName contains the
 *          modified EA Value.  the fFlag member of the passed in data will
 *          be updated to reflect the current state of the Need bit.
 *
 * Purpose: This proc handles the editing of EA Names and their associated
 *          ASCII strings.
 *
 * Usage/Warnings: A PassData struct must be passed in when called.
 *                 Note that the Value buffer is dynamically allocated and
 *                 it is the responsibility of the calling procedure to
 *                 deallocate the buffer (szScratch) when finished with it.
 *                 Note also that memory allocations are NOT fully error
 *                 trapped.
 *
 * Calls:  EAExists()
 */


MRESULT EXPENTRY AsciiEditProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static PASSDATA FAR *pPDat;

   switch(msg)
   {
      case WM_INITDLG:
         WinSendDlgItemMsg(hwnd, IDD_EANAME,EM_SETTEXTLIMIT,
                                 MPFROM2SHORT(MAXEANAME,0),NULL);
         WinSendDlgItemMsg(hwnd, IDD_EAVALUE,EM_SETTEXTLIMIT,
                                 MPFROM2SHORT(MAXEAVALUE,0),NULL);
         pPDat=PVOIDFROMMP(mp2);

         WinSetDlgItemText(hwnd, IDD_EANAME,pPDat->Point);
         WinSetDlgItemText(hwnd, IDD_EATYPE,ConvTable[usRetEAType].szFormat);
         WinSendDlgItemMsg(hwnd, IDD_NEEDBIT, BM_SETCHECK,
                           MPFROM2SHORT(pPDat->fFlag,0),NULL);

         if(szAscii)   /* Set default EA Value if global var is != NULL */
            WinSetDlgItemText(hwnd, IDD_EAVALUE,szAscii);

         if(pPDat->usIndex) /* Disable EANAME if passed in a non-zero value */
         {
            WinEnableWindow(WinWindowFromID(hwnd,IDD_EANAME),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,IDD_NEEDBIT),FALSE);
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,IDD_EAVALUE));
            return (MRESULT) TRUE;       /* We don't want default focus setting */
         }
         return 0L;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case DID_OK:
               pPDat->fFlag = (BYTE) SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_NEEDBIT,
                                                       BM_QUERYCHECK,
                                                       0L, 0L) );
               WinQueryDlgItemText(hwnd, IDD_EANAME, MAXEANAME+1, szEAName);

               if(!strlen(szEAName))    /* They nulled out the name */
               {
                  WinAlarm(HWND_DESKTOP,WA_ERROR);
                  WinMessageBox(HWND_DESKTOP, hwnd,
                                "Cannot create a NULL EA name.",
                                NULL, 0, MB_OK);
                  return 0L;
               }

               GetMem(szScratch,MAXEAVALUE+1);  /* Allocate buffer for value */
               WinQueryDlgItemText(hwnd, IDD_EAVALUE, MAXEAVALUE+1, szScratch);

               if(!strlen(szScratch))    /* They nulled out the value */
               {
                  FreeMem(szScratch,MAXEAVALUE+1); /* Free the value buffer */
                  WinAlarm(HWND_DESKTOP,WA_ERROR);
                  WinMessageBox(HWND_DESKTOP, hwnd,
                                "An EA can't have a NULL value.",
                                NULL, 0, MB_OK);
                  return 0L;
               }

               if(stricmp(szEAName,pPDat->Point)) /* changed the EA name */
               {
                  if(EAExists(szEAName))  /* Can't have duplicate EA names */
                  {
                     WinAlarm(HWND_DESKTOP,WA_ERROR);
                     WinMessageBox(HWND_DESKTOP, hwnd,
                            "EA name already exists.  Can't create that name.",
                            NULL, 0, MB_OK);

                     FreeMem(szScratch,MAXEAVALUE+1);  /* Free value buffer */
                     return 0L;
                  }
               }

               ResizeMem(szScratch,MAXEAVALUE+1,/* Resize buf to actual size */
                         strlen(szScratch)+1);

               WinDismissDlg(hwnd, TRUE);
               return 0L;

            case DID_CANCEL:
               WinDismissDlg(hwnd, FALSE);
               return 0L;
         }
         break;
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: IconDlgProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              No user data is expected.
 *
 * Returns: Currently just calls WinDefDlgProc.
 *
 * Purpose: This proc is unimplemented, but is intedned to handle displaying
 *          and editing icons/bitmaps.
 *
 * Usage/Warnings:
 *
 * Calls:
 */

MRESULT EXPENTRY IconDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch(msg)
   {
      case WM_INITDLG:
         return 0L;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case DID_OK:
               WinDismissDlg(hwnd, TRUE);
               return 0L;
         }
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: MultiTypeProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              Expects a user pointer to a PassData structure with Point
 *              member pointing to the current pFEA structure.  The Multi
 *              member fields of the PassData struct should also be
 *              filled in to indicate the location and size of the current
 *              m-m field.
 *
 * Returns: Always returns TRUE thru WinDismissDlg when DONE is clicked.
 *
 * Purpose: This proc handles the multi-type EA stuff.  Allows the fields
 *          in a given m-m to be edited.
 *
 * Usage/Warnings:  NOTE that this procedure is fully reentrant/recursive
 *                  in that it calls EditEAValue() which can call
 *                  MultiTypeProc.  Since PassData information is placed
 *                  in static variables, the ReEnter structure is used to
 *                  hold a dynamic linked list of values passed into the
 *                  proc and thus keeps the static data set to the proper
 *                  values.
 *
 * Calls: MultiTypeIndex(), EAValueString(), EditEAValue(), MultiAdd()
 */

MRESULT EXPENTRY MultiTypeProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static HOLDFEA *pFEA;        /* Points to the current EA         */
   static PASSDATA FAR *pPDat;  /* Points to the passed Data struct */
   static REENTER *CurEntry;    /* Points to most recent data frame */
   REENTER *ReEntHold;          /* Temp used to setup the frame     */
   PASSDATA PDat;               /* Used for call to EditEAValue     */
   USHORT *pusPtr,cnt;          /* Utility vars                     */
   SHORT  sOffset;
   CHAR   *pInsert,*pValue,*pDelete;
   CHAR   szCodePage[LENUSHORTBUF];
   BYTE   fNeed;                /* Holds state of need checkbox     */

   switch(msg)
   {
      case WM_INITDLG:
         pPDat = PVOIDFROMMP(mp2);             /* Set up static vars        */
         pFEA  = (HOLDFEA *) pPDat->Point;

         GetMem(ReEntHold,sizeof(REENTER));    /* Allows for recusive calls */
         ReEntHold->pPDat = pPDat;
         ReEntHold->pFEA  = pFEA;
         ReEntHold->next  = CurEntry;
         CurEntry = ReEntHold;

         WinSendDlgItemMsg(hwnd, IDD_NEEDBIT, BM_SETCHECK, /* Need bit setup */
                           MPFROM2SHORT((pFEA->fEA&0x80)? TRUE : FALSE,0),
                           NULL);

         WinSetDlgItemText(hwnd,IDD_EATYPE,"");  /* Setup the codepage */
         WinSetDlgItemText(hwnd,IDD_EANAME,pFEA->szName);
         WinSendDlgItemMsg(hwnd, IDD_CODEPAGE,EM_SETTEXTLIMIT,
                                 MPFROM2SHORT(LENUSHORTBUF-1,0),NULL);

         pusPtr= (USHORT *) ((CHAR *) pFEA->aValue+pPDat->usMultiOffset);
         pusPtr++;       /* Skip the EA_Type field and point to codepage */
         sprintf(szCodePage,"%u",*pusPtr);
         WinSetDlgItemText(hwnd, IDD_CODEPAGE,szCodePage);

         pusPtr++;      /* Skip codepage and point to the field count */

         for(cnt=0;cnt< *pusPtr;cnt++) /* Add each field to the L-Box */
         {
            pInsert = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset,cnt);
            pValue = EAValueString(hwnd,pInsert); /* Ptr to asciiz string */

            WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_INSERTITEM,
                              MPFROM2SHORT(LIT_END,0),
                              MPFROMP(pValue));
            FreeMem(pValue,strlen(pValue)+1);  /* Free asciiz string */
         }
         return 0L;

      case WM_CONTROL:
         switch(SHORT1FROMMP(mp1))
         {
            case IDD_LBOX:
               switch(SHORT2FROMMP(mp1))
               {
                  case LN_SELECT:   /* Display proper EA type for selection */
                     sOffset = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                                       LM_QUERYSELECTION,0,0));
                     if(sOffset<0)
                        break;

                     pValue = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset,
                                             sOffset);
                     pusPtr = (USHORT *) pValue;

                     WinSetDlgItemText(hwnd,IDD_EATYPE,
                                ConvTable[LookupEAType(*pusPtr)].szFormat);
                     break;

                  case LN_ENTER:  /* Setup and edit a m-m field */
                     sOffset = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                                       LM_QUERYSELECTION,0,0));
                     if(sOffset<0)
                        return 0L;

                     PDat.Point         = (CHAR *) pFEA; /* Pass curr data */
                     PDat.cbMulti       = pPDat->cbMulti;
                     PDat.usMultiOffset = pPDat->usMultiOffset;
                     PDat.usIndex       = (USHORT) sOffset;

                     if(EditEAValue(hwnd,&PDat)) /* They didn't cancel */
                     {
                        pInsert=MultiTypeIndex(pFEA->aValue
                                                + pPDat->usMultiOffset,
                                               sOffset);
                        pValue =EAValueString(hwnd,pInsert);

                        WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_SETITEMTEXT,
                                          MPFROMSHORT(sOffset),
                                          MPFROMP(pValue));
                        FreeMem(pValue,strlen(pValue)+1); /* Free asciiz str */
                     }
                     return 0L;
               }
         }

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case IDD_ADD:                  /* Add an item for a m-m */
               MultiAdd(hwnd, pFEA,pPDat);
               return 0L;

            case IDD_EDIT:  /* Setup and edit a m-m field */
               sOffset = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                                   LM_QUERYSELECTION,0,0));
               if(sOffset<0)
                  return 0L;

               PDat.Point         = (CHAR *) pFEA; /* Pass curr data */
               PDat.cbMulti       = pPDat->cbMulti;
               PDat.usMultiOffset = pPDat->usMultiOffset;
               PDat.usIndex       = (USHORT) sOffset;

               if(EditEAValue(hwnd,&PDat)) /* They didn't cancel */
               {
                  pInsert=MultiTypeIndex(pFEA->aValue + pPDat->usMultiOffset,
                                         sOffset);
                  pValue =EAValueString(hwnd,pInsert);

                  WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_SETITEMTEXT,
                                    MPFROMSHORT(sOffset),
                                    MPFROMP(pValue));
                  FreeMem(pValue,strlen(pValue)+1); /* Free asciiz str */
               }
               return 0L;

            case IDD_DELETE:               /* Delete hi-lited item  */
               sOffset = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                                    LM_QUERYSELECTION,0,0));
               if(sOffset<0)   /* No item is currently selected */
                  return 0L;

               WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_DELETEITEM,
                                 MPFROMSHORT(sOffset),0L);

               /* Get pointers to start of selected field and the following
                  field, then move the rest of the EA back, resize the
                  buffer, and change the EA Value counter                  */

               pDelete = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset,
                                        sOffset);
               pValue  = MultiTypeIndex(pFEA->aValue+pPDat->usMultiOffset,
                                        sOffset+1);
               memmove(pDelete,pValue,pFEA->cbValue-(pValue-pFEA->aValue));
               ResizeMem(pFEA->aValue,pFEA->cbValue,
                         pFEA->cbValue-(pValue-pDelete));
               pFEA->cbValue -= pValue-pDelete;

               /* Decrement the field count in the m-m */
               pusPtr  = (USHORT *) ((CHAR *) pFEA->aValue +
                                              pPDat->usMultiOffset);
               pusPtr+=2;         /* Point to the field count */
               *pusPtr -= 1;      /* Decrement the field cnt  */

               FILE_CHANGED = TRUE;
               return 0L;

            case DID_DONE:
               /* Handle the possible change of the need checkbox */
               fNeed = (BYTE) SHORT1FROMMR(WinSendDlgItemMsg(hwnd, IDD_NEEDBIT,
                                                BM_QUERYCHECK,
                                                0L, 0L));
               if(fNeed)
                  fNeed = 0x80;
               if(fNeed != (pFEA->fEA & (BYTE) 0x80)) /* Need changed */
               {
                  pFEA->fEA = (pFEA->fEA & (BYTE) 0x7F) | fNeed;
                  FILE_CHANGED = TRUE;
               }

               /* Handle the possible change of the codepage */
               WinQueryDlgItemText(hwnd, IDD_CODEPAGE,
                                   LENUSHORTBUF, szCodePage);
               sscanf(szCodePage,"%u",&cnt);
               pusPtr= (USHORT *) ((CHAR *) pFEA->aValue+pPDat->usMultiOffset);
               pusPtr++;    /* Skip the EA_Type field and point to codepage */
               if(*pusPtr != cnt) /* They changed the codepage value */
               {
                  *pusPtr = cnt;
                  FILE_CHANGED = TRUE;
               }

               ReEntHold = CurEntry->next;       /* Finish recursion safety */
               FreeMem(CurEntry,sizeof(REENTER));
               CurEntry = ReEntHold;
               if(CurEntry)
               {
                  pPDat = CurEntry->pPDat;
                  pFEA  = CurEntry->pFEA;
               }
               WinDismissDlg(hwnd, TRUE);
               return 0L;
         }
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: ShowEAType()
 *
 * Parameters:  hwnd which is the current window handle.
 *
 * Returns: VOID
 *
 * Purpose: This routine handles the display of the current EA type by
 *          setting the text in the appropriate dialog field.
 *
 * Usage/Warnings:  Routine should be bullet proof as it does its own
 *                  error checking.  It assumes that hwnd points to the
 *                  correct window with the name listbox in it.
 *
 * Calls: GetCurFEA(), LookupEAType()
 */

VOID ShowEAType(HWND hwnd)
{
   USHORT usEAType;         /* Holds the offset of EAType into ConvTable */
   HOLDFEA *pFEA;

   pFEA = GetCurFEA(hwnd,pHoldFEA); /* Points to the selected EA */
   if(!pFEA)                        /* Kill line if nothing is selected */
   {
      WinSetDlgItemText(hwnd,IDD_EATYPE,"");
      return;
   }
   usEAType = LookupEAType((USHORT) *pFEA->aValue);

   WinSetDlgItemText(hwnd,IDD_EATYPE,ConvTable[usEAType].szFormat);
}



/*
 * Function name: EditEA()
 *
 * Parameters:  hwnd which is the current window handle.
 *
 * Returns: TRUE iff the edit was successful.
 *
 * Purpose: This routine handles the editing of an EA when it is selected
 *          from the listbox.  It get the item selected, sets up the PassData
 *          structure then calls EditEAValue to do the actual edit.
 *
 * Usage/Warnings:  Routine should be bullet proof as it does its own
 *                  error checking.  It assumes that hwnd points to the
 *                  correct window with the name listbox in it.
 *
 * Calls: EditEAValue()
 */

BOOL EditEA(HWND hwnd)
{
   HOLDFEA *pFEA=pHoldFEA;  /* Pointer for selected EA */
   LONG offset,lCnt;        /* Both used to reference offset of selected EA */
   PASSDATA PData;

   if(!FILE_ISOPEN)
      return(FALSE);

   offset = lCnt = (LONG) WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                            LM_QUERYSELECTION,0,0);

   if(offset<0)        /* Nothing was selected */
      return(FALSE);

   while(lCnt--)         /* Get to the right EA */
      pFEA = pFEA->next;

   PData.Point         = (CHAR *) pFEA;        /* Set the pass data struct */
   PData.usMultiOffset = 0;
   PData.cbMulti       = 0;

   if(EditEAValue(hwnd,&PData))  /* It worked */
   {
      WinSendDlgItemMsg(hwnd, IDD_LBOX,LM_SETITEMTEXT,
                        MPFROMSHORT((SHORT) offset),MPFROMP(pFEA->szName));
      return(TRUE);
   }
   return(FALSE);
}


/* This routine returns a pointer to the EA currently highlighted in the
   list box. */

/*
 * Function name: GetCurFEA()
 *
 * Parameters:  hwnd which is the current window handle.
 *              pFEA which points to the base FEA in the linked list.
 *
 * Returns: The current EA as determined by querying the l-box selector,
 *          returns NULL if there is an error or nothing is selected.
 *
 * Purpose: This routine returns a pointer to the EA currently highlighted
 *          in the list box.
 *
 * Usage/Warnings:  Routine should be bullet proof as it does its own
 *                  error checking.  It assumes that hwnd points to the
 *                  correct window with the name listbox in it.
 *
 * Calls: EditEAValue()
 */

HOLDFEA *GetCurFEA(HWND hwnd, HOLDFEA *pFEA)
{
   LONG lOffset;

   lOffset = (LONG) WinSendDlgItemMsg(hwnd, IDD_LBOX,
                                      LM_QUERYSELECTION,0,0);

   if(lOffset<0)      /* Nothing is highlighted */
      return(NULL);

   while(lOffset--)
   {
      pFEA = pFEA->next;
   }

   return(pFEA);
}



