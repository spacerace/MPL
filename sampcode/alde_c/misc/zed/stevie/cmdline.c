/*
 * STevie - ST editor for VI enthusiasts.   ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

static	char	*altfile = NULL;	/* alternate file */
static	int	altline;		/* line # in alternate file */

static	char	*nowrtmsg = "No write since last change (use ! to override)";

extern	char	**files;		/* used for "n" and "rew" */
extern	int	numfiles, curfile;

/*
 * The next two variables contain the bounds of any range given in a
 * command. If no range was given, both contain null line pointers.
 * If only a single line was given, u_pos will contain a null line
 * pointer.
 */
static	LPTR	l_pos, u_pos;

static	bool_t	interactive;	/* TRUE if we're reading a real command line */

#define	CMDSZ	100		/* size of the command buffer */

static	bool_t	doecmd();
static	void	badcmd(), doshell(), get_range();
static	LPTR	*get_line();

#ifdef	MEGAMAX
overlay "cmdline"
#endif

/*
 * readcmdline() - accept a command line starting with ':', '/', or '?'
 *
 * readcmdline() accepts and processes colon commands and searches. If
 * 'cmdline' is null, the command line is read here. Otherwise, cmdline
 * points to a complete command line that should be used. This is used
 * in main() to handle initialization commands in the environment variable
 * "EXINIT".
 */
void
readcmdline(firstc, cmdline)
int	firstc;		/* either ':', '/', or '?' */
char	*cmdline;	/* optional command string */
{
	int c;
	char buff[CMDSZ];
	char *p, *q, *cmd, *arg;

	/*
	 * Clear the range variables.
	 */
	l_pos.linep = (struct line *) NULL;
	u_pos.linep = (struct line *) NULL;

	interactive = (cmdline == NULL);

	if (interactive)
		gotocmd(1,1,firstc);
	p = buff;
	if ( firstc != ':' )
		*p++ = firstc;

	if (interactive) {
		/* collect the command string, handling '\b' and @ */
		for ( ; ; ) {
			c = vgetc();
			if ( c=='\n'||c=='\r'||c==EOF )
				break;
			if ( c=='\b' ) {
				if ( p > buff ) {
					p--;
					/* this is gross, but it relies
					 * only on 'gotocmd'
					 */
					gotocmd(1,0,firstc==':'?':':0);
					for ( q=buff; q<p; q++ )
						outchar(*q);
				} else {
					msg("");
					return;		/* back to cmd mode */
				}
				continue;
			}
			if ( c=='@' ) {
				p = buff;
				gotocmd(1,1,firstc);
				continue;
			}
			outchar(c);
			*p++ = c;
		}
		*p = '\0';
	} else {
		if (strlen(cmdline) > CMDSZ-2)	/* should really do something */
			return;			/* better here... */
		strcpy(p, cmdline);
	}

	/* skip any initial white space */
	for ( cmd = buff; *cmd != NUL && isspace(*cmd); cmd++ )
		;

	/* search commands */
	c = *cmd;
	if ( c == '/' || c == '?' ) {
		cmd++;
		if ( *cmd == c ) {
			/* the command was '//' or '??' */
			repsearch();
			return;
		}
		/* If there is a matching '/' or '?' at the end, toss it */
		p = strchr(cmd, NUL);
		if ( *(p-1) == c && *(p-2) != '\\' )
			*(p-1) = NUL;
		dosearch((c == '/') ? FORWARD : BACKWARD, cmd);
		return;
	}

	/*
	 * Parse a range, if present (and update the cmd pointer).
	 */
	get_range(&cmd);

	/* isolate the command and find any argument */
	for ( p=cmd; *p != NUL && ! isspace(*p); p++ )
		;
	if ( *p == NUL )
		arg = NULL;
	else {
		*p = NUL;
		for (p++; *p != NUL && isspace(*p) ;p++)
			;
		arg = p;
		if ( *arg == '\0' )
			arg = NULL;
	}
	if ( strcmp(cmd,"q!")==0 )
		getout();
	if ( strcmp(cmd,"q")==0 ) {
		if ( Changed )
			emsg(nowrtmsg);
		else
			getout();
		return;
	}
	if ( strcmp(cmd,"w")==0 ) {
		if ( arg == NULL ) {
			if (Filename != NULL) {
				writeit(Filename, &l_pos, &u_pos);
				UNCHANGED;
			} else
				emsg("No output file");
		}
		else
			writeit(arg, &l_pos, &u_pos);
		return;
	}
	if ( strcmp(cmd,"wq")==0 ) {
		if (Filename != NULL) {
			if ( writeit(Filename, NULL, NULL) )
				getout();
		} else
			emsg("No output file");
		return;
	}
	if ( strcmp(cmd, "x") == 0 ) {
		if (Changed) {
			if (Filename != NULL) {
				if (!writeit(Filename, NULL, NULL))
					return;
			} else {
				emsg("No output file");
				return;
			}
		}
		getout();
	}
	if ( strcmp(cmd,"f")==0 && arg==NULL ) {
		fileinfo();
		return;
	}
	if ( *cmd == 'n' ) {
		if ( (curfile + 1) < numfiles ) {
			/*
			 * stuff ":e[!] FILE\n"
			 */
			stuffin(":e");
			if (cmd[1] == '!')
				stuffin("!");
			stuffin(" ");
			stuffin(files[++curfile]);
			stuffin("\n");
		} else
			emsg("No more files!");
		return;
	}
	if ( *cmd == 'p' ) {
		if ( curfile > 0 ) {
			/*
			 * stuff ":e[!] FILE\n"
			 */
			stuffin(":e");
			if (cmd[1] == '!')
				stuffin("!");
			stuffin(" ");
			stuffin(files[--curfile]);
			stuffin("\n");
		} else
			emsg("No more files!");
		return;
	}
	if ( strncmp(cmd, "rew", 3) == 0) {
		if (numfiles <= 1)		/* nothing to rewind */
			return;
		curfile = 0;
		/*
		 * stuff ":e[!] FILE\n"
		 */
		stuffin(":e");
		if (cmd[3] == '!')
			stuffin("!");
		stuffin(" ");
		stuffin(files[0]);
		stuffin("\n");
		return;
	}
	if ( strcmp(cmd,"e") == 0 || strcmp(cmd,"e!") == 0 ) {
		doecmd(arg, cmd[1] == '!');
		return;
	}
	if ( strcmp(cmd,"f") == 0 ) {
		Filename = strsave(arg);
		filemess("");
		return;
	}
	if ( strcmp(cmd,"r") == 0 || strcmp(cmd,".r") == 0 ) {
		if ( arg == NULL ) {
			badcmd();
			return;
		}
		if (readfile(arg, Curschar, 1)) {
			emsg("Can't open file");
			return;
		}
		updatescreen();
		CHANGED;
		return;
	}
	if ( strcmp(cmd,".=")==0 ) {
		char messbuff[80];
		sprintf(messbuff,"line %d", cntllines(Filemem,Curschar));
		msg(messbuff);
		return;
	}
	if ( strcmp(cmd,"$=")==0 ) {
		char messbuff[8];
		sprintf(messbuff, "%d", cntllines(Filemem, Fileend)-1);
		msg(messbuff);
		return;
	}
	if ( strncmp(cmd,"ta", 2) == 0 ) {
		dotag(arg, cmd[2] == '!');
		return;
	}
	if ( strcmp(cmd,"set")==0 ) {
		doset(arg, interactive);
		return;
	}
	if ( strcmp(cmd,"help")==0 ) {
		if (help()) {
			screenclear();
			updatescreen();
		}
		return;
	}
	if ( strcmp(cmd, "version") == 0) {
		extern	char	*Version;

		msg(Version);
		return;
	}
	if ( strcmp(cmd, "sh") == 0) {
		doshell();
		return;
	}
	/*
	 * If we got a line, but no command, then go to the line.
	 */
	if (*cmd == NUL && l_pos.linep != NULL) {
		*Curschar = l_pos;
		cursupdate();
		return;
	}

	badcmd();
}

/*
 * get_range - parse a range specifier
 *
 * Ranges are of the form:
 *
 * addr[,addr]
 *
 * where 'addr' is:
 *
 * $  [+- NUM]
 * 'x [+- NUM]	(where x denotes a currently defined mark)
 * .  [+- NUM]
 * NUM
 *
 * The pointer *cp is updated to point to the first character following
 * the range spec. If an initial address is found, but no second, the
 * upper bound is equal to the lower.
 */
static void
get_range(cp)
char	**cp;
{
	LPTR	*l;
	char	*p;

	if ((l = get_line(cp)) == NULL)
		return;

	l_pos = *l;

	for (p = *cp; *p != NUL && isspace(*p) ;p++)
		;

	*cp = p;

	if (*p != ',') {		/* is there another line spec ? */
		u_pos = l_pos;
		return;
	}

	*cp = ++p;

	if ((l = get_line(cp)) == NULL) {
		u_pos = l_pos;
		return;
	}

	u_pos = *l;
}

static LPTR *
get_line(cp)
char	**cp;
{
	static	LPTR	pos;
	LPTR	*lp;
	char	*p, c;
	int	lnum;

	pos.index = 0;		/* shouldn't matter... check back later */

	p = *cp;
	/*
	 * Determine the basic form, if present.
	 */
	switch (c = *p++) {

	case '$':
		pos.linep = Fileend->linep->prev;
		break;

	case '.':
		pos.linep = Curschar->linep;
		break;

	case '\'':
		if ((lp = getmark(*p++)) == NULL) {
			emsg("Unknown mark");
			return (LPTR *) NULL;
		}
		pos = *lp;
		break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		for (lnum = c - '0'; isdigit(*p) ;p++)
			lnum = (lnum * 10) + (*p - '0');

		pos = *gotoline(lnum);
		break;

	default:
		return (LPTR *) NULL;
	}

	while (*p != NUL && isspace(*p))
		p++;

	if (*p == '-' || *p == '+') {
		bool_t	neg = (*p++ == '-');

		for (lnum = 0; isdigit(*p) ;p++)
			lnum = (lnum * 10) + (*p - '0');

		if (neg)
			lnum = -lnum;

		pos = *gotoline( cntllines(Filemem, &pos) + lnum );
	}

	*cp = p;
	return &pos;
}

static void
badcmd()
{
	if (interactive)
		emsg("Unrecognized command");
}

#define	LSIZE	512	/* max. size of a line in the tags file */

/*
 * dotag(tag, force) - goto tag
 */
void
dotag(tag, force)
char	*tag;
bool_t	force;
{
	FILE	*tp, *fopen();
	char	lbuf[LSIZE];
	char	*fname, *str;

	if ((tp = fopen("tags", "r")) == NULL) {
		emsg("Can't open tags file");
		return;
	}

	while (fgets(lbuf, LSIZE, tp) != NULL) {
	
		if ((fname = strchr(lbuf, TAB)) == NULL) {
			emsg("Format error in tags file");
			return;
		}
		*fname++ = '\0';
		if ((str = strchr(fname, TAB)) == NULL) {
			emsg("Format error in tags file");
			return;
		}
		*str++ = '\0';

		if (strcmp(lbuf, tag) == 0) {
			if (doecmd(fname, force)) {
				stuffin(str);		/* str has \n at end */
				stuffin("\007");	/* CTRL('G') */
				fclose(tp);
				return;
			}
		}
	}
	emsg("tag not found");
	fclose(tp);
}

static	bool_t
doecmd(arg, force)
char	*arg;
bool_t	force;
{
	int	line = 1;		/* line # to go to in new file */

	if (!force && Changed) {
		emsg(nowrtmsg);
		return FALSE;
	}
	if ( arg != NULL ) {
		/*
		 * First detect a ":e" on the current file. This is mainly
		 * for ":ta" commands where the destination is within the
		 * current file.
		 */
		if (Filename != NULL && strcmp(arg, Filename) == 0) {
			if (!Changed || (Changed && !force))
				return TRUE;
		}
		if (strcmp(arg, "#") == 0) {	/* alternate */
			char	*s = Filename;

			if (altfile == NULL) {
				emsg("No alternate file");
				return FALSE;
			}
			Filename = altfile;
			altfile  = s;
			line = altline;
			altline = cntllines(Filemem, Curschar);
		} else {
			altfile = Filename;
			altline = cntllines(Filemem, Curschar);
			Filename = strsave(arg);
		}
	}
	if (Filename == NULL) {
		emsg("No filename");
		return FALSE;
	}

	/* clear mem and read file */
	freeall();
	filealloc();
	UNCHANGED;

	readfile(Filename, Filemem, 0);
	*Topchar = *Curschar;
	if (line != 1) {
		stuffnum(line);
		stuffin("G");
	}
	setpcmark();
	updatescreen();
	return TRUE;
}

static void
doshell()
{
	char	*sh, *getenv();

	if ((sh = getenv("SHELL")) == NULL) {
		emsg("Shell variable not set");
		return;
	}
	gotocmd(TRUE, FALSE, 0);

	if (system(sh) < 0) {
		emsg("Exec failed");
		return;
	}

	wait_return();
}

void
gotocmd(clr, fresh, firstc)
bool_t  clr, fresh;
char	firstc;
{
	int n;

	windgoto(Rows-1,0);
	if ( clr )
		outstr(T_EL);		/* clear the bottom line */
	if ( firstc )
		outchar(firstc);
}

/*
 * msg(s) - displays the string 's' on the status line
 */
void
msg(s)
char *s;
{
	gotocmd(TRUE, TRUE, 0);
	outstr(s);
}

void
smsg(s, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char	*s;
int	a1, a2, a3, a4, a5, a6, a7, a8, a9;
{
	char	sbuf[80];

	sprintf(sbuf, s, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	msg(sbuf);
}

/*
 * emsg() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 */
void
emsg(s)
char	*s;
{
	if (P(P_EB))
		beep();
	msg(s);
}

void
wait_return()
{
	char	c;

	outstr("Press RETURN to continue");
	do {
		c = vgetc();
	} while (c != '\r' && c != '\n');

	screenclear();
	updatescreen();
}
