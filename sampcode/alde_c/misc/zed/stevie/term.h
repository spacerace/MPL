/*
 * STEVIE - ST Editor for VI Enthusiasts   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

/*
 * This file contains the machine dependent escape sequences that
 * the editor needs to perform various operations. Some of the sequences
 * here are optional. Anything not available should be indicated by
 * a null string. In the case of insert/delete line sequences, the
 * editor checks the capability and works around the deficiency, if
 * necessary.
 *
 * Currently, insert/delete line sequences are used for screen scrolling.
 * There are lots of terminals that have 'index' and 'reverse index'
 * capabilities, but no line insert/delete. For this reason, the editor
 * routines s_ins() and s_del() should be modified to use 'index'
 * sequences when the line to be inserted or deleted line zero.
 */

/*
 * The macro names here correspond (more or less) to the actual ANSI names
 */

#ifdef	ATARI
#define	T_EL	"\033l"		/* erase the entire current line */
#define	T_IL	"\033L"		/* insert one line */
#define	T_DL	"\033M"		/* delete one line */
#define	T_SC	"\033j"		/* save the cursor position */
#define	T_ED	"\033E"		/* erase display (may optionally home cursor) */
#define	T_RC	"\033k"		/* restore the cursor position */
#define	T_CI	"\033f"		/* invisible cursor (very optional) */
#define	T_CV	"\033e"		/* visible cursor (very optional) */
#endif

#ifdef	UNIX
/*
 * The UNIX sequences are hard-wired for ansi-like terminals. I should
 * really use termcap/terminfo, but the UNIX port was done for profiling,
 * not for actual use, so it wasn't worth the effort.
 */
#define	T_EL	"\033[2K"	/* erase the entire current line */
#define	T_IL	"\033[L"	/* insert one line */
#define	T_DL	"\033[M"	/* delete one line */
#define	T_ED	"\033[2J"	/* erase display (may optionally home cursor) */
#define	T_SC	"\0337"		/* save the cursor position */
#define	T_RC	"\0338"		/* restore the cursor position */
#define	T_CI	""		/* invisible cursor (very optional) */
#define	T_CV	""		/* visible cursor (very optional) */
#endif

#ifdef	OS2

/* some work needs done here for use under Turbo C & MS/PC-DOS
   6/14/88 - dlm */

/*
 * The OS/2 ansi console driver is pretty deficient. No insert or delete line
 * sequences. The erase line sequence only erases from the cursor to the end
 * of the line. For our purposes that works out okay, since the only time
 * T_EL is used is when the cursor is in column 0.
 */
#define	T_EL	"\033[K"	/* erase the entire current line */
#define	T_IL	""		/* insert one line */
#define	T_DL	""		/* delete one line */
#define	T_ED	"\033[2J"	/* erase display (may optionally home cursor) */
#define	T_SC	"\033[s"	/* save the cursor position */
#define	T_RC	"\033[u"	/* restore the cursor position */
#define	T_CI	""		/* invisible cursor (very optional) */
#define	T_CV	""		/* visible cursor (very optional) */
#endif
