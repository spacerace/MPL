#define MSDOS2

#include "dos.h"
#include "stdio.h"
#include "ctype.h"
#include "ios1.h"

#define MAXARG 100              /* maximum command line arguments */

extern int _stack,_fmode,_iomode;
extern char _iname[],_oname[],_dos;
extern struct UFB _ufbs[];

static int argc;			/* arg count */
static char *argv[MAXARG];		/* arg pointers */


/**
 *
 * name         _main - process command line, open files, and call "main"
 *
 * synopsis     _main(line);
 *              char *line;     ptr to command line that caused execution
 *
 * description	This function performs the standard pre-processing for
 *		the main module of a C program.  It accepts a command
 *		line of the form
 *
 *			pgmname arg1 arg2 ...
 *
 *		and builds a list of pointers to each argument.  The first
 *		pointer is to the program name.  For some environments, the
 *		standard I/O files are also opened, using file names that
 *		were set up by the OS interface module XCMAIN.
 *
 ********
 *		Modified: 11 April 1984
 *			  Ross Nelson
 *
 *		This routine now treats quoted strings in the command line
 *		as a single token, and allows the expansion of wildcard
 *		characters (see WILDCARD.C).  A quoted string begins with
 *		one of the characters ' or " and continues until that
 *		character is found again (unless escaped by a \).  Any
 *		command argument containing the MSDOS wildcard characters
 *		(* and ?) must be quoted or this routine will attempt to
 *		perform filename expansion on them.  The program will
 *		exit if any wildcard expansion fails to match.	If wildcard
 *		expansion fills available memory or overflows MAXARG, a
 *		messaage will be printed, but processing will continue.
 ********
 *
 *		Further mods: Oct-11-84 Chuck Forsberg V2.12, Unix pathnames
 *
 */
_main(line)
char *line;
{
	char c;
	char quoted;
	int x;
#ifdef MSDOS2
	char buf[135], *wild, *malloc();
#endif
#if UNIX == 0
	FILE *fp0, *fp1, *fp2;
	extern int _bufsiz;
	char *getmem();
#endif

/*
 *
 * Build argument pointer list
 *
 */
	for(argc = 0; argc < MAXARG; )
	{
		while(isspace(*line))
			line++;
		if(*line == '\0')
			break;
		argv[argc++] = line;
		quoted = 0;
		if (((*line == '\'') || (*line == '"')) && (*(line-1) != '\\')) 
		{
			quoted++;
			c = *line;
			argv[argc-1]++;
			while (*++line)
				if ((*line == c) && (*(line-1) != '\\'))
					break;
		}
		else
			while((*line != '\0') && (isspace(*line) == 0))
				line++;
		c = *line;
		*line++ = '\0';
#ifdef MSDOS2
		if (!quoted && stpbrk(argv[argc-1], "*?")) {
			wild = argv[--argc];
			if (!wildcard(wild, 0x10, buf)) {
				bdos (9, "No match.\r\n$");
				exit (1);
			}
			else
				do {
					if (strcmp(buf, ".") && strcmp(buf, "..")) 
					{
						if ((argc == MAXARG) ||
						!(argv[argc] = malloc(strlen(buf)+1)))
						{
							bdos (9, "Wildcard expansion truncated.\r\n$");
							break;
						}
						strcpy(argv[argc++], buf);
					}
				}
					while (wildcard(wild, 0x10, buf));
		}
#endif
		if(c == '\0')
			break;
	}
/*
 *
 * Open standard files
 *
 */
#if UNIX == 0
	if(_dos < 2)
	{
		fp0 = freopen(_iname,"r",stdin);
		if(_oname[0] != '>')
			fp1 = freopen(_oname,"w",stdout);
		else
			fp1 = freopen(&_oname[1],"a",stdout);
		fp2 = freopen("","a",stderr);
		if (fp2 == NULL)
			_exit(1);
		if (fp0 == NULL) {
			fputs("Can't open stdin file\n", fp2);
			exit(1);
		}
		setbuf(fp0, getmem(_bufsiz));	/* set stdin buffered */
		fp0->_flag &= ~_IOMYBUF;	/* allow rlsmem if later set unbuff'd */
		if (fp1 == NULL) {
			fputs("Can't open stdout file\n", fp2);
			exit(1);
		}
	}
	else {
		stdin->_file = 0;
		stdin->_flag = _IOREAD;
		stdout->_file = 1;
		stdout->_flag = _IOWRT;
		stderr->_file = 2;
		stderr->_flag = _IOWRT | _IONBF;

		x = ((_fmode ^ _iomode) & 0x8000) ? UFB_NT : 0;
		_ufbs[0].ufbflg = UFB_OP | UFB_RA | x;
		_ufbs[1].ufbfh = 1;
		_ufbs[1].ufbflg = UFB_OP | UFB_WA | x;
		_ufbs[2].ufbfh = 2;
		_ufbs[2].ufbflg = UFB_OP | UFB_WA | x;
		if (_fgdi(1) & 0x80)
			stdout->_flag |= _IONBF;
	}
#endif


/*
 *
 * Call the program already.
 *
 */
	main(argc,argv);              /* Call main function */
	exit(0);
}
