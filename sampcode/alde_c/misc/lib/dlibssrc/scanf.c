#include <stdio.h>
#include <ctype.h>

extern	char	_numstr[];

/* #define	skip()	do{c=(*get)(ip); if (c<1) goto done;}while(isspace(c))*/

#define	skip()	while(isspace(c)) { if ((c=(*get)(ip))<1) goto done; }

_scanf(ip, get, unget, fmt, args)
register unsigned char *ip;
int (*get)();
int (*unget)();
register unsigned char *fmt;
char **args;

/*
 *	<fmt> points to a format control string.  <args> pointers to a
 *	list of arguments, each of which is the address of a variable in
 *	which input data may be stored.  The format string is used to
 *	control reading of characters from the <get> function.  As each
 *	character is needed <get> is called in the form "c = (*get)(ip);"
 *	where <c> is the character read (negative for errors) and <ip> is
 *	the auxiliary pointer specified by the <ip> parameter.  If a
 *	character needs to be un-gotten, a call to <unget> of the form
 *	"(*unget)(c, ip);" is made.  The format string is composed of
 *	characters and format specifications.  Any characters in <fmt>,
 *	except whitespace characters, which are not part of a format
 *	specifier are expected to be matched one-to-one by characters in
 *	the input stream.  Scanning terminates if a mismatch occurs or if
 *	any call to <get> results in an error.  Whitespace characters
 *	match 0 or more whitespace characters in the input stream.  The
 *	'%' character introduces a format specifier.  The general form of
 *	a format specifier is:
 *
 *		 %[*][<width>][l|h]{d|u|o|x|b|i|c|s}
 *
 *	The '*' specifies that a field is to be scanned by not stored.
 *	No variable pointer should be provided for non-stored format
 *	specs.  The <width> field specifies that maximum number of
 *	characters to be process to fill the given format type.  Less
 *	than <width> characters will be processed if the field ends
 *	before <width> characters have been processed.  A field ends when
 *	either a whitespace character, or a character which does not fit
 *	the specified format, is read.  The preceding 'l' (or
 *	capitalizing the conversion character) specifies that the
 *	associated variable is a "long" type.  The trailing character
 *	specifies the format type, as follows:
 *
 *		d Signed decimal integer
 *		u Unsigned decimal integer
 *		o Unsigned octal integer
 *		x Unsigned hexadecimal integer
 *		b Unsigned binary integer
 *		i Unsigned decimal/octal/hexadecimal/binary integer
 *		c Character
 *		s String
 *
 *	If a <width> is specified with the 'c' format, exactly <width>
 *	characters (including whitespace) are read from the input stream,
 *	and written to a string.  No '\0' character is added If the
 *	character following the '%' is not recognized, it is expected to
 *	match the input stream as a non-format character, thus "%%" is
 *	used to match a single '%' character.
 *
 *	One additional conversion is the brace-format.  Shown as "%[...]",
 *	the '...' represent a list of characters.  If the first character
 *	in the list is a '^', the field contains any characters -not- in
 *	the list (starting with the 1st character after the '^').  If the
 *	first character of the list is not a '^', then the field will
 *	only contain those characters found in the list.  The field will
 *	be stored as a null terminated string.  Unlike most of the other
 *	formats, this conversion does allow the programmer to specify
 *	that whitespace characters will be included in the resulting
 *	string.
 */

{
	register long n;
	register int c, width, lval, cnt = 0;
	int store, neg, base, wide1, endnull;
	register unsigned char *p;
	char unsigned delim[128], digits[17], *q;
	char *strchr(), *strcpy();

	if (!*fmt)
		return(0);

	c = (*get)(ip);
	while(c > 0)
		{
		store = FALSE;
		if (*fmt == '%')
			{
			n	= 0;
			width	= -1;
			wide1	= 1;
			base	= 10;
			lval	= FALSE;
			store	= TRUE;
			endnull	= TRUE;
			neg	= -1;

			strcpy(delim,  "\011\012\013\014\015 ");
			strcpy(digits, _numstr); /* "01234567890ABCDEF" */

			if (fmt[1] == '*')
				{
				store = FALSE;
				++fmt;
				}

			while (isdigit(*++fmt))		/* width digit(s) */
				{
				if (width == -1)
					width = 0;
				wide1 = width = (width * 10) + (*fmt - '0');
				}
			--fmt;
fmtnxt:
			++fmt;
			switch(tolower(*fmt))	/* tolower() is a MACRO! */
				{
				case '*':
					store = FALSE;
					goto fmtnxt;

				case 'l':	/* long data */
					lval = TRUE;
/* for compatability --> */	case 'h':	/* short data */
					goto fmtnxt;

				case 'i':	/* any-base numeric */
					base = 0;
					goto numfmt;

				case 'b':	/* unsigned binary */
					base = 2;
					goto numfmt;

				case 'o':	/* unsigned octal */
					base = 8;
					goto numfmt;

				case 'x':	/* unsigned hexadecimal */
					base = 16;
					goto numfmt;

				case 'd':	/* SIGNED decimal */
					neg = FALSE;
					/* FALL-THRU */

				case 'u':	/* unsigned decimal */
numfmt:					skip();

					if (isupper(*fmt))
						lval = TRUE;

					if (!base)
						{
						base = 10;
						neg = FALSE;
						if (c == '%')
							{
							base = 2;
							goto skip1;
							}
						else if (c == '0')
							{
							c = (*get)(ip);
							if (c < 1)
								goto savnum;
							if ((c != 'x')
							 && (c != 'X'))
								{
								base = 8;
								digits[8]= '\0';
								goto zeroin;
								}
							base = 16;
							goto skip1;
							}
						}

					if ((neg == FALSE) && (base == 10)
					 && ((neg = (c == '-')) || (c == '+')))
						{
skip1:
						c = (*get)(ip);
						if (c < 1)
							goto done;
						}

					digits[base] = '\0';
					p = strchr(digits,toupper(c));

					if (!p && width)
						goto done;

					while (p && width-- && c)
						{
						n = (n * base) + (p - digits);
						c = (*get)(ip);
zeroin:
						p = strchr(digits,toupper(c));
						}
savnum:
					if (store)
						{
						p = *args;
						if (neg == TRUE)
							n = -n;
						if (lval)
							*((long*) p) = n;
						else
							*((int *) p) = n;
						++cnt;
						}
					break;

				case 'c':	/* character data */
					width = wide1;
					endnull	= FALSE;
					delim[0] = '\0';
					goto strproc;

				case '[':	/* string w/ delimiter set */

					/* get delimiters */
					p = delim;
					if (*++fmt == '^')
						fmt++;
					else
						lval = TRUE;

					while ((*p++ = *fmt) != ']')
						if (*fmt++ == '\0')
							goto done;
					*--p = '\0';
					goto strproc;

				case 's':	/* string data */
					skip();
strproc:
					/* process string */
					p = *args;

					while (width--)
						{
						q = strchr(delim, c);
						if (lval ? !q : q)
							break;
						if (store)
							*p++ = c;
						c = (*get)(ip);
						if (width && (c < 1))
							{
							if (endnull)
								*p = '\0';
							goto done;
							}
						}

					if (store)
						{
						if (endnull)
							*p = '\0';
						++cnt;
						}
					break;

				case '\0':	/* early EOS */
					--fmt;
					/* FALL THRU */

				default:
					goto cmatch;
				}
			}
		else if (isspace(*fmt))		/* skip whitespace */
			{
			skip();
			}
		else 
			{			/* normal match char */
cmatch:
			if (c != *fmt) 
				break;
			c = (*get)(ip);
			}

		if (store)
			args++;

		if (!*++fmt)
			break;
		}

done:						/* end of scan */
	if ((c < 1) && (cnt == 0))
		return(EOF);
	(*unget)(c, ip);
	return(cnt);
}

