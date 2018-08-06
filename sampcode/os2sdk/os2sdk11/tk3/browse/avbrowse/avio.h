/*
    Define QueryLine function type
*/
typedef char * (_loadds *PFNQL) (USHORT);
/*
    Define linebuffer specifications
*/
typedef struct _LBINFO	{	/* lbi */
    SHORT	sPScols;	/* # cols in AVIO PS */
    SHORT	sPSrows;	/* # rows in AVIO PS */
    SHORT	sCols;		/* Actual # cols */
    SHORT	sRows;		/* Actual # rows */
    PFNQL	pfnQL;		/* Pointer to QueryLine function */
    BOOL	fLargeFont;	/* Whether or not... */
} LBINFO;
typedef LBINFO far *PLBINFO;	/* Far pointer to our information */

/*
    Avio function prototypes
*/
void	AvioInit(PLBINFO plbi);
void	AvioScroll(USHORT SB_Command, USHORT Position, BOOL Horizontal);
MRESULT	AvioSize(HWND hWnd, USHORT msg, MPARAM mp1, MPARAM mp2);
void	AvioClose(void);
void	AvioPaint(HWND hWnd);
MRESULT	AvioMinMax(PSWP pSWP);
void	AvioClear(void);
void	AvioAdjustFramePos(PSWP pSWP);
void	AvioTrackFrame(HWND hWnd, MPARAM TrackFlags);
void	AvioQueryTrackInfo(PTRACKINFO pTI);
void	AvioRedraw(void);
void	AvioStartup(BOOL fLargeFont);
void	AvioLargeFont(BOOL fLargeFont);
MRESULT	CALLBACK AvioFrameWndProc(HWND, USHORT, MPARAM, MPARAM);
MRESULT CALLBACK AvioClientWndProc(HWND, USHORT, MPARAM, MPARAM);
