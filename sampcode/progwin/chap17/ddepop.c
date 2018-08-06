/*--------------------------------------------
   DDEPOP.C -- DDE Server for Population Data
               (c) Charles Petzold, 1990
  --------------------------------------------*/

#include <windows.h>
#include <dde.h>
#include <string.h>
#include <time.h>

struct
     {
     char *szState ;
     long lPop70 ;
     long lPop80 ;
     long lPop ;
     }
     pop [] = {
              "AL",   3444354,   3894025, 0, "AK",    302583,    401851, 0,
              "AZ",   1775399,   2716598, 0, "AR",   1923322,   2286357, 0,
              "CA",  19971069,  23667764, 0, "CO",   2209596,   2889735, 0,
              "CT",   3032217,   3107564, 0, "DE",    548104,    594338, 0,
              "DC",    756668,    638432, 0, "FL",   6791418,   9746961, 0,
              "GA",   4587930,   5462982, 0, "HI",    769913,    964691, 0,
              "ID",    713015,    944127, 0, "IL",  11110285,  11427409, 0,
              "IN",   5195392,   5490212, 0, "IA",   2825368,   2913808, 0,
              "KS",   2249071,   2364236, 0, "KY",   3220711,   3660324, 0,
              "LA",   3644637,   4206116, 0, "ME",    993722,   1125043, 0,
              "MD",   3923897,   4216933, 0, "MA",   5689170,   5737093, 0,
              "MI",   8881826,   9262044, 0, "MN",   3806103,   4075970, 0,
              "MS",   2216994,   2520770, 0, "MO",   4677623,   4916762, 0,
              "MT",    694409,    786690, 0, "NE",   1485333,   1569825, 0,
              "NV",    488738,    800508, 0, "NH",    737681,    920610, 0,
              "NJ",   7171112,   7365011, 0, "NM",   1017055,   1303302, 0,
              "NY",  18241391,  17558165, 0, "NC",   5084411,   5880415, 0,
              "ND",    617792,    652717, 0, "OH",  10657423,  10797603, 0,
              "OK",   2559463,   3025487, 0, "OR",   2091533,   2633156, 0,
              "PA",  11800766,  11864720, 0, "RI",    949723,    947154, 0,
              "SC",   2590713,   3120730, 0, "SD",    666257,    690768, 0,
              "TN",   3926018,   4591023, 0, "TX",  11198655,  14225513, 0,
              "UT",   1059273,   1461037, 0, "VT",    444732,    511456, 0,
              "VA",   4651448,   5346797, 0, "WA",   3413244,   4132353, 0,
              "WV",   1744237,   1950186, 0, "WI",   4417821,   4705642, 0,
              "WY",    332416,    469557, 0, "US", 203302031, 226542580, 0
              } ;

#define NUM_STATES (sizeof (pop) / sizeof (pop [0]))

typedef struct
     {
     unsigned int fAdvise:1 ;
     unsigned int fDeferUpd:1 ;
     unsigned int fAckReq:1 ;
     unsigned int dummy:13 ;
     long         lPopPrev ;
     }
     POPADVISE ;

#define ID_TIMER    1
#define DDE_TIMEOUT 3000

long FAR PASCAL WndProc         (HWND, WORD, WORD, LONG) ;
long FAR PASCAL ServerProc      (HWND, WORD, WORD, LONG) ;
BOOL FAR PASCAL TimerEnumProc   (HWND, LONG) ;
BOOL FAR PASCAL CloseEnumProc   (HWND, LONG) ;
BOOL            PostDataMessage (HWND, HWND, int, BOOL, BOOL, BOOL) ;

char   szAppName []     = "DdePop" ;
char   szServerClass [] = "DdePop.Server" ;
HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (hPrevInstance) 
          return FALSE ;

     hInst = hInstance ;

               // Register window class

     wndclass.style         = 0 ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     RegisterClass (&wndclass) ;

               // Register window class for DDE Server

     wndclass.style         = 0 ;
     wndclass.lpfnWndProc   = ServerProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 2 * sizeof (WORD) ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = NULL ;
     wndclass.hbrBackground = NULL ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szServerClass ;

     RegisterClass (&wndclass) ;

     hwnd = CreateWindow (szAppName, "DDE Population Server",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     SendMessage (hwnd, WM_TIMER, 0, 0L) ;   // initialize 'pop' structure

     if (!SetTimer (hwnd, ID_TIMER, 5000, NULL))
          {
          MessageBox (hwnd, "Too many clocks or timers!", szAppName,
                      MB_ICONEXCLAMATION | MB_OK) ;

          return FALSE ;
          }

     ShowWindow (hwnd, SW_SHOWMINNOACTIVE) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }

     KillTimer (hwnd, ID_TIMER) ;

     return msg.wParam ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static FARPROC lpTimerEnumProc, lpCloseEnumProc ;
     static char    szTopic [] = "US_Population" ;
     ATOM           aApp, aTop ;
     HWND           hwndClient, hwndServer ;
     int            i ;
     long double    ldSecsInDecade, ldSecSince1970 ;
     time_t         lSecSince1970 ;

     switch (message)
          {
          case WM_CREATE:
               lpTimerEnumProc = MakeProcInstance (TimerEnumProc, hInst) ;
               lpCloseEnumProc = MakeProcInstance (CloseEnumProc, hInst) ;
               return 0 ;

          case WM_DDE_INITIATE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- application atom
                    // HIWORD (lParam) -- topic atom

               hwndClient = wParam ;

               aApp = GlobalAddAtom (szAppName) ;
               aTop = GlobalAddAtom (szTopic) ;

                    // Check for matching atoms, create window, and acknowledge

               if ((LOWORD (lParam) == NULL || LOWORD (lParam) == aApp) &&
                   (HIWORD (lParam) == NULL || HIWORD (lParam) == aTop))
                    {
                    hwndServer = CreateWindow (szServerClass, NULL,
                                               WS_CHILD, 0, 0, 0, 0,
                                               hwnd, NULL, hInst, NULL) ;

                    SetWindowWord (hwndServer, 0, hwndClient) ;
                    SendMessage (wParam, WM_DDE_ACK, hwndServer,
                                 MAKELONG (aApp, aTop)) ;
                    }

                    // Otherwise, delete the atoms just created

               else
                    {
                    GlobalDeleteAtom (aApp) ;
                    GlobalDeleteAtom (aTop) ;
                    }

               return 0 ;

          case WM_TIMER:
          case WM_TIMECHANGE:
               time (&lSecSince1970) ;

                    // Calculate new current populations

               ldSecSince1970 = (long double) lSecSince1970 ;
               ldSecsInDecade = (long double) 3652 * 24 * 60 * 60 ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    {
                    pop[i].lPop = (long)
                         (((ldSecsInDecade - ldSecSince1970) * pop[i].lPop70 +
                            ldSecSince1970 * pop[i].lPop80) / ldSecsInDecade
                                   + .5) ;
                    }

                    // Notify all child windows

               EnumChildWindows (hwnd, lpTimerEnumProc, 0L) ;
               return 0 ;

          case WM_QUERYOPEN:
               return 0 ;

          case WM_CLOSE:

                    // Notify all child windows

               EnumChildWindows (hwnd, lpCloseEnumProc, 0L) ;

               break ;                  // for default processing

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

long FAR PASCAL ServerProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     ATOM          aItem ;
     char          szItem [10], szPopulation [10] ;
     DDEACK        DdeAck ;
     DDEADVISE     Advise ;
     DDEADVISE FAR *lpDdeAdvise ;
     DDEDATA FAR   *lpDdeData ;
     DWORD         dwTime ;
     GLOBALHANDLE  hPopAdvise, hDdeData, hDdeAdvise, hCommands, hDdePoke ;
     int           i ;
     HWND          hwndClient ;
     MSG           msg ;
     POPADVISE FAR *lpPopAdvise ;
     WORD          cfFormat, wStatus ;

     switch (message)
          {
          case WM_CREATE:

                    // Allocate memory for POPADVISE structures

               hPopAdvise = GlobalAlloc (GHND, NUM_STATES * sizeof (POPADVISE));

               if (hPopAdvise == NULL)
                    DestroyWindow (hwnd) ;
               else
                    SetWindowWord (hwnd, 2, hPopAdvise) ;

               return 0 ;

          case WM_DDE_REQUEST:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- data format
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               cfFormat   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

                    // Check for matching format and data item

               if (cfFormat == CF_TEXT)
                    {
                    GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                    for (i = 0 ; i < NUM_STATES ; i++)
                         if (strcmp (szItem, pop[i].szState) == 0)
                              break ;

                    if (i < NUM_STATES)
                         {
                         GlobalDeleteAtom (aItem) ;
                         PostDataMessage (hwnd, hwndClient, i,
                                          FALSE, FALSE, TRUE) ;
                         return 0 ;
                         }
                    }

                    // Negative acknowledge if no match

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_ADVISE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- DDEADVISE memory handle
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               hDdeAdvise = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               lpDdeAdvise = (DDEADVISE FAR *) GlobalLock (hDdeAdvise) ;

                    // Check for matching format and data item

               if (lpDdeAdvise->cfFormat == CF_TEXT)
                    {
                    GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                    for (i = 0 ; i < NUM_STATES ; i++)
                         if (strcmp (szItem, pop[i].szState) == 0)
                              break ;

                         // Fill in the POPADVISE structure and acknowledge

                    if (i < NUM_STATES)
                         {
                         hPopAdvise = GetWindowWord (hwnd, 2) ;
                         lpPopAdvise = (POPADVISE FAR *)
                                              GlobalLock (hPopAdvise) ;

                         lpPopAdvise[i].fAdvise   = TRUE ;
                         lpPopAdvise[i].fDeferUpd = lpDdeAdvise->fDeferUpd ;
                         lpPopAdvise[i].fAckReq   = lpDdeAdvise->fAckReq ;
                         lpPopAdvise[i].lPopPrev  = pop[i].lPop ;

                         GlobalUnlock (hDdeAdvise) ;
                         GlobalFree (hDdeAdvise) ;

                         DdeAck.bAppReturnCode = 0 ;
                         DdeAck.reserved       = 0 ;
                         DdeAck.fBusy          = FALSE ;
                         DdeAck.fAck           = TRUE ;

                         wStatus = * (WORD *) & DdeAck ;

                         if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                           MAKELONG (wStatus, aItem)))
                              {
                              GlobalDeleteAtom (aItem) ;
                              }
                         else
                              {
                              PostDataMessage (hwnd, hwndClient, i,
                                               lpPopAdvise[i].fDeferUpd,
                                               lpPopAdvise[i].fAckReq,
                                               FALSE) ;
                              }

                         GlobalUnlock (hPopAdvise) ;
                         return 0 ;
                         }
                    }

                         // Otherwise post a negative WM_DDE_ACK

               GlobalUnlock (hDdeAdvise) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalFree (hDdeAdvise) ;
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_UNADVISE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- data format
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               cfFormat   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = TRUE ;

               hPopAdvise  = GetWindowWord (hwnd, 2) ;
               lpPopAdvise = (POPADVISE FAR *) GlobalLock (hPopAdvise) ;

                    // Check for matching format and data item

               if (cfFormat == CF_TEXT || cfFormat == 0)
                    {
                    if (aItem == NULL)
                         for (i = 0 ; i < NUM_STATES ; i++)
                              lpPopAdvise[i].fAdvise = FALSE ;
                    else
                         {
                         GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                         for (i = 0 ; i < NUM_STATES ; i++)
                              if (strcmp (szItem, pop[i].szState) == 0)
                                   break ;

                         if (i < NUM_STATES)
                              lpPopAdvise[i].fAdvise = FALSE ;
                         else
                              DdeAck.fAck = FALSE ;
                         }
                    }
               else
                    DdeAck.fAck = FALSE ;

                    // Acknowledge either positively or negatively

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    if (aItem != NULL)
                         GlobalDeleteAtom (aItem) ;
                    }

               GlobalUnlock (hPopAdvise) ;
               return 0 ;

          case WM_DDE_EXECUTE:

                    // Post negative acknowledge

               hwndClient = wParam ;
               hCommands  = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, hCommands)))
                    {
                    GlobalFree (hCommands) ;
                    }
               return 0 ;

          case WM_DDE_POKE:

                    // Post negative acknowledge

               hwndClient = wParam ;
               hDdePoke   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalFree (hDdePoke) ;
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_TERMINATE:

                    // Respond with another WM_DDE_TERMINATE message

               hwndClient = wParam ;
               PostMessage (hwndClient, WM_DDE_TERMINATE, hwnd, 0L) ;
               DestroyWindow (hwnd) ;
               return 0 ;

          case WM_TIMER:

                    // Post WM_DDE_DATA messages for changed populations

               hwndClient  = GetWindowWord (hwnd, 0) ;
               hPopAdvise  = GetWindowWord (hwnd, 2) ;
               lpPopAdvise = (POPADVISE FAR *) GlobalLock (hPopAdvise) ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    if (lpPopAdvise[i].fAdvise)
                         if (lpPopAdvise[i].lPopPrev != pop[i].lPop)
                              {
                              if (!PostDataMessage (hwnd, hwndClient, i,
                                                    lpPopAdvise[i].fDeferUpd,
                                                    lpPopAdvise[i].fAckReq,
                                                    FALSE))
                                   break ;

                              lpPopAdvise[i].lPopPrev = pop[i].lPop ;
                              }

               GlobalUnlock (hPopAdvise) ;
               return 0 ;

          case WM_CLOSE:

                    // Post a WM_DDE_TERMINATE message to the client

               hwndClient = GetWindowWord (hwnd, 0) ;
               PostMessage (hwndClient, WM_DDE_TERMINATE, hwnd, 0L) ;

               dwTime = GetCurrentTime () ;

               while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
                    if (PeekMessage (&msg, hwnd, WM_DDE_TERMINATE,
                                     WM_DDE_TERMINATE, PM_REMOVE))
                         break ;

               DestroyWindow (hwnd) ;
               return 0 ;

          case WM_DESTROY:
               hPopAdvise = GetWindowWord (hwnd, 2) ;
               GlobalFree (hPopAdvise) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

BOOL FAR PASCAL TimerEnumProc (HWND hwnd, LONG lParam)
     {
     SendMessage (hwnd, WM_TIMER, 0, 0L) ;

     return TRUE ;
     }

BOOL FAR PASCAL CloseEnumProc (HWND hwnd, LONG lParam)
     {
     SendMessage (hwnd, WM_CLOSE, 0, 0L) ;

     return TRUE ;
     }

BOOL PostDataMessage (HWND hwndServer, HWND hwndClient, int iState,
                      BOOL fDeferUpd, BOOL fAckReq, BOOL fResponse)
     {
     ATOM         aItem ;
     char         szPopulation [10] ;
     DDEACK       DdeAck ;
     DDEDATA FAR  *lpDdeData ;
     DWORD        dwTime ;
     GLOBALHANDLE hDdeData ;
     MSG          msg ;

     aItem = GlobalAddAtom (pop[iState].szState) ;

          // Allocate a DDEDATA structure if not defered update

     if (fDeferUpd)
          {
          hDdeData = NULL ;
          }
     else
          {
          wsprintf (szPopulation, "%ld\r\n", pop[iState].lPop) ;

          hDdeData = GlobalAlloc (GHND | GMEM_DDESHARE,
                                  sizeof (DDEDATA) + strlen (szPopulation)) ;

          lpDdeData = (DDEDATA FAR *) GlobalLock (hDdeData) ;

          lpDdeData->fResponse = fResponse ;
          lpDdeData->fRelease  = TRUE ;
          lpDdeData->fAckReq   = fAckReq ;
          lpDdeData->cfFormat  = CF_TEXT ;

          lstrcpy ((LPSTR) lpDdeData->Value, szPopulation) ;

          GlobalUnlock (hDdeData) ;
          }

          // Post the WM_DDE_DATA message

     if (!PostMessage (hwndClient, WM_DDE_DATA, hwndServer,
                       MAKELONG (hDdeData, aItem)))
          {
          if (hDdeData != NULL)
               GlobalFree (hDdeData) ;

          GlobalDeleteAtom (aItem) ;
          return FALSE ;
          }

          // Wait for the acknowledge message if it's requested

     if (fAckReq)
          {
          DdeAck.fAck = FALSE ;

          dwTime = GetCurrentTime () ;

          while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
               {
               if (PeekMessage (&msg, hwndServer, WM_DDE_ACK, WM_DDE_ACK,
                                PM_REMOVE))
                    {
                    DdeAck = * (DDEACK *) & LOWORD (msg.lParam) ;
                    aItem  = HIWORD (msg.lParam) ;
                    GlobalDeleteAtom (aItem) ;
                    break ;
                    }
               }

          if (DdeAck.fAck == FALSE)
               {
               if (hDdeData != NULL)
                    GlobalFree (hDdeData) ;

               return FALSE ;
               }
          }

     return TRUE ;
     }
