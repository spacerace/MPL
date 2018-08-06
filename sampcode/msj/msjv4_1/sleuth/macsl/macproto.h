/*-----------------------------------------------------------------*/
/* MacPrototypes.h                                                 */
/* Lightspeed C, for some strange reason, fails to provide         */
/* prototypes for the Mac toolbox functions, so here we have       */
/* the prototypes for the fucntions we use.                        */
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* Mac menu bar data structure, missing from Mac .h files          */
/*-----------------------------------------------------------------*/

typedef struct {
    MenuHandle  hmenu;
    SHORT       sHorzCoord;
} MlistItem;

typedef struct {
    USHORT      usMlistSize;
    SHORT       sHorzSize;
    USHORT      usUnused;
    MlistItem   mlist[1];
} Mbar, *MbarPtr, **MbarHandle;

#define MENUCOUNTOFHMBAR( hmbar )  \
    ( (**(hmbar)).usMlistSize / sizeof(MlistItem) )

/*-----------------------------------------------------------------*/
/* Function prototypes                                             */
/*-----------------------------------------------------------------*/

#define P pascal

P void        AddResMenu( MenuHandle hmenu, ResType restype );
P void        BeginUpdate( WindowPeek pwin );
P void        CopyRgn( RgnHandle hrgnSrc, RgnHandle hrgnDest );
P void        DiffRgn( RgnHandle hrgnSrcA, RgnHandle hrgnSrcB,
                       RgnHandle hrgnDest );
P void        DisposeWindow( WindowPeek pwin );
P void        DisposHandle( Handle hMem );
P void        DisposeRgn( RgnHandle hrgn );
P long        DragGrayRgn( RgnHandle hrgn, Point point,
                           Rect* prectBounds, Rect* prectSlop,
                           short sAxis, ProcPtr pfnAction );
P void        DragWindow( WindowPeek pwin, Point point,
                          Rect* prect );
P void        DrawControls( WindowPeek pwin );
P void        DrawGrowIcon( WindowPeek pwin );
P void        DrawMenuBar( void );
P void        DrawText( Ptr pch, short sFirst, short sLen );
P void        EndUpdate( WindowPeek pwin );
P void        EraseRect( Rect* prect );
P Boolean     EventAvail( short sMask, EventRecord* pEvent );
P void        ExitToShell( void );
P int         FindControl( Point point, WindowPeek pwin,
                           ControlHandle* hctl );
P short       FindWindow( Point point, WindowPeek* ppwin );
P void        FlushEvents( short sWhichMask, short sStopMask );
P FMOutPtr    FMSwapFont( FMInput* pfmi );
P WindowPtr   FrontWindow( void );
P long        GetCaretTime( void );
P long        GetDblTime( void );
P void        GetIndString( char* pstr, short id, short index );
P void        GetItem( MenuHandle hmenu, short item, char* pstr );
P void        GetMouse( Point* ppoint );
P Boolean     GetNextEvent( short sMask, EventRecord* pEvent );
P Handle      GetNewMBar( short idBar );
P Handle      GetResource( ResType restype, short id );
P void        GetWMgrPort( GrafPtr* pgraf );
P void        GetWTitle( WindowPeek pwin, char* pstr );
P long        GrowWindow( WindowPeek pwin, Point point,
                          Rect* prect );
P void        HideWindow( WindowPeek pwin );
P void        HiliteMenu( short idMenu );
P void        InitCursor( void );
P void        InitDialogs( ProcPtr resumeProc );
P void        InitFonts( void );
P void        InitGraf( GrafPtr* pGlobals );
P void        InitMenus( void );
P void        InitWindows( void );
P void        InsetRgn( RgnHandle hrgn, short dh, short dv );
P void        InvalRect( Rect* prect );
P void        MaxApplZone( void );
P long        MenuSelect( Point point );
P void        MoreMasters( void );
P void        MoveTo( short h, short v );
P void        MoveWindow( WindowPeek pwin, short h, short v,
                          Boolean fFront );
P ControlHandle NewControl( WindowPeek pwin, Rect* prectBounds,
                            char* pstrTitle, Boolean fVisible,
                            short sValue, short sMin, short sMax,
                            short sProcID, long lRefCon );
P Handle      NewHandle( Size lBytes );
P Ptr         NewPtr( Size lBytes );
P RgnHandle   NewRgn( void );
P WindowPtr   NewWindow( Ptr pWinBuf, Rect* prectBounds,
                         char* pstrTitle, Boolean fVisible,
                         short sProcID, WindowPeek pwinBehind,
                         Boolean fGoAway, long lRefCon );
P OsErr       PostEvent( int iEvent, long lMsg );
P short       OpenDeskAcc( char* pstrName );
P short       OpenResFile( char* pstrFileName );
P Boolean     PtInRect( Point point, Rect* prect );
P void        RectRgn( RgnHandle hrgn, Rect* prect );
P void        SectRgn( RgnHandle hrgnSrcA, RgnHandle hrgnSrcB,
                       RgnHandle hrgnDest );
P void        SelectWindow( WindowPeek pwin );
P void        SetEventMask( short sMask );
P void        SetMenuBar( MbarHandle hmbar );
P void        SetRectRgn( RgnHandle hrgn, int left, int top,
                          int right, int bottom );
P void        SetWTitle( WindowPeek pwin, char* pstr );
P void        ShowHide( WindowPeek pwin, Boolean fShow );
P void        ShowWindow( WindowPeek pwin );
P void        SizeWindow( WindowPeek pwin, short h, short v,
                          Boolean fUpdate );
P void        SystemClick( EventRecord* pEvent, WindowPeek pwin );
P void        SystemTask( void );
P void        TEInit( void );
P Boolean     TrackBox( WindowPeek pwin, Point point, short sPart );
P int         TrackControl( ControlHandle hctl, Point point,
                            ProcPtr pfnAction );
P Boolean     TrackGoAway( WindowPeek pwin, Point point );
P void        UpdtControl( WindowPeek pwin, RgnHandle hrgn );
P void        ZoomWindow( WindowPeek pwin, short sPart,
                          Boolean fFront );

#undef P

/*-----------------------------------------------------------------*/
