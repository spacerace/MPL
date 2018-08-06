/*
 * STevie - ST editor for VI enthusiasts.    ...Tim Thompson...twitch!tjt...
 *
 * Extensive modifications by:  Tony Andrews       onecom!wldrdg!tony
 * Turbo C 1.5 port by: Denny Muscatelli 061988
 */

#include "stevie.h"

#include <regexp.h>	/* Henry Spencer's regular expression routines */

#ifdef	MEGAMAX
overlay "search"
#endif

/*
 * This file contains various searching-related routines. These fall into
 * three groups: string searches (for /, ?, n, and N), character searches
 * within a single line (for f, F, t, T, etc), and "other" kinds of searches
 * like the '%' command, and 'word' searches.
 */

/*
 * String searches
 *
 * The actual searches are done using Henry Spencer's regular expression
 * library.
 */

#define	BEGWORD	"([^a-zA-Z0-9_]|^)"	/* replaces "\<" in search strings */
#define	ENDWORD	"([^a-zA-Z0-9_]|$)"	/* likewise replaces "\>" */

bool_t	begword;	/* does the search include a 'begin word' match */

/*
 * mapstring(s) - map special backslash sequences
 */
static char *
mapstring(s)
register char	*s;
{
	static	char	ns[80];
	char	*p;

	begword = FALSE;

	for (p = ns; *s ;s++) {
		if (*s != '\\') {	/* not an escape */
			*p++ = *s;
			continue;
		}
		switch (*++s) {
		case '/':
			*p++ = '/';
			break;

		case '<':
			strcpy(p, BEGWORD);
			p += strlen(BEGWORD);
			begword = TRUE;
			break;

		case '>':
			strcpy(p, ENDWORD);
			p += strlen(ENDWORD);
			break;

		default:
			*p++ = '\\';
			*p++ = *s;
			break;
		}
	}
	*p++ = NUL;

	return ns;
}

static char *laststr = NULL;
static int lastsdir;

static LPTR *
ssearch(dir,str)
int	dir;	/* FORWARD or BACKWARD */
char	*str;
{
	static LPTR *bcksearch(), *fwdsearch();
	LPTR	*pos;

	if ( laststr != NULL )
		free(laststr);
	laststr = strsave(str);
	lastsdir = dir;
	if ( dir == BACKWARD )
		pos = bcksearch(mapstring(str));
	else
		pos = fwdsearch(mapstring(str));

	/*
	 * This is kind of a kludge, but its needed to make
	 * 'beginning of word' searches land on the right place.
	 */
	if (begword) {
		if (pos->index != 0)
			pos->index += 1;
	}
	return pos;
}

void
dosearch(dir,str)
int dir;
char *str;
{
	LPTR *p;

	if ((p = ssearch(dir,str)) == NULL)
		msg("Pattern not found");
	else {
		LPTR savep;

		cursupdate();
		/* if we're backing up, we make sure the line we're on */
		/* is on the screen. */
		setpcmark();
		*Curschar = savep = *p;
		cursupdate();
	}
}

#define	OTHERDIR(x)	(((x) == FORWARD) ? BACKWARD : FORWARD)

void
repsearch(flag)
int	flag;
{
	int	dir = lastsdir;

	if ( laststr == NULL )
		beep();
	else
		dosearch(flag ? OTHERDIR(lastsdir) : lastsdir, laststr);

	lastsdir = dir;
}

/*
 * regerror - called by regexp routines when errors are detected.
 */
void
regerror(s)
char	*s;
{
	emsg(s);
}

static LPTR *
fwdsearch(str)
register char *str;
{
	static LPTR infile;
	register LPTR *p;
	regexp *prog;
	bool_t	want_start = (*str == '^');	/* looking for start of line? */

	register char *s;
	register int i;

	if ((prog = regcomp(str)) == NULL) {
		emsg("Invalid search string");
		return NULL;
	}

	p = Curschar;
	i = Curschar->index + 1;
	do {
		s = p->linep->s + i;
		i = 0;

		if (regexec(prog, s)) {		/* got a match */
			/*
			 * If we wanted the start of a line and we aren't
			 * really there, then a match doesn't count.
			 */
			if (want_start && (s != p->linep->s))
				continue;

			infile.linep = p->linep;
			infile.index = (int) (prog->startp[0] - p->linep->s);
			free(prog);
			return (&infile);
		}
	} while ((p = nextline(p)) != NULL);

	/*
	 * If wrapscan isn't set, then don't scan from the beginning
	 * of the file. Just return failure here.
	 */
	if (!P(P_WS)) {
		free(prog);
		return NULL;
	}

	/* search from the beginning of the file to Curschar */
	for (p = Filemem; p != NULL ;p = nextline(p)) {
		s = p->linep->s;

		if (regexec(prog, s)) {		/* got a match */
			infile.linep = p->linep;
			infile.index = (int) (prog->startp[0] - s);
			free(prog);
			return (&infile);
		}

		if (p->linep == Curschar->linep)
			break;
	}

	free(prog);
	return(NULL);
}

static LPTR *
bcksearch(str)
char *str;
{
	static LPTR infile;
	register LPTR *p;
	regexp	*prog;
	register char *s;
	register int i;
	bool_t	want_start = (*str == '^');	/* looking for start of line? */
	register char	*match;

	/* make sure str isn't empty */
	if (str == NULL || *str == NUL)
		return NULL;

	if ((prog = regcomp(str)) == NULL) {
		emsg("Invalid search string");
		return NULL;
	}

	p = Curschar;
	dec(p);

	if (begword)		/* so we don't get stuck on one match */
		dec(p);

	i = (want_start) ? 0 : p->index;

	do {
		s = p->linep->s;

		if (regexec(prog, s)) {		/* match somewhere on line */

			if (want_start) {	/* could only have been one */
				infile.linep = p->linep;
				infile.index = (int) (prog->startp[0] - s);
				free(prog);
				return (&infile);
			}

			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one
			 * before the cursor, if we're on that line.
			 */

			match = prog->startp[0];

			while (regexec(prog, prog->endp[0])) {
				if ((i >= 0) && ((prog->startp[0] - s) > i))
					break;
				match = prog->startp[0];
			}

			if ((i >= 0) && ((match - s) > i)) {
				i = -1;
				continue;
			}

			infile.linep = p->linep;
			infile.index = (int) (match - s);
			free(prog);
			return (&infile);
		}
		i = -1;

	} while ((p = prevline(p)) != NULL);

	/*
	 * If wrapscan isn't set, bag the search now
	 */
	if (!P(P_WS)) {
		free(prog);
		return NULL;
	}

	/* search backward from the end of the file */
	p = prevline(Fileend);
	do {
		s = p->linep->s;

		if (regexec(prog, s)) {		/* match somewhere on line */

			if (want_start) {	/* could only have been one */
				infile.linep = p->linep;
				infile.index = (int) (prog->startp[0] - s);
				free(prog);
				return (&infile);
			}

			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one.
			 */

			match = prog->startp[0];

			while (regexec(prog, prog->endp[0]))
				match = prog->startp[0];

			infile.linep = p->linep;
			infile.index = (int) (match - s);
			free(prog);
			return (&infile);
		}

		if (p->linep == Curschar->linep)
			break;

	} while ((p = prevline(p)) != NULL);

	free(prog);
	return NULL;
}

/*
 * Character Searches
 */

static char lastc = NUL;	/* last character searched for */
static int  lastcdir;		/* last direction of character search */
static int  lastctype;		/* last type of search ("find" or "to") */

/*
 * searchc(c, dir, type)
 *
 * Search for character 'c', in direction 'dir'. If type is 0, move to
 * the position of the character, otherwise move to just before the char.
 */
bool_t
searchc(c, dir, type)
char	c;
int	dir;
int	type;
{
	LPTR	save;

	save = *Curschar;	/* save position in case we fail */
	lastc = c;
	lastcdir = dir;
	lastctype = type;

	/*
	 * On 'to' searches, skip one to start with so we can repeat
	 * searches in the same direction and have it work right.
	 */
	if (type)
		(dir == FORWARD) ? oneright() : oneleft();

	while ( (dir == FORWARD) ? oneright() : oneleft() ) {
		if (gchar(Curschar) == c) {
			if (type)
				(dir == FORWARD) ? oneleft() : oneright();
			return TRUE;
		}
	}
	*Curschar = save;
	return FALSE;
}

bool_t
crepsearch(flag)
int	flag;
{
	int	dir = lastcdir;
	int	rval;

	if (lastc == NUL)
		return FALSE;

	rval = searchc(lastc, flag ? OTHERDIR(lastcdir) : lastcdir, lastctype);

	lastcdir = dir;		/* restore dir., since it may have changed */

	return rval;
}

/*
 * "Other" Searches
 */

/*
 * showmatch - move the cursor to the matching paren or brace
 */
LPTR *
showmatch()
{
	static	LPTR	pos;
	int	(*move)(), inc(), dec();
	char	initc = gchar(Curschar);	/* initial char */
	char	findc;				/* terminating char */
	char	c;
	int	count = 0;

	pos = *Curschar;		/* set starting point */

	switch (initc) {

	case '(':
		findc = ')';
		move = inc;
		break;
	case ')':
		findc = '(';
		move = dec;
		break;
	case '{':
		findc = '}';
		move = inc;
		break;
	case '}':
		findc = '{';
		move = dec;
		break;
	case '[':
		findc = ']';
		move = inc;
		break;
	case ']':
		findc = '[';
		move = dec;
		break;
	default:
		return (LPTR *) NULL;
	}

	while ((*move)(&pos) != -1) {		/* until end of file */
		c = gchar(&pos);
		if (c == initc)
			count++;
		else if (c == findc) {
			if (count == 0)
				return &pos;
			count--;
		}
	}
	return (LPTR *) NULL;			/* never found it */
}

/*
 * findfunc(dir) - Find the next function in direction 'dir'
 *
 * Return TRUE if a function was found.
 */
bool_t
findfunc(dir)
int	dir;
{
	LPTR	*curr;

	curr = Curschar;

	do {
		curr = (dir == FORWARD) ? nextline(curr) : prevline(curr);

		if (curr != NULL && curr->linep->s[0] == '{') {
			setpcmark();
			*Curschar = *curr;
			return TRUE;
		}
	} while (curr != NULL);

	return FALSE;
}

/*
 * The following routines do the word searches performed by the
 * 'w', 'W', 'b', 'B', 'e', and 'E' commands.
 */

/*
 * To perform these searches, characters are placed into one of three
 * classes, and transitions between classes determine word boundaries.
 *
 * The classes are:
 *
 * 0 - white space
 * 1 - letters, digits, and underscore
 * 2 - everything else
 */

static	int	stype;		/* type of the word motion being performed */

#define	C0(c)	(((c) == ' ') || ((c) == '\t') || ((c) == NUL))
#define	C1(c)	(isalpha(c) || isdigit(c) || ((c) == '_'))

/*
 * cls(c) - returns the class of character 'c'
 *
 * The 'type' of the current search modifies the classes of characters
 * if a 'W', 'B', or 'E' motion is being done. In this case, chars. from
 * class 2 are reported as class 1 since only white space boundaries are
 * of interest.
 */
static	int
cls(c)
char	c;
{
	if (C0(c))
		return 0;

	if (C1(c))
		return 1;

	/*
	 * If stype is non-zero, report these as class 1.
	 */
	return (stype == 0) ? 2 : 1;
}


/*
 * fwd_word(pos, type) - move forward one word
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *
fwd_word(p, type)
LPTR	*p;
int	type;
{
	static	LPTR	pos;
	int	sclass = cls(gchar(p));		/* starting class */

	pos = *p;

	stype = type;

	/*
	 * We always move at least one character.
	 */
	if (inc(&pos) == -1)
		return NULL;

	if (sclass != 0) {
		while (cls(gchar(&pos)) == sclass) {
			if (inc(&pos) == -1)
				return NULL;
		}
		/*
		 * If we went from 1 -> 2 or 2 -> 1, return here.
		 */
		if (cls(gchar(&pos)) != 0)
			return &pos;
	}

	/* We're in white space; go to next non-white */

	while (cls(gchar(&pos)) == 0) {
		/*
		 * We'll stop if we land on a blank line
		 */
		if (pos.index == 0 && pos.linep->s[0] == NUL)
			break;

		if (inc(&pos) == -1)
			return NULL;
	}

	return &pos;
}

/*
 * bck_word(pos, type) - move backward one word
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *
bck_word(p, type)
LPTR	*p;
int	type;
{
	static	LPTR	pos;
	int	sclass = cls(gchar(p));		/* starting class */

	pos = *p;

	stype = type;

	if (dec(&pos) == -1)
		return NULL;

	/*
	 * If we're in the middle of a word, we just have to
	 * back up to the start of it.
	 */
	if (cls(gchar(&pos)) == sclass && sclass != 0) {
		/*
		 * Move backward to start of the current word
		 */
		while (cls(gchar(&pos)) == sclass) {
			if (dec(&pos) == -1)
				return NULL;
		}
		inc(&pos);			/* overshot - forward one */
		return &pos;
	}

	/*
	 * We were at the start of a word. Go back to the start
	 * of the prior word.
	 */

	while (cls(gchar(&pos)) == 0) {		/* skip any white space */
		/*
		 * We'll stop if we land on a blank line
		 */
		if (pos.index == 0 && pos.linep->s[0] == NUL)
			return &pos;

		if (dec(&pos) == -1)
			return NULL;
	}

	sclass = cls(gchar(&pos));

	/*
	 * Move backward to start of this word.
	 */
	while (cls(gchar(&pos)) == sclass) {
		if (dec(&pos) == -1)
			return NULL;
	}
	inc(&pos);			/* overshot - forward one */

	return &pos;
}

/*
 * end_word(pos, type) - move to the end of the word
 *
 * There is an apparent bug in the 'e' motion of the real vi. At least
 * on the System V Release 3 version for the 80386. Unlike 'b' and 'w',
 * the 'e' motion crosses blank lines. When the real vi crosses a blank
 * line in an 'e' motion, the cursor is placed on the FIRST character
 * of the next non-blank line. The 'E' command, however, works correctly.
 * Since this appears to be a bug, I have not duplicated it here.
 *
 * Returns the resulting position, or NULL if EOF was reached.
 */
LPTR *
end_word(p, type)
LPTR	*p;
int	type;
{
	static	LPTR	pos;
	int	sclass = cls(gchar(p));		/* starting class */

	pos = *p;

	stype = type;

	if (inc(&pos) == -1)
		return NULL;

	/*
	 * If we're in the middle of a word, we just have to
	 * move to the end of it.
	 */
	if (cls(gchar(&pos)) == sclass && sclass != 0) {
		/*
		 * Move forward to end of the current word
		 */
		while (cls(gchar(&pos)) == sclass) {
			if (inc(&pos) == -1)
				return NULL;
		}
		dec(&pos);			/* overshot - forward one */
		return &pos;
	}

	/*
	 * We were at the end of a word. Go to the end
	 * of the next word.
	 */

	while (cls(gchar(&pos)) == 0) {		/* skip any white space */
		if (inc(&pos) == -1)
			return NULL;
	}

	sclass = cls(gchar(&pos));

	/*
	 * Move forward to end of this word.
	 */
	while (cls(gchar(&pos)) == sclass) {
		if (inc(&pos) == -1)
			return NULL;
	}
	dec(&pos);			/* overshot - forward one */

	return &pos;
}
