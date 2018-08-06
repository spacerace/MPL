/*****************************************************************

    ENVIRONMENT.H 

*****************************************************************/

  
PUBLIC BOOL	env_InitEnvironment		(VOID);
PUBLIC BOOL	env_PtInRect			(Point *,pRect);
PUBLIC VOID	env_OffsetRect			(Rect *,int,int);

PUBLIC VOID	env_FatalError			(/*varargs*/);
PUBLIC VOID	env_MessageToUser		(/*varargs*/);
PUBLIC VOID	env_ExitProgram			(/*varargs*/);
PUBLIC VOID	env_NotImplemented		(/*varargs*/);

#ifdef MAC
PUBLIC VOID	memset					(MEMPTR,int,int);
#endif

/****************************************************************
    MEMORY ALLOCATION
****************************************************************/

PUBLIC MEMBLOCK	env_AllocMem		(MEMSIZE);
PUBLIC VOID		env_ReAllocMem		(MEMBLOCK *,MEMSIZE);
PUBLIC MEMPTR	env_GraspMem		(MEMBLOCK);
PUBLIC VOID		env_UnGraspMem		(MEMBLOCK);
PUBLIC VOID		env_FreeMem			(MEMBLOCK);


/****************************************************************
    PATTERNS
****************************************************************/

typedef enum {
	PATT_WHITE,
	PATT_BLACK,
	PATT_GRAY,
	PATT_LTGRAY,
	PATT_DKGRAY,
	PATT_FWD,
	PATT_BACK,
	PATT_HORL,
	PATT_VERT,
	PATT_VERTX,
	PATT_DIAGX,
	PATT_NULLPA,
	PATT_DOT,
	PATT_DASH,
	PATT_DASHDO,
	PATT_DASHDD
} PATT;


/****************************************************************
    WINDOWS, CANVASES and DRAWING
****************************************************************/

PUBLIC BOOL		env_NewWindow		(STRING,Rect *,INDEX,INDEX);
PUBLIC VOID		env_GetScreenSize	(Rect *);

#ifdef WM_CREATE /*WINDOWS.H*/
#ifndef MAC
LOCALF VOID		select_fill_pattern	(HDC,PATT);
LOCALF VOID		null_brush			(HDC);
LOCALF VOID		null_pen			(HDC);
#endif
#endif
#ifdef MAC
PUBLIC BOOL		env_IsDevData		(LONG);
PUBLIC MEMPTR	env_GetAddrDevBlock	(HCANVAS);
#endif

LOCALF VOID		env_IsCanvasValid	(HCANVAS);

PUBLIC VOID		env_InitCanvases	(VOID);
PUBLIC HCANVAS	env_NewCanvas		(LONG);
PUBLIC VOID		env_FreeCanvas		(HCANVAS);
PUBLIC VOID		env_SetCanvasDev	(HCANVAS,LONG);
PUBLIC LONG		env_GetCanvasDev	(HCANVAS);

PUBLIC VOID		env_ActivateCanvas	(HCANVAS);
PUBLIC VOID		env_InvalidateCanvas(HCANVAS,Rect *);

PUBLIC VOID		env_MoveRel			(HCANVAS,short,short);
PUBLIC VOID		env_Line			(HCANVAS,short,short);
PUBLIC VOID		env_MoveTo			(HCANVAS,short,short);

PUBLIC VOID		env_ResizeCanvas	(HCANVAS,Point *);
PUBLIC VOID		env_MoveCanvas		(HCANVAS,Point *);
PUBLIC VOID		env_CalcSizeCanvas	(HCANVAS);

PUBLIC VOID		env_StartDrawing	(HCANVAS);
PUBLIC VOID		env_EndDrawing		(HCANVAS);
PUBLIC VOID		env_GlobalToLocal	(HCANVAS,Point *);
PUBLIC VOID		env_OffsetOrgCanvas	(HCANVAS,short,short);
PUBLIC VOID		env_SetScaleCanvas	(HCANVAS,short,short,short);

PUBLIC VOID		env_BeginUpdate		(HCANVAS);
PUBLIC VOID		env_EndUpdate		(HCANVAS);
PUBLIC VOID		env_FrameRect		(HCANVAS,Rect *);
PUBLIC VOID		env_PaintRect		(HCANVAS,Rect *);
PUBLIC VOID		env_FillRect		(HCANVAS,Rect *,PATT);

PUBLIC VOID		env_TextOut		(HCANVAS,PIXEL,PIXEL,STRING);

/****************************************************************
    APPLICATION EVENTS
****************************************************************/    

/*return values for GetNextEvent() */
enum 
{
	NULL_EVENT,
	QUIT_EVENT,
	CMD_EVENT,
	MOUSEDOWN_EVENT,
	MOUSEUP_EVENT,
	OPENVIEW_EVENT,
	CLOSEVIEW_EVENT,
	MOVEVIEW_EVENT,
	RESIZEVIEW_EVENT,
	ACTIVATEVIEW_EVENT,
	UPDATEVIEW_EVENT,
	CHAR_EVENT
};

typedef struct
{
	int			cmd_category;
	int			cmd_item;
	HCANVAS		canvas;
	Point		where;
	int			the_char;
	int		event_type;
	long		when;
} APPEVENT;

PUBLIC VOID	env_GetAppEvent		(APPEVENT *);
#ifdef MAC
LOCALF BOOL	mac_EventLoop		(APPEVENT *);
#endif

/****************************************************************
    MENUS
****************************************************************/    

PUBLIC BOOL	env_InitMenus		(VOID);
PUBLIC VOID	env_DoSysMenu		(ENUM);


/****************************************************************
    DEBUGGING
****************************************************************/

#ifdef DBG 
#ifdef MAC
PUBLIC BOOL 	ut_InitDebugModule		();
PUBLIC BOOL		ut_CreateDebugMenu		();
PUBLIC BOOL 	ut_FilterMenuSelection	();
#else
#ifdef WM_CREATE
PUBLIC LONG WCALLBACK DebugWndProc		(HWND,int,WORD,long);
PUBLIC INT	ut_CreateDBugWindow		(HWND,int);
PUBLIC INT	ut_RegisterDBugWindow		(HANDLE,int);
PUBLIC BOOL 	ut_DebugMenuFilter		(int);
#endif
#endif

PUBLIC VOID	ut_SetPrintLevel		(int);
PUBLIC VOID	ut_PutLine			(STRING);
PUBLIC VOID	ut_RawTextOut			(short,short,STRING);
PUBLIC VOID 	ut_PrintMacRect			(STRING,Rect *);
PUBLIC VOID 	ut_PrintShort			(STRING,short);
PUBLIC VOID 	ut_Print2Shorts			(STRING,short,short);
PUBLIC VOID 	ut_Print4Shorts			(STRING,short,short,short,short);
PUBLIC VOID	ut_PutString			(STRING,STRING);
PUBLIC VOID	ut_PrintHex			(STRING,LONG);
PUBLIC VOID	ut_Print2Hex			(STRING,LONG,LONG);
PUBLIC VOID	ut_Print4Hex			(STRING,LONG,LONG,LONG,LONG);

#ifdef WM_CREATE
int   far PASCAL OpenPathname	(LPSTR, int );
int   far PASCAL DeletePathname	(LPSTR );
int   far PASCAL _lopen		(LPSTR, int );
void  far PASCAL _lclose	(int );
int   far PASCAL _lcreat	(LPSTR, int );
BOOL  far PASCAL _ldelete	(LPSTR );
WORD  far PASCAL _ldup		(int );
LONG  far PASCAL _llseek	(int, long, int );
WORD  far PASCAL _lread		(int, LPSTR, int );
WORD  far PASCAL _lwrite	(int, LPSTR, int );

#define READ        0   /* Flags for _lopen */
#define WRITE       1
#define READ_WRITE  2

#endif

#else /*!DBG*/

#define ut_SetPrintLevel(a)
#define ut_PutLine(a)
#define ut_RawTextOut(a,b,c)
#define ut_PrintMacRect(a,b)
#define ut_PrintShort(a,b)
#define ut_Print2Shorts(a,b,c)
#define ut_Print4Shorts(a,b,c,d,e)
#define ut_PutString(a,b)
#define ut_PrintHex(a,b)
#define ut_Print2Hex(a,b,c)
#define ut_Print4Hex(a,b,c,d,e)

#endif 


