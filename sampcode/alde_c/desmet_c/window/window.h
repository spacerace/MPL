typedef struct wcb	/* --- window control block	---		*/
{
	int		ulx,	/* upper left corner x coordinate	*/
			uly,	/* ditto y coordinate				*/
			xsize,	/* line width of inside of window	*/
			ysize,	/* number of lines inside of window	*/
			cx,		/* current cursor offset in window	*/
			cy,
			style,	/* attribute to be used in window	*/
			*scrnsave,	/* pointer to screen save buffer */
			oldx,	/* cursor position when window was	*/
			oldy;	/* opened (used for screen restore)	*/
} WINDOW, *WINDOWPTR;
