/*-----------------------------------------------------------------*/
/* MacPM.h                                                         */
/* Header file for all MacPM internal modules.                     */
/* Precompile this into "MacPM.h" before compiling the C files.    */
/*-----------------------------------------------------------------*/

#include <MacHeaders>

#include <string.h>
#include <stdio.h>

#define memzero( pDest )  memset( (pDest), 0, sizeof(*(pDest)) )

/* OS/2 stuff */

#define MACPM_INTERNAL

#include "Os2h.h"

#define MPM_DEBUG 1

/* Stuff specific to Lightspeed C */

#include "MacPrototypes.h"

/*-----------------------------------------------------------------*/
/* Stuff for DosXXXXX functions                                    */
/*-----------------------------------------------------------------*/

#define MY_PID  9  /* This is a hack - there is only one PID */

/*-----------------------------------------------------------------*/
/* temp hacks                                                      */
/*-----------------------------------------------------------------*/

char * strncpy( char *, char *, short );

/*-----------------------------------------------------------------*/
/* Define our two special Mac events                               */
/*-----------------------------------------------------------------*/

#define postedEvt     app1Evt  /* WinPostMsg event */
#define mouseMove     app2Evt  /* (not used at this time) */

/*-----------------------------------------------------------------*/
/* Any of these bits force frame window to use documentProc        */
/*-----------------------------------------------------------------*/

#define FCF_DOCBITS  \
    ( FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_SIZEBORDER |  \
      FCF_HORZSCROLL | FCF_VERTSCROLL )

/*-----------------------------------------------------------------*/
/* "nano-PS" data structure.  Not much here, other than a way to   */
/* get to the Mac's GrafPort.                                      */
/*-----------------------------------------------------------------*/

#define PSF_INUSE       0x0001

typedef struct _PS {
    USHORT        signature;          /* Validation signature */
    USHORT        flags;              /* PSF_ flags */
    HWND          hwnd;               /* Owning window handle */
    GrafPort      port;               /* Mac's port structure */
} PS;

typedef PS * PPS;

#define PPSOFHPS( hps )  (  *(PPS *)(hps) )
#define PSOFHPS( hps )  (  *PPSOFHPS(hps) )

#define HWNDOFHPS( hps )  ( PSOFHPS(hps).hwnd )
#define GRAFOFHPS( hps )  ( PSOFHPS(hps).port )
#define PGRAFOFHPS( hps )  ( &GRAFOFHPS(hps) )

/*-----------------------------------------------------------------*/
/* Window class structure                                          */
/*-----------------------------------------------------------------*/

#define WC_DESKTOP    ( (PSZ)0xFFFF8088L )

typedef struct _MYCLASS {
    USHORT            usHash;         /* Hash of name */
    struct _MYCLASS **hNextClass;     /* handle to next MYCLASS */
    CLASSINFO         class;          /* PM class structure */
    CHAR              szName[1];      /* Name text begins here */
} MYCLASS;

typedef MYCLASS  *    PMYCLASS;       /* pcl */
typedef PMYCLASS *    HMYCLASS;       /* hcl */

#define CLASSWINDOWBITS  \
    ( CS_CLIPCHILDREN & CS_CLIPSIBLINGS & CS_PARENTCLIP & CS_SAVEBITS )

/*-----------------------------------------------------------------*/
/* Window structure                                                */
/* WK_ definitions are order dependent!                            */
/*-----------------------------------------------------------------*/

#define WK_OBJECT       1             /* Object window */
#define WK_DESKTOP      2             /* Desktop window */
#define WK_MAIN         3             /* Top level (Mac) window */
#define WK_CHILD        4             /* Child window */

typedef struct _MYWND {
    /* MacPM private data */
    USHORT        signature;          /* Validation signature */
    UCHAR         ucKind;             /* WK_ value */
    UCHAR         ucFiller1;          /* (fill to word boundary) */
    /* PM-style data */
    PFNWP         pfnwp;              /* Window function pointer */
    HMYCLASS      hclass;             /* MYCLASS handle */
    ULONG         flStyle;            /* Window style */
    USHORT        id;                 /* Window ID */
    HWND          hwndOwner;          /* Various kin of the window */
    HWND          hwndParent;
    HWND          hwndTopChild;
    HWND          hwndBottomChild;
    HWND          hwndPrevSibling;
    HWND          hwndNextSibling;
    SHORT         x;                  /* coords relative to parent */
    SHORT         y;
    SHORT         cx;
    SHORT         cy;
    ULONG         flFrameFlags;       /* Should be in frame "extra"! */
    /* Mac-style data */
    WindowPeek    pwin;               /* Mac window */
    ControlHandle hctl;               /* Mac control handle */
    Rect          rectAdj;            /* Mac client area adjustment */
    /* Application data */
    BYTE          bExtra[1];          /* window "extra" data */
} MYWND;

typedef MYWND *PMYWND;

#define PMYWNDOF( hwnd )  ( *(hwnd) )
#define MYWNDOF( hwnd )   ( *PMYWNDOF(hwnd) )

#define HMYCLASSOF( hwnd )  ( MYWNDOF(hwnd).hclass )
#define PMYCLASSOF( hwnd )  ( *HMYCLASSOF(hwnd) )
#define MYCLASSOF( hwnd )   ( *PMYCLASSOF(hwnd) )

#define WND_SIGNATURE   0x4D47

#define ISDESKTOPWINDOW( hwnd )  \
    ( MYWNDOF(hwnd).ucKind == WK_DESKTOP )

#define ISOBJECTWINDOW( hwnd )  \
    ( MYWNDOF(hwnd).ucKind == WK_OBJECT )

#define ISCHILDWINDOW( hwnd )  \
    ( MYWNDOF(hwnd).ucKind == WK_CHILD )

#define ISMAINWINDOW( hwnd )  \
    ( MYWNDOF(hwnd).ucKind == WK_MAIN )

#define HWNDOFPWIN( pwin )  ( (HWND)(/*(WindowPeek)*/(pwin))->refCon )
#define PWINOFHWND( hwnd )  ( MYWNDOF(hwnd).pwin )

#define MAINHWND( hwnd )  \
    ( PWINOFHWND(hwnd) ? HWNDOFPWIN(PWINOFHWND(hwnd)) : (hwnd) )

/*-----------------------------------------------------------------*/
/* Definitions for frame controls                                  */
/*-----------------------------------------------------------------*/

#define FID_CTL_MIN   FID_SIZEBORDER      /* 0x8001 */
#define FID_CTL_MAX   FID_CLIENT          /* 0x8008 */

#define FID_CTL_COUNT ( FID_CTL_MAX - FID_CTL_MIN + 1 )
#define FID_INDEX( fid ) ( (fid) - FID_CTL_MIN )

#define I_SIZEBORDER    ( FID_SIZEBORDER  - FID_CTL_MIN )
#define I_SYSMENU       ( FID_SYSMENU     - FID_CTL_MIN )
#define I_TITLEBAR      ( FID_TITLEBAR    - FID_CTL_MIN )
#define I_MINMAX        ( FID_MINMAX      - FID_CTL_MIN )
#define I_MENU          ( FID_MENU        - FID_CTL_MIN )
#define I_VERTSCROLL    ( FID_VERTSCROLL  - FID_CTL_MIN )
#define I_HORZSCROLL    ( FID_HORZSCROLL  - FID_CTL_MIN )
#define I_CLIENT        ( FID_CLIENT      - FID_CTL_MIN )

/*-----------------------------------------------------------------*/
/* Debugging stuff                                                 */
/*-----------------------------------------------------------------*/

#ifdef MPM_DEBUG

#define ERROR( pszMsg )  MpmErrorBox( pszMsg )

#define ASSERT( expr, pszMsg )  if( ! (expr) )  ERROR( pszMsg );

#else

#define ERROR( pszMsg )

#define ASSERT( expr, pszMsg )

#endif

/*-----------------------------------------------------------------*/
/* Global variables                                                */
/*-----------------------------------------------------------------*/

#ifndef _EXTERN
#define _EXTERN extern
#endif

_EXTERN CHAR      _szNull[1];

_EXTERN LONG      _alSysVal[SV_CSYSVALUES];

_EXTERN SHORT     _sSetFocusDepth;

_EXTERN HWND      _hwndObject;
_EXTERN HWND      _hwndDesktop;
_EXTERN HWND      _hwndActive;
_EXTERN HWND      _hwndFocus;
_EXTERN HWND      _hwndMenu;

_EXTERN PS        _ps1;
_EXTERN PPS       _pps1;
_EXTERN HPS       _hps1;

/*-----------------------------------------------------------------*/
/* Function prototypes for internal functions                      */
/*-----------------------------------------------------------------*/

#define P pascal

#define LOCAL

#define WinMapWindowRect( hwndFrom, hwndTo, prcl )  \
    WinMapWindowPoints( (hwndFrom), (hwndTo), (PPOINTL)(prcl), 2 )

LOCAL VOID      MpmActivateScrollBars( HWND hwnd, BOOL fActive );
LOCAL VOID      MpmDrawGrowIcon( HWND hwnd );
LOCAL VOID      MpmErrorBox( PSZ pszMsg );
LOCAL BOOL      MpmIsFrameClass( PSZ pszClassName );
LOCAL VOID      MpmMapAbsOfWin( HWND hwnd, PPOINTL pptl,
                                  SHORT sCount );
LOCAL VOID      MpmMapMacOfPtl( HWND hwnd, Point* ppoint,
                                  PPOINTL pptl );
LOCAL VOID      MpmMapMacOfRcl( HWND hwnd, Rect* prect,
                                  PRECTL prcl );
LOCAL VOID      MpmMapPtlOfMac( HWND hwnd, PPOINTL pptl,
                                  Point* ppoint );
LOCAL VOID      MpmMapRclOfMac( HWND hwnd, PRECTL prcl,
                                  Rect* prectl );
LOCAL VOID      MpmMapWinOfAbs( HWND hwnd, PPOINTL pptl,
                                  SHORT sCount );
LOCAL BOOL      MpmMenuLoad( HWND hwndFrame, USHORT idResources );
LOCAL PVOID     MpmNewHandleZ( USHORT usLen );
LOCAL PVOID     MpmNewPtrZ( USHORT usLen );
LOCAL VOID      MpmQueryMacRect( HWND hwnd, Rect* prect );
LOCAL BOOL      MpmValidateWindow( HWND hwnd );
LOCAL BOOL      MpmValidatePS( HPS hps );
LOCAL BOOL      MpmValidateWindowNull( HWND hwnd );

#define DCLFNWP( fnwp )  \
    MRESULT EXPENTRY fnwp( HWND, USHORT, MPARAM, MPARAM );

DCLFNWP( MpmFnwpButton );
DCLFNWP( MpmFnwpDesktop );
DCLFNWP( MpmFnwpDialog );
DCLFNWP( MpmFnwpEntryField );
DCLFNWP( MpmFnwpFrame );
DCLFNWP( MpmFnwpListBox );
DCLFNWP( MpmFnwpMenu );
DCLFNWP( MpmFnwpObject );
DCLFNWP( MpmFnwpScrollBar );
DCLFNWP( MpmFnwpSizeBorder );
DCLFNWP( MpmFnwpStatic );
DCLFNWP( MpmFnwpTitleBar );

#undef P

/*-----------------------------------------------------------------*/
