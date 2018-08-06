/*
**   CFLOWX.H : header file for CFLOW.C and CFLOWX.C
**
**	Copyright (c) 1985 by:
**
**		Lawrence R. Steeger
**		1009 North Jackson
**		Milwaukee, Wisconsin 53202
**		414-277-8149
*/

typedef	int (*PFI)();		/* pointer to function returning int	*/

#define	TRUE	1
#define	FALSE	0

#define	MAXBUF	513		/* maximum C source record buffer	*/
#define	MAXPATH	128		/* maximum MS-DOS 2 file name buffer	*/

/*	CFLOWX record types						*/

#define	NOFUNC	0		/* non-function related output (must=0)	*/
#define	HDRSYST	1		/* -h system prefix			*/
#define	HDRPROJ	2		/* -h project prefix			*/
#define	WILDPTH 3		/* wildcard path name			*/
#define	WILDNME	4		/* wildcard specification		*/
#define	FILEPTH	5		/* C source file path name		*/
#define	FILENME	6		/* C source file name			*/
#define	FUNCNME	7		/* function name			*/
#define	FUNCDCL	8		/* function definition			*/
#define	FUNCARG	9		/* function definition argument		*/
#define	FUNCREF	10		/* function reference			*/
#define	MACNME	11		/* #define macro name			*/
#define	MACDCL	12		/* #define macro definition		*/
#define	MACEQU	13		/* #define macro equate			*/
#define	NESTERR	14		/* C source {...} nesting level error	*/

#define	HIGHKEY (NESTERR+1)	/* highest valid cflowx record key + 1	*/

#define	WILDNF	15		/* no files match wildcard		*/
#define	FILEERR	16		/* C source file name (open error)	*/

static	char *types[] = {	/* CFLOWX record type names		*/
	"NOFUNC",
	"HDRSYST",
	"HDRPROJ",
	"WILDPTH",
	"WILDNME",
	"FILEPTH",
	"FILENME",
	"FUNCNME",
	"FUNCDCL",
	"FUNCARG",
	"FUNCREF",
	"MACNME",
	"MACDCL",
	"MACEQU",
	"NESTERR",
	"WILDNF",
	"FILEERR"
	};

/*	CFLOWX record fields:
**
**	1.  record type
**	2.  file name number
**	3.  {...} nest or #include file level number
**	4.  current line number
**	5.  record data string
*/

#define	MAXFLDS	5			/* maximum CFLOWX record fields	*/

#define	RS '\n'				/* CFLOWX record separator	*/
#define	US ','				/* CFLOWX unit separator	*/

/*	build CFLOWX record format string				*/

static	unsigned char *cxref(fields)
	int fields;
	{
		static unsigned char cp[(MAXFLDS*3)+1];

		/*	build CFLOWX record format string		*/

		sprintf(cp, "%s%c%s%c%s%c%s%c%s%c",
			"%d", US,
			"%d", US,
			"%d", US,
			"%d", US,
			"%s", RS
			);

		/*	truncate at requested number of fields		*/

		if (fields > MAXFLDS)
			abort("CXREF - %d > %d fields",
				fields,
				MAXFLDS);	/* too many fields !	*/
		else	cp[(fields*3)] = EOS;

		return (cp);		/* return format string pointer	*/
	}

/*	end of cflowx.h							*/

e