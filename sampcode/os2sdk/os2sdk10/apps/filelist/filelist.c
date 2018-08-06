/*
 * This example illustrates the use of DOSFINDFIRST, DOSFINDNEXT, and
 * DOSFINDCLOSE.  The program lists the contents of a directory.
 * the parameter format is: FILELIST [-1al] names...
 */

#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <doscalls.h>
#include <subcalls.h>

#define	TRUE	0
#define	FALSE	1

/* error codes */

#define ERROR_NO_MORE_FILES	18

/* file attribute constants */

#define	ATTR_READONLY	0x001	/* read only file */
#define	ATTR_HIDDEN	0x002	/* hidden file */
#define	ATTR_SYSTEM	0x004	/* system file */
#define	ATTR_DIRECTORY	0x010	/* subdirectory entry */
#define	ATTR_ARCHIVE	0x020	/* archive bit */

/* type of list */

#define	NAME_MULTICOLM	0	/* list name only in multipule columns */
#define	NAME_SINGCOLM	1	/* list name only in a single column */
#define	LONG_LIST	2	/* list all data on file */
#define	TOTALCOLMS	4	/* number of colms when listing name only */

#define	RESULTBUFLEN	sizeof(struct FileFindBuf)	/* size of ResultBuf */

#define	SEARCHALL	"*.*"	/* default - search for everything */
#define	MAXPATHSIZE	128
#define	FILEPATHSIZE	MAXPATHSIZE + sizeof(SEARCHALL) + 1

main(argc, argv)
	int	argc;
	char	*argv[];
{
	unsigned	DirHandle = -1;	/* use any available directory handle */
	unsigned	SearchCount;	/* number of files to search for */
	unsigned 	Attribute = ATTR_DIRECTORY; 	/* default attribute */
	unsigned	rc;				/* return code */
	unsigned	listType = NAME_MULTICOLM; 	/* default output */
	char 		FilePath[FILEPATHSIZE];
	char		*s;
	struct FileFindBuf *ResultBuf;	   /* pointer to returned data */

	/* parse command line for switches */

	while ((--argc > 0) && (**++argv == '-')) {
	    for (s = argv[0]+1; *s != '\0'; s++)

		switch(*s) {

		case 'a': Attribute |= (ATTR_HIDDEN | ATTR_SYSTEM);
			  break;

		case 'l': listType = LONG_LIST;
			  Attribute |= (ATTR_HIDDEN | ATTR_SYSTEM);
			  break;
	
		case '1': listType = NAME_SINGCOLM;
			  break;

		default: printf("usage: filelist [ -1al ] name...\n");
			 DOSEXIT(1, 0);
		}
	}

	/* allocate buffer for file data returned from find calls */

	if ((ResultBuf =
		(struct FileFindBuf *) malloc(RESULTBUFLEN)) == NULL) {

	    printf("error, not enough memory\n");
	    DOSEXIT(1, 0);
	}

	do {

	    if (argc > 0) {
		if (strlen(*argv) > MAXPATHSIZE) {
			printf("error, path too large\n");
			DOSEXIT(1, 1);
		}
		else {
			/* if path ends with a \, append "*.*" */

			strcpy(FilePath, *argv);
			if (FilePath[strlen(*argv) - 1] == '\\')
				strcat(FilePath, SEARCHALL);
		}
	    }
	    else
		strcpy(FilePath, SEARCHALL);	/* search using "*.*" */

	    SearchCount = 1;		/* search for one at a time */

	    /* search for first occurance of file search pattern */

	    if (rc = DOSFINDFIRST( (char far *) FilePath,
				   (unsigned far *) &DirHandle,
				   Attribute,
			  	   ResultBuf,	/* ptr to returned data */
				   RESULTBUFLEN,
				   (unsigned far *) &SearchCount,
			 	   0L )) {

		if (rc != ERROR_NO_MORE_FILES) {
		    printf("\nFindFirst failed, error %d\n", rc);
		    DOSEXIT(1, 1);
		}

	    } else {

		printDirEntry( ResultBuf, listType );	/* print file data */

		do {

		    /* search for next occurance of search pattern */

		    if (rc = DOSFINDNEXT( DirHandle,
				          ResultBuf,
				          RESULTBUFLEN,
				          (unsigned far *) &SearchCount )) {

		        if (rc != ERROR_NO_MORE_FILES)  {
			    printf("DosFindNext failed, error: %d\n", rc);
			    DOSEXIT(1, 1);
		        }
		    }
		    else
			/* print file data */

		        printDirEntry( ResultBuf, listType );
    	
	        } while (SearchCount > 0);	/* when 0, no more files */
	    }

	    DOSFINDCLOSE( DirHandle );		/* free directory handle */

	    if (argc > 1)	/* if any more arguments */
		argv++;		/* point to next one */

	} while (--argc > 0);			/* for all filenames */
}
	

/*
 * This routine prints the data on the file found.
 */

printDirEntry(dirEntry, ltype)
	struct FileFindBuf *dirEntry;
	unsigned		ltype;
{
	static int	colm = 0;
	char		attStr[6];
	long		dateTime;


	if (ltype == NAME_SINGCOLM)
	    printf("%s\n", dirEntry->file_name);

	else if (ltype == NAME_MULTICOLM) {

	    printf("%-15s", dirEntry->file_name);

	    if (++colm == TOTALCOLMS) {
		printf("\n");
		colm = 0;
	    }
	}
	else {

	   /* build attribute string */

	   strcpy(attStr, "-----");
	   if (dirEntry->attributes & ATTR_DIRECTORY)
		attStr[0] = 'd';
	   if (dirEntry->attributes * ATTR_ARCHIVE)
		attStr[1] = 'a';
	   if (dirEntry->attributes & ATTR_SYSTEM)
		attStr[2] = 's';
	   if (dirEntry->attributes & ATTR_HIDDEN)
		attStr[3] = 'h';
	   if (dirEntry->attributes & ATTR_READONLY)
		attStr[4] = 'r';

	   printf("%-7s", attStr);			/* print attribute */
	   printf("%7lu", dirEntry->file_size);		/* print file size */
	   printf("  %s\n", dirEntry->file_name);/* print filename */
	}
}
