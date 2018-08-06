/*****************************************************************
    APPLICATION.H
*****************************************************************/


/*values for viewtype*/
#define GRAPHICS_VIEW	1
#define TEXT_VIEW		2


#define DEPTH_RECT(r)	((r)->bottom - (r)->top)

typedef int PIXEL;

/****************************************************************/

#ifdef MAC
typedef Rect *		pRect;
typedef char *		MEMPTR;
typedef MEMPTR *	MEMBLOCK;

#else
#ifndef _STDDEFS_
#ifndef WM_CREATE
typedef short		HANDLE;
#endif
#endif

typedef Rect far *	pRect;
typedef char far *	MEMPTR;
typedef HANDLE 		MEMBLOCK;
#endif
typedef long		MEMSIZE;
typedef int 		HCANVAS;


/****************************************************************/

PUBLIC VOID	app_MainProcedure	(VOID);
LOCALF BOOL	app_InitApplication	(VOID);
LOCALF BOOL	app_EventLoop		(VOID);

/****************************************************************
    OBJECT MANAGER
****************************************************************/

	/*values for objtype*/
enum {	RECT_OBJ, OVAL_OBJ };

	/*values for objattr*/
enum {	FRAME_ATTR, FILL_ATTR, FILLANDFRAME_ATTR };

#define INVALID_OBJID	((OBJID)  -1)

typedef struct
{
	Rect	rect;
	int	objtype;
	int	objattr;
} OBJ;

#ifdef MAC
typedef OBJ *		POBJ;
#else
typedef OBJ far *	POBJ;
#endif

typedef int OBJID;

PUBLIC VOID	obj_HighlightObjAsGraphics	(HCANVAS,POBJ);
PUBLIC VOID	obj_ShowObjAsGraphics		(HCANVAS,POBJ);
PUBLIC VOID	obj_MakeObject				(POBJ,Rect *,ENUM,ENUM);
PUBLIC VOID	obj_ShowObjAsText			(HCANVAS,PIXEL,PIXEL,POBJ);

/****************************************************************
    MODEL MANAGER
*****************************************************************/

typedef struct
{
	int		num_objects;
	OBJ		objects[1]; /*variable length*/
} MODEL;

typedef MEMBLOCK	HMODEL;	/*handle to model is handle to memory block*/

#ifdef MAC
typedef MODEL *		PMODEL; /*ptr to model is ptr to mem block */
#else
typedef MODEL far *	PMODEL; /*ptr to model is ptr to mem block */
#endif

#define INVALID_MODEL	((HMODEL) 0)

PUBLIC HMODEL	mod_NewModel				(VOID);
PUBLIC VOID		mod_HighlightObjAsGraphics	(HCANVAS,HMODEL,OBJID);
PUBLIC VOID		mod_ShowObjAsGraphics		(HCANVAS,HMODEL,OBJID);
PUBLIC VOID		mod_ShowModelAsGraphics		(HCANVAS,HMODEL);
PUBLIC OBJID	mod_DetectHitInGraphicsMode	(HMODEL,Point *);

PUBLIC OBJID	mod_AddObjectToModel(HMODEL *,Rect *, ENUM,ENUM);
PUBLIC VOID		mod_ShowModelAsText(HCANVAS,HMODEL,Rect *,PIXEL,PIXEL);
PUBLIC VOID		mod_ShowObjAsText(HCANVAS,HMODEL,OBJID,Rect *,PIXEL,PIXEL);
PUBLIC OBJID	mod_DetectHitInTextModel(HMODEL,Point *,Rect *,PIXEL,PIXEL);

/****************************************************************
    DOCUMENT MANAGER
*****************************************************************/

#define MAX_DOCUMENTS 	4
#define MAX_VIEWS 	8
#define VIEWS_PER_DOC	4

typedef struct
{
	BOOL		in_use;
	BOOL		is_modified;
	INDEX		views[VIEWS_PER_DOC];
	HMODEL		model;

	/*FILENAME	filename;--for the future*/
} DOC;
	

PUBLIC BOOL		doc_InitDocuments			(VOID);
LOCALF INDEX	allocate_document			(VOID);
PUBLIC INDEX	doc_NewDocument				(VOID);
PUBLIC BOOL		doc_AddViewToDoc			(INDEX,INDEX);
PUBLIC OBJID	doc_AddObjectToDocument		(INDEX,Rect *,ENUM,ENUM);
PUBLIC VOID		doc_ShowObjAsGraphics		(HCANVAS,INDEX,OBJID);
PUBLIC VOID		doc_HighlightObjAsGraphics	(HCANVAS,INDEX,OBJID);
PUBLIC VOID		doc_ShowModelAsGraphics		(HCANVAS,INDEX);
PUBLIC VOID		doc_ShowModelAsText			(HCANVAS,INDEX,Rect *);
PUBLIC OBJID	doc_DetectHitInGraphicsMode	(INDEX,Point *);

/****************************************************************
	VIEW MANAGER
*****************************************************************/

PUBLIC BOOL		vu_InitViews				(VOID);
PUBLIC INDEX	vu_WhichView				(HCANVAS);
PUBLIC INDEX	vu_WhichDocument			(INDEX);
PUBLIC INDEX	vu_TheCurrentView			(VOID);
LOCALF INDEX	allocate_view				(VOID);
PUBLIC VOID		vu_SetCurrentView			(VOID);
PUBLIC INDEX	vu_NewView			(STRING,Rect *,INDEX,ENUM);
PUBLIC VOID		vu_SetSelectionMode			(VOID);
PUBLIC VOID		vu_SetCreateRectMode		(VOID);
PUBLIC VOID		vu_SetCreateOvalMode		(VOID);
LOCALF VOID		vu_DetectHitInGraphicsMode	(INDEX,Point *);
LOCALF VOID		vu_MakeObjInGraphicsMode	(INDEX,Point *);
LOCALF VOID		vu_MakeObjInTextMode		(INDEX,Point *);
PUBLIC VOID		vu_MouseDown				(INDEX,Point *);
PUBLIC VOID		vu_ActivateView				(INDEX);
PUBLIC VOID		vu_InvalidateView			(INDEX);
PUBLIC VOID		vu_CloseView				(INDEX);
PUBLIC VOID		vu_OpenView					(INDEX);
PUBLIC VOID		vu_ResizeView				(INDEX,Point *);
PUBLIC VOID		vu_MoveView					(INDEX,Point *);
PUBLIC VOID		vu_UpdateView				(INDEX);
#ifdef MAC
PUBLIC BOOL		vu_IsAppView				(LONG);
#endif

/****************************************************************
    COMMAND DISPATCHER
*****************************************************************/

LOCALF BOOL		app_InitCommandDispatcher	(VOID);
LOCALF VOID		app_DispatchCommand			(int,int);

/****************************************************************/



