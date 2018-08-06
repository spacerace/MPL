/*************************************************************

 This program demonstrates the use of many threads by doing
 LISTCNT simultaneous sorts.  Each sorting algorithm runs
 from a separate thread, the routine which updates the display
 is run from another thread, and the main thread is used to
 handle the main window's messages.  The display thread is
 started when the program begins and is not terminated (allows
 default cleanup to terminate thread) as the display routine
 is used throughout the program's life.
 This code was written to allow easy modification.  To change
 the number of simultaneous sorts, change the LISTCNT #define
 and modify the global arrays which control the screen
 drawing color, the sort each thread is to use, and the sort name.
 Note:  This program was not intended to infer the relative
        speeds of the various sorting algorithms.

 This program was written by Jeff Johnson, 8/89.

 Procedures in this file:
   main()             Sets up the PM environment and calls
                      the main dialog procedure ClientWndProc
   ClientWndProc()    Handles the main window messages
   CalcThread()       Generic stub that sets up, executes, and
                      terminates each sort in an aux. thread
   DispThread()       Updates the display during the sorts
   BubbleSort()       Implements a bubble sort
   InsertionSort()    Implements an insertion sort
   BatcherSort()      Stub that calls recursive Batcher sort
   BatcherSortR()     Implements a Batcher sort
   QuickSort()        Stub that calls recursive Quick sort
   QuickSortR()       Implements a Quick sort
   EnableMenuItem()   Activates/deactivates a menu choice
   EntryFldDlgProc()  Handles the set number of disks dialog box
   RandomizeData()    Randomizes the data arrays for the sorts

**************************************************************/

#include "sort.h"

/********************* GLOBALS *******************************/

CHAR szClientClass[] = "Sort";
USHORT Data[LISTCNT][NELEMS];     /* Array that contains the data to sort  */

/* Global data that defines each of the sort routines */
ULONG  ulColors[LISTCNT] =   { CLR_RED,        /* Color of each of the     */
                               CLR_BLUE,       /* LISTCNT data lists.      */
                               CLR_DARKGREEN,
                               CLR_YELLOW };
VOID((*pExecSub[LISTCNT])())=  { BubbleSort,     /* List of sorts to be      */
                               BatcherSort,    /* executed for each of the */
                               QuickSort,      /* LISTCNT threads.         */
                               InsertionSort };
CHAR *szSubNames[LISTCNT] =  { "Bubble Sort",  /* Ascii names of sorts     */
                               "Batcher Sort",
                               "Quick Sort",
                               "Insertion Sort" };

/*************************************************************/


/*
 * Function name: main()
 *
 * Parameters:  NONE
 *
 * Returns: Always returns 0
 *
 * Purpose: Sets up the PM environment, calls the main window proc,
            handles the window's messages then cleans up and exits.
 *
 * Usage/Warnings:
 *
 * Calls: ClientWndProc() (thru PM)
 */

int main()
{
   HAB          hab;
   HMQ          hmq;
   HWND         hwndFrame, hwndClient;
   QMSG         qmsg;

   ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU | FCF_MINMAX    |
                        FCF_SHELLPOSITION | FCF_MENU    | FCF_TASKLIST  |
                        FCF_SIZEBORDER    | FCF_ICON;

   /* These PM calls should be error checked */
   hab = WinInitialize(0);
   hmq = WinCreateMsgQueue(hab, 0);

   if(!WinRegisterClass(hab, szClientClass,ClientWndProc,0L,0))
   {
      WinAlarm(HWND_DESKTOP, WA_ERROR);        /* Register failed */
      DosExit(EXIT_PROCESS,1);
   }

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
                                  &flFrameFlags, szClientClass, NULL,
                                  0L, (HMODULE) NULL, ID_MAINMENU, &hwndClient);
   if(!hwndFrame)
   {
      WinAlarm(HWND_DESKTOP, WA_ERROR); /* Window create failed */
      DosExit(EXIT_PROCESS,1);
   }

   while(WinGetMsg(hab,&qmsg,NULL,0,0))        /* Message loop */
      WinDispatchMsg(hab,&qmsg);

   WinDestroyWindow(hwndFrame);                /* Clean up     */
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
}


/*
 * Function name: ClientWndProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              No user data is expected in the WM_CREATE.
 *
 * Returns: Returns with WM_QUIT message
 *
 * Purpose: Handles all the messages associated with the main window
 *          and calls the appropriate handling procedures.
 *
 * Usage/Warnings: Called only by main()
 *
 * Calls: RandomizeData(), DispThread() (thru thread),
          CalcThread() (thru thread)
 */

MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   HPS    hps;                           /* Handle for painting            */
   RECTL  rcl;                           /* Rectangle struct for painting  */
   POINTL ptl;                           /* Point struct for painting      */
   static CALCPARAM cp[LISTCNT];         /* Struct passed to sort threads  */
   static TID    tidCalc[LISTCNT];       /* Sort threads IDs               */
   static VOID   *pThreadStack[LISTCNT]; /* Pointer to sort threads stacks */
   static CALCPARAM cpDisp;              /* Struct passed to disp thread   */
   static TID    tidDisplay;             /* Secondary display thread ID    */
   static VOID   *pDisplayStack;         /* Ptr to secondary display stack */
   static USHORT usWindowYCoord=1;       /* Holds current client window ht */
   static BYTE   cThreadCnt = 0;         /* Count of sort threads running  */
   static USHORT cSetSize = NELEMS;      /* User sort set size             */
   USHORT cnt,cnt2,usID;                 /* Utility counters               */
   ULONG  ulBlip;                        /* Holds the marker pos for Paint */

   switch(msg)
   {
      case WM_CREATE:
         RandomizeData(cSetSize);        /* Initially randomize data set   */

         /* Start display thread */
         if((pDisplayStack = malloc(STACKSIZE)) == NULL)
         {
            WinAlarm(HWND_DESKTOP, WA_ERROR);      /* Can't allocate stack */
            DosExit(EXIT_PROCESS,1);
         }

         cpDisp.hwnd        = hwnd;
         cpDisp.Array       = &usWindowYCoord;
         cpDisp.pcThreadCnt = &cThreadCnt;
         cpDisp.pcSetSize   = &cSetSize;

         if((tidDisplay = _beginthread(DispThread,pDisplayStack,
                                         STACKSIZE, &cpDisp))  == -1)
         {
            free(pDisplayStack);          /* Couldn't start display thread */
            WinAlarm(HWND_DESKTOP, WA_ERROR);
            DosExit(EXIT_PROCESS,1);
         }
         /* Up the display priority an arbitrary amount to cause it to
            update more rapidly and to give it a higher priority than
            the sort threads.*/
         DosSetPrty(PRTYS_THREAD,
                    PRTYC_REGULAR,5,tidDisplay);
         return 0L;

      case WM_SIZE:
         WinInvalidateRect(hwnd,NULL,FALSE);     /* Force a redraw */
         return WinDefWindowProc(hwnd, msg, mp1, mp2);

      case WM_PAINT:
         hps = WinBeginPaint(hwnd,NULL,NULL);   /* Get paint handle      */
         WinQueryWindowRect(hwnd,&rcl);
         usWindowYCoord = (USHORT) rcl.yTop;

         if((XOFFSET < (USHORT) (rcl.xRight-rcl.xLeft)) && /* Draw if screen */
            (YOFFSET < (USHORT) (rcl.yTop-rcl.yBottom)))   /* is big enough  */
         {
            DrawRect(rcl.xLeft,rcl.yBottom,         /* White out the screen  */
                     rcl.xRight,rcl.yTop,CLR_WHITE);

            DrawLine(XOFFSET, YOFFSET,              /* Draw baseline         */
                     rcl.xRight,YOFFSET,CLR_BLACK);

            DrawLine(XOFFSET, YOFFSET,              /* Draw vertical line    */
                     XOFFSET,rcl.yTop,CLR_BLACK);

            for(cnt=0;cnt<LISTCNT;cnt++)            /* Draw data points      */
            {
               for(cnt2=0;cnt2<cSetSize;cnt2++)
               {
                  ulBlip = (rcl.yTop-YOFFSET-5) * Data[cnt][cnt2] / RAND_MAX
                           + YOFFSET+1;

                  Draw2Pel(XOFFSET+1 + cnt2, ulBlip, ulColors[cnt]);
               }
            }
            for(cnt=0;cnt<COLUMNCNT;cnt++)          /* Do bottom legend      */
            {
               for(cnt2=1;cnt2<=ROWCNT;cnt2++)
               {
                  usID = cnt*ROWCNT + cnt2-1;
                  if(usID >= LISTCNT)
                     break;

                  ptl.x = XOFFSET + cnt*COLUMNOFFSET;
                  ptl.y = YOFFSET - cnt2*ROWOFFSET;

                  GpiSetColor(hps,ulColors[usID]);
                  GpiCharStringAt(hps,&ptl,(LONG) strlen(szSubNames[usID]),
                                  szSubNames[usID]);
               }
            }
         }

         WinEndPaint(hps);
         return 0L;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case IDM_START:

               /* Try to get stack space */
               for(cnt=0;cnt<LISTCNT;cnt++)
               {
                  if((pThreadStack[cnt] = malloc(STACKSIZE)) == NULL)
                  {
                     WinAlarm(HWND_DESKTOP, WA_ERROR); /* Can't get memory */
                     return 0L;
                  }
                  cp[cnt].hwnd          = hwnd;  /* Set the static struct  */
                  cp[cnt].fContinueCalc = TRUE;
                  cp[cnt].pFunc         = pExecSub[cnt];
                  cp[cnt].usID          = cnt;
                  cp[cnt].Array         = Data[cnt];
                  cp[cnt].cArray        = cSetSize;

                  /* Try to start the thread */
                  if((tidCalc[cnt] = _beginthread(CalcThread,pThreadStack[cnt],
                                                  STACKSIZE, &cp[cnt]))  == -1)
                  {
                     free(pThreadStack[cnt]);    /* Thread wouldn't start  */
                     WinAlarm(HWND_DESKTOP, WA_ERROR);
                     return 0L;
                  }
                  if(cThreadCnt++ == 0)  /* When the first thread starts */
                  {
                     /* Disable Start, Set, and Randomize, enable Stop */
                     EnableMenuItem(hwnd,IDM_START,FALSE);
                     EnableMenuItem(hwnd,IDM_SET,FALSE);
                     EnableMenuItem(hwnd,IDM_RANDOM,FALSE);
                     EnableMenuItem(hwnd,IDM_STOP,TRUE);
                  }
               }
               return 0L;

            case IDM_STOP:
               for(cnt=0;cnt<LISTCNT;cnt++)
                  cp[cnt].fContinueCalc = FALSE;  /* Notify thread to quit  */
               return 0L;

            case IDM_RANDOM:
               RandomizeData(cSetSize);                /* Randomize data */
               WinInvalidateRect(hwnd,NULL,FALSE);     /* Force a redraw */
               return 0L;

            case IDM_SET:
               if(WinDlgBox(HWND_DESKTOP, hwnd, /* Pop up the query/set box */
                            EntryFldDlgProc,(HMODULE) NULL,ID_SETCOUNT,
                            (VOID FAR *) &cSetSize))
               {
                  WinInvalidateRect(hwnd,NULL,FALSE);     /* Force a redraw */
               }
               return 0L;

            default:
               return WinDefWindowProc(hwnd, msg, mp1, mp2);
         }

      case UM_CALC_DONE:
         usID = (USHORT) SHORT1FROMMP(mp1); /* Get ID of quit thread     */

         if(--cThreadCnt == 0)              /* If all quit, enable menus */
         {
            EnableMenuItem(hwnd,IDM_START,TRUE);
            EnableMenuItem(hwnd,IDM_SET,TRUE);
            EnableMenuItem(hwnd,IDM_RANDOM,TRUE);
            EnableMenuItem(hwnd,IDM_STOP,FALSE);
         }

         free(pThreadStack[usID]);          /* Free thread's stack space */
         return 0L;

      default:
         return WinDefWindowProc(hwnd, msg, mp1, mp2);
   }
}


/*
 * Function name: CalcThread()
 *
 * Parameters:  pcp is a struct which contains the hwnd handle, the
 *              continue flag which is initially set to TRUE, the ID
 *              which is the
 *
 * Returns: VOID
 *
 * Purpose: This generic stub calls the passed "sort" function with a pointer
 *          to the data and an item count, then when the sort terminates, it
 *          cleans up by Posting a done message, then terminating the thread.
 *
 * Usage/Warnings: No DosExitCritSec() is called since _endthread()
 *                 clears the critical section when the thread is
 *                 terminated.
 *
 * Calls:  Whatever function is passed in pcp->pFunc.  (As initially
 *         configured, it calls:  BubbleSort(), InsertionSort(),
 *         BatcherSort(), and QuickSort()
 */

VOID _cdecl FAR CalcThread(PCALCPARAM pcp)
{
   (*pcp->pFunc)(pcp);                      /* Execute recurs routine */

   DosEnterCritSec(); /* Set Crit so the UM_CALC_DONE isn't processed */
                      /* until this thread has completely terminated  */
   WinPostMsg(pcp->hwnd,UM_CALC_DONE,
              MPFROMSHORT(pcp->usID),NULL);              /* Post done */

   _endthread();                /* Terminate thread and exit crit sec */
}


/*
 * Function name: DispThread()
 *
 * Parameters:  pcp is a struct which contains the hwnd handle, the
 *              count of active threads, the ->Array element points to
 *              the current height of the window which can be dynamically
 *              resized by the user, the pcSetSize points to the current
 *              set size which can be changed by the user only when the
 *              sort is stopped.
 *              None of the other pcp members are used.
 *
 * Returns: VOID
 *
 * Purpose: This routine is run as a secondary thread to update the screen
 *          when the sort threads are running.  It works by making passes
 *          through the data lists and redrawing them.  A DosSleep(0) is
 *          called at the end of each pass to help sychronize things a bit.
 *
 * Usage/Warnings: Note that this thread is started when the program is
 *                 initialized, and is never terminated.  Note that
 *                 fDoUpdate is used in such a way that the screen is
 *                 ALWAYS updated once after all threads have terminated.
 *                 This avoids the possibility of the display not being
 *                 fully accurate when the sorts terminate.
 *
 * Calls:
 */

VOID _cdecl FAR DispThread(PCALCPARAM pcp)
{
   HAB hab;              /* Anchor block, used just for WinInitialize() */
   USHORT cnt,cnt2;      /* Utility counters                            */
   HPS hps;              /* Presentation space handle                   */
   POINTL ptl;           /* Used for various drawing macros             */
   ULONG  ulBlip;        /* Holds the Y offset for a given data point   */
   BYTE   fDoUpdate = 0; /* Used to ensure screen is fully updated      */
                         /* after all sort threads terminate            */
   USHORT *pusYCoord = pcp->Array; /* Points to location that always    */
                                   /* contains current client wind hgt  */

   hab = WinInitialize(0);    /* Called to increase Ring 2 stack size   */
   hps = WinGetPS(pcp->hwnd);

   while(TRUE)
   {
      if(*pcp->pcThreadCnt != 0)          /* Set update flag if at least 1 */
         fDoUpdate = (BYTE) (*pcp->pcThreadCnt+1); /* sort thread is still running  */

      while(!fDoUpdate && !(*pcp->pcThreadCnt));  /* Only update when a    */
                                                  /* sort is running       */
      for(cnt=0;cnt<*pcp->pcSetSize;cnt++)        /* Update data set       */
      {
         DrawLine(XOFFSET+1 + cnt,YOFFSET+1,      /* Erase vertical column */
                  XOFFSET+1 + cnt,*pusYCoord, CLR_WHITE);

         for(cnt2=0;cnt2<LISTCNT;cnt2++)          /* Draw each point       */
         {
            ulBlip = (ULONG) (*pusYCoord-YOFFSET-5)*Data[cnt2][cnt] / RAND_MAX
                      + YOFFSET+1;
            Draw2Pel(XOFFSET+1 + cnt, ulBlip, ulColors[cnt2]);
         }
      }
      fDoUpdate--;  /* Decrement update flag */
   }

   /* Note that these 3 lines NEVER get executed, but should the program be
      modified to end the thread, this is the appropriate termination code. */
   WinReleasePS(hps);
   WinTerminate(hab);
   _endthread();
}


/*
 * Function name: BubbleSort()
 *
 * Parameters:  pcp is a struct which contains the hwnd handle, a
 *              pointer to the data array, and a count of the items in
 *              the data array.
 *              None of the other pcp members are used.
 *
 * Returns: VOID
 *
 * Purpose:  Implements a bubble sort which is an O(n^2) algorithm.  It
 *           works by repeatedly going through the data and comparing
 *           consecutive elements and swapping them if they aren't in
 *           the correct order.  This guarantees that each pass will
 *           place at least one additional item in its appropriate
 *           place.
 *
 * Usage/Warnings:
 *
 * Calls:
 */

VOID BubbleSort(PCALCPARAM pcp)
{
   BOOL fModified = FALSE; /* Set whenever a swap is done, if an entire */
                           /* pass doesn't set the flag, the we're done */
   SHORT cnt,cnt2;         /* Counters used for the 2-level loops       */
   USHORT usTemp;          /* Used to hold a data item during a swap    */

   for(cnt=pcp->cArray-1;cnt>=0;cnt--) /* Set for the max no. of passes */
   {
      for(cnt2=0;cnt2<cnt;cnt2++) /* Only sort thru current cnt pass    */
      {
         if(!pcp->fContinueCalc)  /* User wishes to terminate the sort  */
            return;

         if(pcp->Array[cnt2]>pcp->Array[cnt2+1]) /* Items need to swap  */
         {
            fModified = TRUE;

            usTemp = pcp->Array[cnt2];
            pcp->Array[cnt2] = pcp->Array[cnt2+1];
            pcp->Array[cnt2+1]= usTemp;
         }
      }
      if(!fModified)          /* Nothing changed during the entire pass */
         break;
      fModified = FALSE;                       /* Reset the modify flag */
   }
}


/*
 * Function name: InsertionSort()
 *
 * Parameters:  pcp is a struct which contains the hwnd handle, a
 *              pointer to the data array, and a count of the items in
 *              the data array.
 *              None of the other pcp members are used.
 *
 * Returns: VOID
 *
 * Purpose:  Implements an insertion sort which is an O(n^2) algorithm.
 *           This sort works much faster and does not require that much
 *           additional code to implement.  It works by setting a sorted
 *           list to be just the first element, the working each
 *           successive item into the already sorted list.
 *
 * Usage/Warnings:
 *
 * Calls:
 */

VOID InsertionSort(PCALCPARAM pcp)
{
   SHORT cnt,cnt2;         /* Counters used for the 2-level loops       */
   USHORT usTemp;          /* Used to hold a data item during a swap    */

   for(cnt=1;cnt<(SHORT)pcp->cArray;cnt++)         /* Insert each item in turn */
   {
      if(!pcp->fContinueCalc)      /* User wishes to terminate the sort */
         return;
      usTemp=pcp->Array[cnt];                   /* Hold value to insert */
      cnt2=cnt-1;
      while(pcp->Array[cnt2]>usTemp)    /* Move items down to make room */
      {
         pcp->Array[cnt2+1]=pcp->Array[cnt2];
         cnt2--;
         if(cnt2<0)
            break;
      }
      pcp->Array[cnt2+1] = usTemp;                   /* Insert the item */
   }
}


/*
 * Function name: BatcherSort()
 *
 * Parameters:  pcp is a struct which contains the continue flag,
 *              a pointer to the data array, and a count of the items in
 *              the data array.
 *              None of the other pcp members are used.
 *
 * Returns: VOID
 *
 * Purpose:  This routine is a stub that calls the recursive Batcher sort
 *           with the proper initial arguments.
 *
 * Usage/Warnings:  It passes the pcp, the size of the array to be sorted,
 *                  the offset to start sorting at(0), the number number
 *                  of elements each item is from neighboring elements (1),
 *                  and Half flag to sort the halves (1=YES).
 *
 * Calls:  BatcherSortR()
 */

VOID BatcherSort(PCALCPARAM pcp)
{
   BatcherSortR(pcp,pcp->cArray,0,1,1);
}


/*
 * Function name: BatcherSortR()
 *
 * Parameters:  pcp is a struct which contains the continue flag,
 *              a pointer to the data array.
 *              None of the other pcp members are used.
 *              usArrSize is the number of elements in the current sort set.
 *              usStart is the offset to the 1st element in the set.
 *              usSkip is the spacing between consecutive elements.
 *              fHalves sorts the 2 halves when set, otherwise skips the
 *                      1st 2 of the 4 sub-sorts.
 *
 * Returns: VOID
 *
 * Purpose:  Implements Batcher sort which is O(n lg n).  The advantage
 *           of the batcher sort is that the comparisons made do NOT
 *           depend upon the outcome of previous comparisons.  This makes
 *           it a good algorithm for parallelism.  The algorithm works as
 *           follows:  Sort the first half, sort the second half, sort the
 *           odd elements, sort the even elements, then compare swap
 *           elements 2/3, 4/5, ...
 *
 * Usage/Warnings:  There are several adaptations to the algorithm to
 *                  allow it to work with arbitrary sized data sets
 *                  (the original required a power of 2 sized data
 *                  set):  if the set size is less than 2, the routine
 *                  returns, the first "half" is always the largest
 *                  possible power of two, and the top value for the
 *                  final compare/swap is adjusted to round up in
 *                  case of an odd data set.
 *                  Another optimization is that involving the fHalves
 *                  flag.  This stems from the observation that when
 *                  the odd/even sort recurses, the first and second
 *                  halves are already sorted, thus the first 2
 *                  recursive calls are unnecessary in this case.
 *
 * Calls:  BatcherSortR() (recursively)
 */

VOID BatcherSortR(PCALCPARAM pcp, USHORT usArrSize, USHORT usStart,
                  USHORT usSkip, BOOL fHalves)
{
   USHORT cnt,usUpper,usTemp; /* Utility variables */

   if(!pcp->fContinueCalc) /* User wishes to terminate the sort */
      return;

   if(usArrSize<2) /* No sorting needed if <2 items in the set */
      return;

   if(usArrSize==2) /* Do simple compare/swap if there are 2 elements */
   {
      if(pcp->Array[usStart]>pcp->Array[usStart+usSkip])
      {
         usTemp = pcp->Array[usStart];
         pcp->Array[usStart] = pcp->Array[usStart+usSkip];
         pcp->Array[usStart+usSkip]= usTemp;
      }
      return;
   }

   usTemp=1;                  /* usTemp ends up holding the smallest power */
   while(usTemp < usArrSize)  /* of 2 that is at least as big as usArrSize */
      usTemp *= 2;

   if(fHalves)  /* If the sort was NOT called by the odd/even recurses */
   {
      BatcherSortR(pcp,usTemp/2,usStart,usSkip,1);    /* Sort 1st half */
      BatcherSortR(pcp,usArrSize-usTemp/2,            /* Sort 2nd half */
                   usStart+usTemp/2*usSkip,usSkip,1);
   }
   BatcherSortR(pcp,usArrSize-usArrSize/2,usStart,usSkip*2,0); /* Sort evens */
   BatcherSortR(pcp,usArrSize/2,usStart+usSkip,usSkip*2,0);    /* Sort odds  */

   if(!pcp->fContinueCalc) /* User wishes to terminate the sort */
      return;

   usUpper=usStart+usSkip+(usArrSize-usArrSize/2-1)*2*usSkip;
   for(cnt=usStart+usSkip;cnt<usUpper;cnt+=usSkip*2) /* Do final compares */
   {
      if(pcp->Array[cnt]>pcp->Array[cnt+usSkip])
      {
         usTemp = pcp->Array[cnt];
         pcp->Array[cnt] = pcp->Array[cnt+usSkip];
         pcp->Array[cnt+usSkip]= usTemp;
      }
      if(!pcp->fContinueCalc) /* User wishes to terminate the sort */
         return;
   }
}


/*
 * Function name: QuickSort()
 *
 * Parameters:  pcp is a struct which contains the continue flag,
 *              a pointer to the data array, and a count of the items in
 *              the data array.
 *              None of the other pcp members are used.
 *
 * Returns: VOID
 *
 * Purpose:  This routine is a stub that calls the recursive Quick sort
 *           with the proper initial arguments.
 *
 * Usage/Warnings:  It passes the pcp (Which contains the array ptr),
 *                  the offset to start sorting at(0), and the offset to
 *                  finish sorting at (Array size-1).
 *
 * Calls:  QuickSortR()
 */

VOID QuickSort(PCALCPARAM pcp)
{
   QuickSortR(pcp,0,pcp->cArray-1);
}


/*
 * Function name: QuickSortR()
 *
 * Parameters:  pcp is a struct which contains the continue flag,
 *              a pointer to the data array.
 *              None of the other pcp members are used.
 *              sLeft is the offset of the leftmost sort element (smallest).
 *              sRight is the offset of rightmost sort element (largest).
 *
 * Returns: VOID
 *
 * Purpose:  Implements Quick sort which is O(n lg n).  Quick sort is a
 *           good all-purpose sorting algorithm and is widely used.  This
 *           implementation works by placing the first element in the list
 *           into its correct place, by moving all elements smaller to its
 *           left, and all numbers larger to its right.  The Quick sort
 *           recurses on the 2 halves on either side of the properly placed
 *           element.
 *
 * Usage/Warnings:
 *
 * Calls:  QuickSortR() (recursively)
 */

VOID QuickSortR(PCALCPARAM pcp, SHORT sLeft, SHORT sRight)
{
   SHORT sTempLeft  = sLeft;    /* Holds lower bound on split position      */
   SHORT sTempRight = sRight;   /* Holds upper bound on split position      */
   SHORT sSplit;                /* Used to hold offset of positioned elem   */
   USHORT usTemp;               /* Holds temp element during positioning    */

   if(sLeft>=sRight)            /* If there is <=1 element, return          */
      return;

   if(!pcp->fContinueCalc)      /* User wishes to terminate the sort        */
      return;

   /* Split list, sSplit will contain the split element */
   usTemp = pcp->Array[sTempLeft];

   while(TRUE)       /* Loop thru 2 cases until the split position is found */
   {
      while(pcp->Array[sTempRight] > usTemp && /* Skip elems on right side  */
                        sTempRight > sLeft)    /* that are > the split elem */
         sTempRight--;

      if(sTempRight<=sTempLeft)  /* Left and Right have met, split is found */
      {
         pcp->Array[sTempLeft]=usTemp;  /* Place split elem at proper place */
         sSplit = sTempLeft;
         break;
      }
      pcp->Array[sTempLeft++] =
                    pcp->Array[sTempRight];    /* Move small element to LHS */

      while(pcp->Array[sTempLeft] < usTemp &&  /* Skip elems on left side   */
                        sTempLeft < sRight)    /* that are < the split elem */
         sTempLeft++;

      if(sTempRight<=sTempLeft)  /* Left and Right have met, split is found */
      {
         pcp->Array[sTempRight]=usTemp; /* Place split elem at proper place */
         sSplit = sTempRight;
         break;
      }
      pcp->Array[sTempRight--] =
                    pcp->Array[sTempLeft];     /* Move large element to RHS */
   }

   QuickSortR(pcp,sLeft,sSplit-1);     /* Sort the 1st half of the list     */

   if(!pcp->fContinueCalc)             /* User wishes to terminate the sort */
      return;

   QuickSortR(pcp,sSplit+1,sRight);    /* Sort the 2nd half of the list     */
}


/*
 * Function name: EnableMenuItem()
 *
 * Parameters:  hwnd is a handle of the current window.
 *              sMenuItem is the ID of the item to Enable/Disable.
 *              fEnable will enable item if TRUE, otherwise disables it.
 *
 * Returns: VOID
 *
 * Purpose: This routine handles enabling/disabling of menu items.  This
 *          is done by getting Parent and Menu hwnd handles then sending
 *          the appropriate message.
 *
 * Usage/Warnings:
 *
 * Calls:
 */

VOID EnableMenuItem(HWND hwnd, SHORT sMenuItem, BOOL fEnable)
{
   HWND hwndParent = WinQueryWindow(hwnd, QW_PARENT, FALSE);
   HWND hwndMenu   = WinWindowFromID(hwndParent, FID_MENU);

   WinSendMsg(hwndMenu, MM_SETITEMATTR,
              MPFROM2SHORT(sMenuItem, TRUE),
              MPFROM2SHORT(MIA_DISABLED, fEnable ? 0 : MIA_DISABLED));
}


/*
 * Function name: EntryFldDlgProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              A pointer to the current set size is passed in WM_INITDLG.
 *
 * Returns: Terminates with a TRUE iff a new valid data set size is entered.
 *
 * Purpose: Handles all the messages associated with the set entry field
 *          and calls the appropriate handling procedures.  The purpose
 *          of this dialog box is to get a new data set size for the
 *          sort routines.
 *
 *
 * Usage/Warnings: If the value entered is valid, the proc will set the
 *                 variable passed in through the WM_INITDLG to the value.
 *
 * Calls:
 */

MRESULT EXPENTRY EntryFldDlgProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   SHORT sNewSize;                                     /* Holds new set set */
   static USHORT FAR *pcSetSize;    /* Set to point to user-passed set size */
   switch(msg)
   {
      case WM_INITDLG:
         pcSetSize=PVOIDFROMMP(mp2);
         WinSendDlgItemMsg(hwnd, ID_ENTRYFLD,EM_SETTEXTLIMIT,  /* Limit len */
                                 MPFROM2SHORT(3,0),NULL);
         WinSetDlgItemShort(hwnd, ID_ENTRYFLD,(SHORT) *pcSetSize,TRUE);
         return 0L;                           /* Allow normal focus setting */

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case DID_OK:
               WinQueryDlgItemShort(hwnd, ID_ENTRYFLD,
                                    &sNewSize, TRUE); /* Get the short      */
               if(sNewSize>0 && sNewSize<=NELEMS)  /* Set new data set size */
               {
                  *pcSetSize = (USHORT) sNewSize;
                  WinDismissDlg(hwnd,TRUE);
               }
               else                                   /* Invalid value      */
                  WinDismissDlg(hwnd,FALSE);
               return 0L;

            case DID_CANCEL:
               WinDismissDlg(hwnd,FALSE);
               return 0L;

            default:
               return WinDefDlgProc(hwnd, msg, mp1, mp2);
         }

      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
}

/*
 * Function name: RandomizeData()
 *
 * Parameters:  cSetSize is the current size of the data set
 *
 * Returns: VOID
 *
 * Purpose: This routine randomizes the data arrays.
 *
 * Usage/Warnings: This routine assumes that LISTCNT and NELEMS define
 *                 the dimensions of the global Data array which is
 *                 randomized.
 *
 * Calls:
 */

VOID RandomizeData(USHORT cSetSize)
{
   USHORT cnt,cnt2;
   for(cnt=0;cnt<LISTCNT;cnt++)
      for(cnt2=0;cnt2<cSetSize;cnt2++)
         Data[cnt][cnt2] = rand();
}



