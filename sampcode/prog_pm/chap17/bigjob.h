/*----------------------
   BIGJOB.H header file
  ----------------------*/

#define ID_RESOURCE 1

#define IDM_REPS    1
#define IDM_ACTION  2
#define IDM_10      10
#define IDM_100     11
#define IDM_1000    12
#define IDM_10000   13
#define IDM_100000  14
#define IDM_START   20
#define IDM_ABORT   21

#define STATUS_READY     0
#define STATUS_WORKING   1
#define STATUS_DONE      2

#define WM_CALC_DONE     (WM_USER + 0)  // Used in BIGJOB4 and BIGJOB5
#define WM_CALC_ABORTED  (WM_USER + 1)

#define STACKSIZE   4096                // Used in BIGJOB4 and BIGJOB5

typedef struct                          // Used in BIGJOB4 and BIGJOB5
     {
     HWND  hwnd ;
     LONG  lCalcRep ;
     BOOL  fContinueCalc ;
     ULONG ulSemTrigger ;               // Used in BIGJOB5
     }
     CALCPARAM ;

typedef CALCPARAM FAR *PCALCPARAM ;

double  Savage (double A) ;
VOID    CheckMenuItem (HWND hwnd, SHORT sMenuItem, BOOL fCheck) ;
VOID    EnableMenuItem (HWND hwnd, SHORT sMenuItem, BOOL fEnable) ;
VOID    PaintWindow (HWND hwnd, SHORT sStatus, LONG lCalcRep, ULONG ulTime) ;
MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
