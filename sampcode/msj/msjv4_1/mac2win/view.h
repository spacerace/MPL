/****************************************************************
VIEW.H
****************************************************************/

typedef struct
{
	BOOL			in_use;
	INDEX			document;
	ENUM			viewtype; /*text or graphics mode*/
	HCANVAS			canvas;
} VIEW;

