/***************************************************************
    APPLCATN.C 
****************************************************************/    

#ifdef AS
#include <memory.h>
#endif

#include "CONFIG.H"

#include "STDDEFS.H"
#include "APPLCATN.H"
#include "ENVRNMT.H"

/****************************************************************/

PUBLIC VOID app_MainProcedure() 
{
	if(    env_InitEnvironment() 
	    && app_InitApplication()
	  )
	{
		while(app_EventLoop())
		    /*continue*/;
	}
}


/****************************************************************/

LOCAL BOOL app_InitApplication()	
{
	if( app_InitCommandDispatcher()
		&& vu_InitViews()
		&& doc_InitDocuments()
#ifdef MAC
#ifdef DBG
		&& ut_InitDebugModule()
		&& ut_CreateDebugMenu()
#endif
		/*&& vu_NewView("Untitled",(Rect *)NULL, doc_NewDocument(), GRAPHICS_VIEW)*/
#endif		
		)
			return TRUE;
	/*else*/
	return FALSE;
}

/****************************************************************/
#ifdef DBG
LOCAL STRING app_events[] =
{
    "NULL_EVENT","QUIT_EVENT","CMD_EVENT","MOUSEDOWN_EVENT",
    "MOUSEUP_EVENT","OPENVIEW_EVENT","CLOSEVIEW_EVENT",
    "MOVEVIEW_EVENT","RESIZEVIEW_EVENT","ACTIVATEVIEW_EVENT",
    "UPDATEVIEW_EVENT",	"CHAR_EVENT"
};
#endif

LOCAL BOOL app_EventLoop()
{
	APPEVENT		theEvent;
	
	env_GetAppEvent(&theEvent);
	
	ut_PutString("RV|appEventLoop: appevent",
		app_events[theEvent.event_type]);

	switch (theEvent.event_type)
	{
		case QUIT_EVENT:
		{
			return FALSE;
		} break;
		
		case CMD_EVENT: 
		{
			app_DispatchCommand( theEvent.cmd_category, 
						    theEvent.cmd_item ); 
		} break;
			
		case MOUSEDOWN_EVENT:
		{
			INDEX vu = vu_WhichView(theEvent.canvas);
			if (vu != INVALID_INDEX)
					vu_MouseDown(vu, &theEvent.where);
		} break;
	
		case OPENVIEW_EVENT:
		{
			INDEX vu = vu_WhichView(theEvent.canvas);
			if (vu != INVALID_INDEX)
					vu_OpenView(vu);
		} break;
				
		case CLOSEVIEW_EVENT:
		{
			INDEX vu = vu_WhichView(theEvent.canvas);
			if (vu != INVALID_INDEX)
				vu_CloseView(vu);
		} break;
				
		case MOVEVIEW_EVENT:
		{
			INDEX vu = vu_WhichView(theEvent.canvas);
			if (vu != INVALID_INDEX)
			{
				vu_MoveView(vu, &theEvent.where);
			}
		} break;
				

		case RESIZEVIEW_EVENT:
		{
			INDEX vu = vu_WhichView(theEvent.canvas);
			if (vu != INVALID_INDEX)
					vu_ResizeView( vu, &theEvent.where );
		} break;
				
		case CHAR_EVENT:					
		{
			if(theEvent.the_char=='x') 
				env_FatalError("X means quit program.");
		} break;

		case ACTIVATEVIEW_EVENT:
		{
			INDEX vu = vu_WhichView( theEvent.canvas );
			
			if(vu != INVALID_INDEX)
			{
				vu_ActivateView(vu);			
				vu_InvalidateView(vu); /*generate update*/
			}
		}	break;
		
		case UPDATEVIEW_EVENT:
		{
			INDEX vu = vu_WhichView( theEvent.canvas );
	
			ut_Print2Shorts("RV|appEventLoop: vu,canvas",
				    vu,theEvent.canvas);
			if(vu != INVALID_INDEX)
			{
				vu_UpdateView( vu );
			}
		} break;
		
	} /*end switch*/
    return TRUE;
}


/****************************************************************
    OBJECT MANAGER
****************************************************************/    


PUBLIC VOID obj_HighlightObjAsGraphics(canvas, obj)
	HCANVAS		canvas;
	POBJ 		obj;
{
	Rect r;
	r = obj->rect; /*structure copy */
	env_StartDrawing(canvas);
	switch(obj->objtype)
	{
		case RECT_OBJ: env_FillRect(canvas,&r,PATT_GRAY);
				break;
		case OVAL_OBJ: env_PaintRect(canvas,&r); break;
	}
	env_EndDrawing(canvas);
}

PUBLIC VOID obj_ShowObjAsGraphics(canvas, obj)
	HCANVAS	canvas;
	POBJ	obj;
{
	Rect r;
	r = obj->rect; /*structure copy */
	env_StartDrawing(canvas);    
	switch(obj->objtype)
	{
		case RECT_OBJ: env_FrameRect(canvas,&r); break;
		case OVAL_OBJ: env_FrameRect(canvas,&r); break;
	}
	env_EndDrawing(canvas);
}

PUBLIC VOID obj_MakeObject(obj, r, type, attr)
	POBJ 	obj;
	Rect *	r;
	ENUM	type,attr;
{
	obj->rect    = *r; /*structure copy */
	obj->objtype = type; 
	obj->objattr = attr;
}

PUBLIC VOID obj_ShowObjAsText( canvas, x,y,obj)
	HCANVAS		canvas;
	PIXEL		x,y;
	POBJ 		obj;
{
	env_StartDrawing(canvas);    
	switch(obj->objtype)
	{
		case RECT_OBJ: env_TextOut(canvas,x,y,"RECTANGLE"); break;
		case OVAL_OBJ: env_TextOut(canvas,x,y,"OVAL"); break;
	}
	env_EndDrawing(canvas);
}

		
/****************************************************************
    DATA MODEL MANAGER (contains objects)
****************************************************************/    


PUBLIC HMODEL mod_NewModel()
{
	HMODEL h;
	PMODEL m;
	h = (HMODEL) env_AllocMem( (MEMSIZE) sizeof(int));
	m = (PMODEL) env_GraspMem(h);
	m->num_objects = 0;
	env_UnGraspMem(h);
	return h;
}
	
PUBLIC OBJID mod_AddObjectToModel( model, r, type,attr)
	HMODEL *model;
	Rect *	r;
	ENUM	type,attr;
{
	INDEX	i;
	PMODEL	m;
	int	num_objects;

	m = (PMODEL) env_GraspMem(*model);
	num_objects = ++(m->num_objects);

	env_UnGraspMem(*model);
	
	env_ReAllocMem( (MEMBLOCK *) model, 
			(MEMSIZE) (sizeof(int) + (num_objects * sizeof(OBJ))));
	
	m = (PMODEL) env_GraspMem(*model);
	i = m->num_objects-1;
	ut_PrintShort(  "RV|modAddObj: obj#",i);
	ut_PrintMacRect("RV|modAddObj: rect",r);	
	obj_MakeObject((POBJ) &(m->objects[i]), r, type, attr);
	env_UnGraspMem(*model);
	return i;
}

PUBLIC VOID mod_HighlightObjAsGraphics( canvas, model, obj)
	HCANVAS	canvas;
	HMODEL		model;
	OBJID		obj;
{
	PMODEL		m;
	
	m = (PMODEL) env_GraspMem(model);
	obj_HighlightObjAsGraphics(canvas, (POBJ) &(m->objects[obj]));
	env_UnGraspMem(model);
}

PUBLIC VOID mod_ShowObjAsGraphics( canvas, model, obj)
	HCANVAS		canvas;
	HMODEL		model;
	OBJID		obj;
{
	PMODEL 		m;
	m = (PMODEL) env_GraspMem(model);
	obj_ShowObjAsGraphics(canvas, (POBJ) &(m->objects[obj]));
	env_UnGraspMem(model);
}

PUBLIC VOID mod_ShowModelAsGraphics( canvas, model)
	HCANVAS		canvas;
	HMODEL		model;
{
	PMODEL	m;
	INDEX	i;
	m = (PMODEL) env_GraspMem(model);
	for(i=0; i < m->num_objects; i++)
	{
		obj_ShowObjAsGraphics(canvas, (POBJ) &(m->objects[i]) );
	}
	env_UnGraspMem(model);
}


PUBLIC VOID mod_ShowModelAsText( canvas, model,rect,linespace,left_margin)
	HCANVAS	canvas;
	HMODEL		model;
	Rect *		rect;
	PIXEL		linespace,left_margin;
{
	PMODEL		m;
	INDEX		i;
	PIXEL		x,y;

	ut_PrintMacRect("RV|mdShowModelAsText: rect",rect);

	m = (PMODEL) env_GraspMem(model);
	x = rect->left + left_margin;
	y = rect->top;
	
	for(i=0; i < m->num_objects; i++)
	{
		y += linespace;
		if( y > rect->bottom) y = rect->top + linespace;
		obj_ShowObjAsText( canvas, x, y, (POBJ) &(m->objects[i]));
	}
	env_UnGraspMem(model);
}

PUBLIC VOID mod_ShowObjAsText( canvas, model, obj, rect, linespace, left_margin)
	HCANVAS canvas;
	HMODEL	model;
	OBJID	obj;
	Rect *	rect;
	PIXEL	linespace,left_margin;
{
	PMODEL	m;
	INDEX	i;
	PIXEL	x,y;
	
	m = (PMODEL) env_GraspMem(model);
	x = rect->left + left_margin;
	y = rect->top;
	
	for(i=0; i < m->num_objects; i++)
	{
		y += linespace;
		if( y > rect->bottom) y = rect->top + linespace;
		if(obj==i) 
		    obj_ShowObjAsText( canvas, x,y, (POBJ) &(m->objects[i]));
	}
	env_UnGraspMem(model);
}

PUBLIC OBJID mod_DetectHitInTextMode( model,where, rect, linespace, left_margin)
	HMODEL	model;
	Point *	where;
	Rect *	rect;
	PIXEL	linespace,left_margin;
{
	PMODEL	m;
	INDEX	i;
	PIXEL	x,y;
	
	m = (PMODEL) env_GraspMem(model);
	x = rect->left + left_margin;
	y = rect->top;
	
	for(i=0; i < m->num_objects; i++)
	{
		y += linespace;
		if( y > rect->bottom) y = rect->top + linespace;
		if(y > where->v)
		{
			env_UnGraspMem(model);
			return MAX(0,i-1);
		}
	}
	env_UnGraspMem(model);
	return MAX(0, m->num_objects - 1);
}	
	
	
PUBLIC OBJID mod_DetectHitInGraphicsMode( model,where)
	HMODEL	model;
	Point *	where;
{
	PMODEL	m;
	INDEX	i;
	m = (PMODEL) env_GraspMem(model);
	for(i=0; i < m->num_objects; i++)
	{
		if(env_PtInRect(where,(pRect) &(m->objects[i].rect)))
		{
			env_UnGraspMem(model);
			return (OBJID) i;
		}
	}
	env_UnGraspMem(model);
	return INVALID_OBJID;
}



/****************************************************************
    DOCUMENT MANAGER
	Each document contains a data model and multiple
	views to that data model.
****************************************************************/    


LOCAL DOC theSetOfDocuments[MAX_DOCUMENTS];


PUBLIC BOOL doc_InitDocuments()
{
	REG DOC *p;
	int i;
	
	for(p = theSetOfDocuments; p < &theSetOfDocuments[MAX_DOCUMENTS]; p++)
	{
		p->in_use = p->is_modified = FALSE;
		for(i=0; i < VIEWS_PER_DOC; i++) p->views[i] = INVALID_INDEX;
		p->model = INVALID_MODEL;
	}
	return TRUE;
}

LOCAL INDEX allocate_document()
{
	REG DOC *p;
	for(p = theSetOfDocuments; p < &theSetOfDocuments[MAX_DOCUMENTS]; p++)
	{
		if( !p->in_use )
		{
			p->in_use		= TRUE;
			p->is_modified	= FALSE;
			p->model		= mod_NewModel();
			if(p->model==INVALID_MODEL) return INVALID_INDEX;
			return (INDEX) (p - theSetOfDocuments);
		}
	}
	/*not found*/
	return INVALID_INDEX;
}

PUBLIC INDEX doc_NewDocument()
{
	INDEX i = allocate_document();
	if(i==INVALID_INDEX) env_FatalError("NewDoc: allocate doc failed!");
	return i;
}

PUBLIC BOOL doc_AddViewToDoc( doc, vu)
	INDEX doc,vu;
{
	DOC *p = &theSetOfDocuments[doc];
	int i;
	for(i = 0; i < VIEWS_PER_DOC; i++)
	{
		if(p->views[i] == INVALID_INDEX) break;
	}
	if(p->views[i] != INVALID_INDEX) return FALSE;
	p->views[i] = vu;
	return TRUE;
}

PUBLIC OBJID doc_AddObjectToDocument( doc, r, type, attr)
	INDEX doc;
	Rect *r;
	ENUM type;
	ENUM attr;
{
	DOC *p = &theSetOfDocuments[doc];
	int i;

	ut_PrintShort("RV|AddObj: doc#",doc);

	p->is_modified = TRUE;
	for(i = 0; i < VIEWS_PER_DOC; i++)
	{
		if(p->views[i] != INVALID_INDEX) vu_InvalidateView(p->views[i]);
	}
	return mod_AddObjectToModel( &(p->model),r,type,attr);
}


/*****************************************************************
    DOC routines which invoke the display of data model objects
****************************************************************/    

PUBLIC VOID doc_ShowObjAsGraphics( canvas, doc, obj) 
	HCANVAS	canvas;
	INDEX 	doc;	/* called from MakeObj in graphics mode*/
	OBJID	obj;
{
	mod_ShowObjAsGraphics(canvas, theSetOfDocuments[doc].model, obj);
	
}

PUBLIC VOID doc_HighlightObjAsGraphics( canvas, doc, obj) 
	HCANVAS canvas;
	INDEX 	doc;		/* called from MakeObj in graphics mode*/
	OBJID	obj;
{
	mod_HighlightObjAsGraphics(canvas, theSetOfDocuments[doc].model, obj);
	
}

PUBLIC VOID doc_ShowObjAsText( canvas, doc, obj, rect)
	HCANVAS		canvas;
	INDEX 		doc;
	OBJID		obj;
	Rect *		rect;
{
	mod_ShowObjAsText(canvas, theSetOfDocuments[doc].model, obj, rect,
				10 /*linespace*/, 10 /*leftmargin*/);
	
}

PUBLIC VOID doc_ShowModelAsGraphics( canvas, doc) 
	HCANVAS		canvas;
	INDEX 		doc;
{
	mod_ShowModelAsGraphics(canvas, theSetOfDocuments[doc].model);
	
}

PUBLIC VOID doc_ShowModelAsText( canvas, doc, rect) 
	HCANVAS	canvas;
	INDEX 		doc;
	Rect *		rect;
{
	mod_ShowModelAsText(canvas, theSetOfDocuments[doc].model, rect,
				10 /*linespace*/, 10 /*leftmargin*/);
}

PUBLIC OBJID doc_DetectHitInGraphicsMode( doc, where)
	INDEX 	doc;
	Point *	where;
{
	return mod_DetectHitInGraphicsMode(theSetOfDocuments[doc].model, where);
	
}


/****************************************************************
    VIEW MANAGER
	Each view is connected to one canvas (drawing surface)
*****************************************************************/    


#include "view.h"	

LOCAL VIEW		theSetOfViews[MAX_VIEWS];
LOCAL Rect		theWholeScreen; /*q:how about multiple screens? */

LOCAL INDEX		theCurrentView;
LOCAL ENUM		theCurrentVuMode;

/*values for VuMode*/
#define SELECTION_MODE		0
#define CREATE_RECT_MODE	1
#define CREATE_OVAL_MODE	2


PUBLIC BOOL vu_InitViews()
{
	REG VIEW *p;
	
	for(p = theSetOfViews; p < &theSetOfViews[MAX_VIEWS]; p++)	
	{
		p->in_use	= FALSE;
		p->document = INVALID_INDEX;
		p->viewtype     = NULL;
	}
	theCurrentView = INVALID_INDEX;
	theCurrentVuMode = CREATE_RECT_MODE;
	
	env_GetScreenSize(&theWholeScreen);
	return TRUE;
}


PUBLIC INDEX vu_WhichView(canvas)
	HCANVAS canvas;
{
	REG VIEW *p;
	
	for(p = theSetOfViews; p < &theSetOfViews[MAX_VIEWS]; p++)	
	{
		if(p->in_use && p->canvas==canvas)
			return (INDEX)(p - theSetOfViews);
	}
	return INVALID_INDEX;
}

#ifdef MAC	/*this may no longer be needed for Mac */
PUBLIC BOOL vu_IsAppView(devdata)
	LONG devdata;
{
	return env_IsDevData(devdata);
}
#endif

PUBLIC INDEX vu_WhichDocument(vu)
	INDEX vu;
{
#ifdef AS
#else
	REG VIEW *p;
#endif
	return theSetOfViews[vu].document;
}

PUBLIC INDEX vu_TheCurrentView()
{
	return theCurrentView;
}

LOCAL INDEX allocate_view()
{
	VIEW *p;
	for(p = theSetOfViews; p < &theSetOfViews[MAX_VIEWS]; p++)
	{
		if( !p->in_use )
		{
			p->in_use = TRUE;
			p->document  = INVALID_INDEX;
			p->viewtype  = NUþL;
			return (INDEX) (p - theSetOfViews);
		}
	}
	/*not found*/
	return INVALID_INDEX;
}

PUBLIC VOID vu_SetCurrentView()
{
#ifdef MACXXXX /*move to environment.c*/
	if(theCurrentView != INVALID_INDEX)
	    SetPort((GrafPtr) &(theSetOfViews[theCurrentView].window_record));
#endif
}

PUBLIC INDEX vu_NewView(title, rect, doc, viewtype)
	STRING	title;
	Rect *	rect;
	INDEX	doc;
	ENUM	viewtype;
{
	INDEX		vu;
	Rect		r;
	
	if(rect==(Rect *)NULL) /*compute size of window*/
	{
		r = theWholeScreen; /*structure copy */
		r.top		+= 50;
		r.left		+= 20;
		r.bottom	= r.top + DEPTH_RECT(&theWholeScreen)/2;
		r.right		-= 40;
	}
	
	if( (vu=allocate_view()) ==INVALID_INDEX) 
		env_FatalError("NewView: create new view failed!");

	theSetOfViews[vu].canvas   = env_NewCanvas( (long) vu);
	theSetOfViews[vu].document = doc;
	theSetOfViews[vu].viewtype = viewtype;

	if(!doc_AddViewToDoc(doc,vu)) env_FatalError("NewView: AddView failed!");

	theCurrentView = vu;

	if( !env_NewWindow( title, &r, vu, theSetOfViews[vu].canvas) )
	       env_FatalError("NewView: envNewWindow failed!");
	vu_SetCurrentView();
	return TRUE;				
}

PUBLIC VOID vu_SetSelectionMode() {theCurrentVuMode = SELECTION_MODE; }
PUBLIC VOID vu_SetCreateRectMode() {theCurrentVuMode = CREATE_RECT_MODE; }
PUBLIC VOID vu_SetCreateOvalMode() {theCurrentVuMode = CREATE_OVAL_MODE; }

	
/****** OPERATIONS ON OBJECTS WITHIN A VIEW ********/

LOCAL VOID vu_DetectHitInGraphicsMode(vu, where)	
	INDEX vu; Point * where;
{
	OBJID		obj;
	
	env_GlobalToLocal( theSetOfViews[vu].canvas, where );
	obj = doc_DetectHitInGraphicsMode(theSetOfViews[vu].document,where);
	if(obj != INVALID_OBJID)Š		doc_HighlightObjAsGraphics(theSetOfViews[vu].canvas,
			    theSetOfViews[vu].document, obj);
}

LOCAL VOID vu_MakeObjInGraphicsMode(vu, where)	
	INDEX vu; Point * where;
{
	Rect			r;
	OBJID			obj;
	

	env_GlobalToLocal(theSetOfViews[vu].canvas, where );

	r.top = where->v; r.left = where->î;
	r.bottom = r.top + 10;
	r.right  = r.left +10;
	
	obj = doc_AddObjectToDocument(theSetOfViews[vu].document, &r, 
					    RECT_OBJ, FRAME_ATTR);
	if(obj==INVALID_OBJID) env_FatalError("AddObject failed!");
	doc_ShowObjAsGraphics(theSetOfViews[vu].canvas, 
					    theSetOfViews[vu].document, obj);
}

LOCAL VOID vu_MakeObjInTextMode(vu, where)	
	INDEX vu; Point * where;
{
#ifdef MACXXXX    /*to fix up*/
	HCANVAS		canvas = (HCANVAS) &(theSetOfViews[vu].window_record);
	Rect		r;
	OBJID		obj;
	
	env_GlobalToLocal(theSetOfViews[vu].canvas, where );
	r.top = where->v; r.left = where->h;
	r.bottom = r.top + 10;
	r.right  = r.left +10;
	
	obj = doc_AddObjectToDocument(theSetOfViews[vu].document, &r, RECT_OBJ, FRAME_ATTR);
	if(obj==INVALID_OBJID) env_FatalError("AddObject failed!");
	doc_ShowObjAsText(canvas, theSetOfViews[vu].document, obj, &(canvas->portRect));
#endif
}

	
PUBLIC VOID vu_MouseDown(vu, where)
	INDEX vu; Point *where; 
{
	Point pt;
	pt = *where; /*structure copy */

	if(theSetOfViews[vu].viewtype==GRAPHICS_VIEW)
	{
		switch(theCurrentVuMode)
		{
			case SELECTION_MODE:
			{
				vu_DetectHitInGraphicsMode(vu,&pt); 
			} break;
			case CREATE_RECT_MODE:
			case CREATE_OVAL_MODE:
			{
				vu_MakeObjInGraphicsMode(vu, &pt);
			} break;
		}
	}
	else if (theSetOfViews[vu].viewtype==TEXT_VIEW)
	{
		switch(theCurrentVuMode)
		{
#if 0 /*to be done*/
			case SELECTION_MODE:
			{
				vu_DetectHitInTextMode(vu,&pt); 
			} break;
			case CREATE_RECT_MODE:
			case CREATE_OVAL_MODE:
			{
				vu_make_a_rectangle(vu, &pt);
			} break;
#endif
		}
	}
	
}

/****************************************************************
    OPERATIONS ON VIEWS: move, resize, close, invalidate, update
****************************************************************/    

PUBLIC VOID vu_ActivateView(vu)
	INDEX	vu;
{
	env_ActivateCanvas( theSetOfViews[vu].canvas );
}

PUBLIC VOID vu_InvalidateView(vu)
	INDEX	vu;
{
	env_InvalidateCanvas( theSetOfViews[vu].canvas, (Rect*)NULL );
}

PUBLIC VOID vu_CloseView(vu)
	INDEX vu;
{
	env_FatalError("CloseView called!"); /*to be done*/
}

PUBLIC VOID vu_OpenView(vu)
	INDEX vu;
{
	env_CalcSizeCanvas(theSetOfViews[vu].canvas);    
}

PUBLIC VOID vu_ResizeView(vu,where) 
	INDEX vu; Point *where;
{
	env_ResizeCanvas(theSetOfViews[vu].canvas,where);
	env_CalcSizeCanvas(theSetOfViews[vu].canvas);
}

PUBLIC VOID vu_MoveView(vu,where)
	INDEX vu; Point *where;
{
	env_MoveCanvas(vu,where);
	env_CalcSizeCanvas(theSetOfViews[vu].canvas);    
}

PUBLIC VOID vu_UpdateView(vu)
	INDEX	vu;
{
	HCANVAS	canvas = theSetOfViews[vu].canvas;
#ifdef AS
#else
	Rect	rect;
#endif

	ut_Print2Shorts("RV|vu_UpdateView: ENTRY view# doc#",
			vu,theSetOfViews[vu].document);
	
	env_BeginUpdate( canvas );
	
	if(theSetOfViews[vu].viewtype==GRAPHICS_VIEW)
	{
		doc_ShowModelAsGraphics(canvas, theSetOfViews[vu].document);
	}
	else if (theSetOfViews[vu].viewtype==TEXT_VIEW)
	{
#if 0 /*to be done*/
		doc_ShowModelAsText(canvas, theSetOfViews[vu].document,
			    &(canvas->portRect));
#endif
	}
	env_EndUpdate( canvas );
	ut_PrintShort("RV|vu_UpdateView: EXIT canvas#",canvas);
}

/****************************************************************
    COMMAND DISPATCHER
*****************************************************************/

#include "dispatch.h"

LOCAL BOOL app_InitCommandDispatcher() 
{
    if(env_InitMenus())
	return TRUE;
    /*else*/
    return FALSE;
}


LOCAL VOID app_DispatchCommand( menu_category, menu_item )
	int		menu_category, menu_item;
{
	ut_Print2Shorts("RV|appDispatchCmd: category,item",
				    menu_category,menu_item);
	switch (menu_category) 
	{
		case R_ID_SYS_MENU:
		{
			env_DoSysMenu(menu_item);
		} break;
		
		case R_ID_FILE_MENU: 
		{
			switch(menu_item)
			{
				case FILE_CMD_NEW:
				{
					vu_NewView("Untitled",(Rect *)NULL, doc_NewDocument(), GRAPHICS_VIEW);
				} break;
				
				case FILE_CMD_OPEN:	env_NotImplemented("FIle Open"); break;
				case FILE_CMD_CLOSE:	env_NotImplemented("FIle Close"); break;
				case FILE_CMD_SAVE:	env_NotImplemented("FIle Save"); break;
				case FILE_CMD_SAVE_AS:	env_NotImplemented("FIle SaveAs"); break;
				case FILE_CMD_REVERT:	env_NotImplemented("FIle Revert"); break;
				case FILE_CMD_PAGE_SETUP:
				case FILE_CMD_PRINT:
				case FILE_CMD_QUIT:
				default: ;
			}
		} break;
		
#if 0 /*to be done*/
		case R_ID_EDIT_MENU: 
		{
			if (SystemEdit(menu_item - 1)==0) 
			{
				switch (menu_item) 
				{
				case EDIT_CMD_CUT:		TECut(    TEH );	dirty = TRUE;	break;
				case EDIT_CMD_COPY:		TECopy(   TEH );					break;
				case EDIT_CMD_PASTE:	TEPaste(  TEH );	dirty = TRUE;	break;
				case EDIT_CMD_CLEAR:	TEDelete( TEH );	dirty = TRUE;	break;
				default: ;
				}
				ShowSelect();
			}
		} break;

		case R_ID_VIEW_MENU: 
		{
			switch(menu_item)
			{
			case VIEW_CMD_TEXT_VIEW: vu_NewView("Text View",(Rect *)NULL,
									vu_WhichDocument(vu_TheCurrentView()),TEXT_VIEW);  
										break;
			case VIEW_CMD_GRAPHICS_VIEW: vu_NewView("Graphics View",(Rect *)NULL,
									vu_WhichDocument(vu_TheCurrentView()),GRAPHICS_VIEW);  
			}
			HiliteMenu(0);
		} break;
#endif		

		case R_ID_PALETTE_MENU: 
		{
			switch(menu_item)
			{
				case PALETTE_SELECTION_MODE:vu_SetSelectionMode(); break;
				case PALETTE_CREATE_RECT_MODE:vu_SetCreateRectMode(); break;
			}
#ifdef MAC
			HiliteMenu(0);
#endif		    
		} break;
	} /*outer-switch*/
}
