
/*
 *		G o t h i c   P r i n t e r
 *
 *
 * Compile with gottab.c
 *
 * Prints the arguments, if none, it prompts for an output file and
 * reads stdin.
 *
 *	If the first argument is -h, the display is halved.
 *
 *	/ in the text reverses the background
 *
 * The following characters are displayed:
 *
 *	#.(@!$);-,?:'"  A-Z  a-z  0-9
 *
 * All others become blanks.
 *
 */
#include <stdio.h>

char		outbuf[133];
char		line[133];
int		background	= 0;		/* Background color	*/
int		half		= 0;		/* Set to halve display	*/

main (argc, argv)
int		argc;
char		*argv[];

{
	register int		i;

	if (argc > 1 && argv[1][0] == '-' && (argv[1][1] | 040) == 'h') {
		argc--;
		argv++;
		half++;
	}
	if (argc <= 1) {
		printf("Gothic output file <terminal>: ");
		fflush(stdout);
		if (gets(outbuf) == NULL)
			panic("No output file", NULL);
		if (half == 0) {
			printf("Half size (Yes/No) <N>: ");
			fflush(stdout);
			if (gets(line) == NULL)
				panic("Unexpected EOF", NULL);
			if ((line[0] | 040) == 'y')
				half++;
		}
		if (outbuf[0] != 0) {
			if (freopen(outbuf, "w", stdout) == NULL)
				panic("Can't open", outbuf);
		}
		while (gets(line) != NULL) {
			dotext(line);
		}
	}
	else {
		for (i = 1; i < argc; i++) {
			dotext(argv[i]);
		}
		puts("\f");
	}
}

dotext(text)
char		*text;
/*
 * Convert text to gothic letters, write them to stdout
 */
{
	register char	*tp;
	register int	c;

	for (tp = text; (c = *tp++) != 0;) {
		if (c == '/')
			background = (background) ? 0 : 2;
		else	gothic(c);
	}
}

gothic(character)
int		character;
/*
 * Process the character
 */
{
	char			*gp;
	register char		*op;
	register int		i;
	register int		byte;
	int			index;
	extern char		*gottab[];
	int			lhalf;		/* Line half flag	*/
	int			chalf;		/* Column half flag	*/
		
	index = (background) ? -1 : 0;
	lhalf = chalf = 0;
	gp = gottab[character & 127];
	op = outbuf;
	for (;;) {
		if ((i = (*gp++ & 0377)) >= 254) {
			if (background) {
				while (op < &outbuf[132])
					*op++ = 'X';
			}
			*op = 0;
			lhalf = ~lhalf;
			chalf = 0;
			if (half) {
				if (lhalf) {
					outbuf[132 / 2] = 0;
					puts(outbuf);
				}
			}
			else {
				puts(outbuf);
			}
			op = outbuf;
			if (i == 255)
				break;
		}
		else {
			byte = "O X "[background - (index = (-1 - index))];
			while (--i >= 0) {
				if (half) {
					chalf = ~chalf;
					if (chalf) {
						*op++ = byte;
					}
				}
				else {
					*op++ = byte;
				}
			}
			if (op >= &outbuf[132])
				panic("big line", NULL);
		}
	}
}

panic(s, arg)
char		*s;
char		*arg;
/*
 * Fatal error exit
 */
{
	fprintf(stderr, "?Gothic-E-fatal error %s", s);
	if (arg != NULL)
		fprintf(stderr, ": \"%s\"", arg);
	fprintf(stderr, "\n");
	exit(1);
}
