/*------------------------------------------------
   STRLIB.C -- Library module for STRPROG program
               (c) Charles Petzold,  1990
  ------------------------------------------------*/

#include <windows.h>

typedef BOOL FAR PASCAL GETSTR (LPSTR, LPSTR) ;
HANDLE hStrings [256] ;
short  nTotal = 0 ;

int FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
     {
     if (wHeapSize > 0)
          UnlockData (0) ;

     return 1 ;
     }

BOOL FAR PASCAL AddString (LPSTR lpStringIn)
     {
     HANDLE hString ;
     NPSTR  npString ;
     short  i, nLength, nCompare ;

     if (nTotal == 255)
          return FALSE ;

     if (0 == (nLength = lstrlen (lpStringIn)))
          return FALSE ;

     if (NULL == (hString = LocalAlloc (LHND, 1 + nLength)))
          return FALSE ;

     npString = LocalLock (hString) ;
     lstrcpy (npString, lpStringIn) ;
     AnsiUpper (npString) ;
     LocalUnlock (hString) ;

     for (i = nTotal ; i > 0 ; i--)
          {
          npString = LocalLock (hStrings [i - 1]) ;
          nCompare = lstrcmpi (lpStringIn, npString) ;
          LocalUnlock (hStrings [i - 1]) ;

          if (nCompare > 0)
               {
               hStrings [i] = hString ;
               break ;
               }
          hStrings [i] = hStrings [i - 1] ;
          }

     if (i == 0)
          hStrings [0] = hString ;

     nTotal++ ;
     return TRUE ;
     }

BOOL FAR PASCAL DeleteString (LPSTR lpStringIn)
     {
     NPSTR npString ;
     short i, j, nCompare ;

     if (0 == lstrlen (lpStringIn))
          return FALSE ;

     for (i = 0 ; i < nTotal ; i++)
          {
          npString = LocalLock (hStrings [i]) ;
          nCompare = lstrcmpi (npString, lpStringIn) ;
          LocalUnlock (hStrings [i]) ;

          if (nCompare == 0)
               break ;
          }

     if (i == nTotal)
          return FALSE ;

     for (j = i ; j < nTotal ; j++)
          hStrings [j] = hStrings [j + 1] ;

     nTotal-- ;
     return TRUE ;
     }

short FAR PASCAL GetStrings (GETSTR lpfnGetStrCallBack, LPSTR lpParam)
     {
     BOOL  bReturn ;
     NPSTR npString ;
     short i ;

     for (i = 0 ; i < nTotal ; i++)
          {
          npString = LocalLock (hStrings [i]) ;
          bReturn = (*lpfnGetStrCallBack) ((LPSTR) npString, lpParam) ;
          LocalUnlock (hStrings [i]) ;

          if (bReturn == FALSE)
               return i + 1 ;
          }
     return nTotal ;
     }
