/*
 *	Header file for wildcard routines 
 */

#define GE_BADPATH	 -1	/* invalid path - directory not found	*/
#define GE_AMBIGUOUS	 -2	/* ambiguous directory spec		*/
#define GE_MAXFILES	 -3	/* too many files found (MAXFILES)	*/
#define GE_NOMEM	 -4	/* out of memory			*/

extern int globerror;		/* PUBLIC - global error number */
extern char **glob( char * );

