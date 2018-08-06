#include <osbind.h>
#include <stdio.h>
#include <io.h>

#define	REDIRECT
/*
 *	Since the operating system and many shells don't do i/o
 *	redirection like they should, this #define includes code
 *	to handle redirection during argument processing.  If
 *	you don't want this to happen, #undef REDIRECT
 */

extern char	*_base;
extern int	_argc;
extern char	**_argv;
extern char	*_envp;

static char	args[512];		/* argc/argv buffer */

void _initargs(cmdline, cmdlen)
register char *cmdline;
register int cmdlen;
/*
 *	Process command line and retrieve the environment string.  This
 *	function is responsible for allocating space to hold the parsed
 *	command line arguments, etc.  Values to be passed to the user's
 *	main() function are stored in the global variables "_argc",
 *	"_argv" and "_envp".  _main() initiallizes these variables to
 *	indicate that no arguments are available.  If a program doesn't
 *	use command line arguments or the environment string, an
 *	_initargs() function, which does nothing, should be defined in
 *	the module containing main().  This will result in a smaller
 *	executable size and a smaller run-time image.
 */
{
	register char *p, **q;
	register int i, j, n;
	char *t, *strtok();

	/* process command line args */
	if(q = p = args) {
		n = 1;
		p += 256;
		_argv = q;
		*q++ = p;
		t = *((char **) (_base + 0x24));	/* parent's basepage */
		t = *((char **) (t + 0x20));		/* dta buffer */
		t += 30;				/* name field */
		while(*p++ = *t++)			/* make argv[0] */
			;
		for(t=p; cmdlen--; *p++ = *cmdline++)
			;
		*p = '\0';
		p = strtok(t, " \t");
		while(p) {
#ifdef REDIRECT
			switch(*p) {
				case '<':	/* redirect stdin */
					i = open(++p, _OPNr);
					if(i < 0) {
						Cconws("Can't open ");
						Cconws(p);
						Cconws(" as stdin.\r\n");
						exit(ERROR);
					}
					stdin->F_h = i;
					stdin->F_stat &= ~F_DEVICE;
					break;
				case '>':	/* redirect stdout */
					j = FALSE;
					if(*++p == '>') {	/* append */
						i = open(++p, _OPNw);
						j = TRUE;
					}
					else
						i = creat(p, _CRErw);
					if(i < 0) {
						Cconws("Can't open ");
						Cconws(p);
						Cconws(" as stdout.\r\n");
						exit(ERROR);
					}
					if(j)
						lseek(i, 0L, _LSKend);
					stdout->F_h = i;
					stdout->F_stat &= ~F_DEVICE;
					break;
				default:	/* normal argument */
					++n;
					*q++ = p;
			}
#else
			++n;
			*q++ = p;
#endif
			p = strtok(NULL, " \t");
		}
		_argc = n;
	}
}

/*------------------------------------------------------------------------*/
/*	The following are alternate _initargs() implementations,	  */
/*	each of which has certain features that may be useful in	  */
/*	various applications.				-Dal		  */
/*------------------------------------------------------------------------*/

#ifdef NEVER

/*
 *	This _initargs() function implements redirection and wildcard
 *	filename expansion, and allocates space for arguments dynamically.
 *	It does not support quoting of any kind.  This should be added.
 *						-Dal  (10/01/87)
 */

#define	ARGLIMIT	64		/* maximum number of arguments */
#define	WILDCARD	TRUE		/* expand wildcards */

extern char	*_base;
extern int	_argc;
extern char	**_argv;
extern char	*_envp;

static char	*args[ARGLIMIT];	/* argv pointer buffer */

void _initargs(cmdline, cmdlen)
char *cmdline;
int cmdlen;
/*
 *	Process command line, handling redirection and wildcards.  This
 *	function is responsible for allocating space to hold the parsed
 *	command line arguments, etc.  Values to be passed to the user's
 *	main() function are stored in the global variables "_argc",
 *	"_argv" and "_envp".
 */
{
    register char *p, **q, *s;
    register int i, j, n;
    STAT dir, *oldirp;
    char tmp[128];
    char *strtok(), *strdup(), *strrpbrk();

    /* process command line args */
    if(q = args) {
	n = 1;
	_argv = q;
	s = *((char **) (_base + 0x24));	/* parent's basepage */
	s = *((char **) (t + 0x20));		/* dta buffer */
	s += 30;				/* name field */
	*q++ = strdup(s);			/* make argv[0] */
	cmdline[cmdlen] = '\0';
	p = strtok(cmdline, " \t");
	while(p && (n < ARGLIMIT)) {
	    switch(*p) {
		case '<':	/* redirect stdin */
			i = open(++p, _OPNr);
			if(i < 0) {
				Cconws("Can't open ");
				Cconws(p);
				Cconws(" as stdin.\r\n");
				exit(ERROR);
			}
			stdin->F_h = i;
			stdin->F_stat &= ~F_DEVICE;
			break;
		case '>':	/* redirect stdout */
			j = FALSE;
			if(*++p == '>') {	/* append */
				i = open(++p, _OPNw);
				j = TRUE;
			}
			else
				i = creat(p, _CRErw);
			if(i < 0) {
				Cconws("Can't open ");
				Cconws(p);
				Cconws(" as stdout.\r\n");
				exit(ERROR);
			}
			if(j)
				lseek(i, 0L, _LSKend);
			stdout->F_h = i;
			stdout->F_stat &= ~F_DEVICE;
			break;
		default:
#ifdef WILDCARD
			if(strchr(p, '*') || strchr(p, '?')) {
				oldirp = Fgetdta();
				Fsetdta(&dir);
				if(Fsfirst(p, 0x00) == 0) {
					if(s = strrpbrk(p, ":\\"))
						*++s = '\0';
					else
						*p = '\0';
					do {
						++n;
						strcpy(tmp, p);
						strcat(tmp, dir.S_name);
						*q++ = strdup(tmp);
					} while(Fsnext() == 0);
				}
				Fsetdta(oldirp);
			}
			else {			/* normal argument */
				++n;
				*q++ = strdup(p);
			}
#else /* !WILDCARD */
			++n;
			*q++ = strdup(p);
#endif /* WILDCARD */
	    }
	    p = strtok(NULL, " \t");
	}
	_argc = n;
    }
}

#endif /* NEVER */
