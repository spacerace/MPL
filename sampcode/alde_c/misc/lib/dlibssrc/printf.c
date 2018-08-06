#include <stdio.h>
#include <ctype.h>

_printf(op, put, fmt, args)
char *op;
unsigned int (*put)();
register unsigned char *fmt;
register unsigned int *args;
/*
 *	<fmt> points to a format control string.  <args> pointers to a
 *	list of arguments.  The format string is used to create and output
 *	stream with the arguments.  The <put> function is used to output
 *	each character.  The <op> parameter is given to the <put> function
 *	to specify the output stream.  Calls to <put> are of the form:
 *	"(*put)(c, op);" where <c> is the character to output.  The format
 *	string is composed of characters and format specifications.  The
 *	'%' character introduces a format specifier.  The general form of
 *	a format specifier is:
 *
 *	    %[-][ |+][0][<width>|*][.[<precision>|*]][l]{d|i|u|o|x|b|c|s}
 *
 *	The '-' specifies left justification.  The ' ' or '+' specifies
 *	the character which preceeds positive numeric values.  The '0'
 *	specifies that numeric fields will be padded with '0' rather than
 *	' '.  The <width> field is a numeric value specifying a minimum
 *	field width.  The <precision> field is a numeric value specifying
 *	the maximum number of data characters to display.  If '*' is
 *	specified for the width or the precision, an "int" value is taken
 *	from the argument list and used for that value.  If no width is
 *	specified, the field width varies according to the data width.  If
 *	no precision is specified, all data characters are included in the
 *	data width.  If the data width exceeds the field width, the field
 *	width will expand to allow all data characters to be printed.
 *	Including the 'l' or capitalizing the trailing character specifies
 *	that the associated value is a "long" type.  The trailing character
 *	specifies the format type, as follows:
 *
 *		d	Signed decimal integer
 *		i	same as 'd'
 *		u	Unsigned decimal integer
 *		o	Unsigned octal integer
 *		x	Unsigned hexadecimal integer
 *		b	Unsigned binary integer
 *		c	Character
 *		s	String
 *
 *	If the character following the '%' is not recognized, it is
 *	simply passed along to the output stream, thus "%%" is used to
 *	print a single '%' character.
 */
{
	register int i, cnt = 0, ljustf, lval;
	int preci, dpoint, width;
	char pad, sign, radix;
	register char *ptmp;
	char tmp[64], *ltoa(), *ultoa();

	while(*fmt) {
		if(*fmt == '%') {
			ljustf = FALSE;	/* left justify flag */
			sign = '\0';	/* sign char & status */
			pad = ' ';	/* justification padding char */
			width = -1;	/* min field width */
			dpoint = FALSE;	/* found decimal point */
			preci = -1;	/* max data width */
			radix = 10;	/* number base */
			ptmp = tmp;	/* pointer to area to print */
			lval = FALSE;	/* long value flaged */
fmtnxt:
			i = 0;
			while (isdigit(*++fmt))	{
				i = (i * 10) + (*fmt - '0');
				if (dpoint)
					preci = i;
				else if (!i && (pad == ' ')) {
					pad = '0';
					goto fmtnxt;
				}
				else
					width = i;
			}

			switch(*fmt) {
				case '\0':	/* early EOS */
					--fmt;
					goto charout;

				case '-':	/* left justification */
					ljustf = TRUE;
					goto fmtnxt;

				case ' ':
				case '+':	/* leading sign flag */
					sign = *fmt;
					goto fmtnxt;

				case '*':	/* parameter width value */
					i = *args++;
					if (dpoint)
						preci = i;
					else
						width = i;
					goto fmtnxt;

				case '.':	/* secondary width field */
					dpoint = TRUE;
					goto fmtnxt;

				case 'l':	/* long data */
					lval = TRUE;
					goto fmtnxt;

				case 'd':	/* Signed decimal */
				case 'i':
					ltoa((long)((lval)
						?(*((long *) args)++)
						:(*((int  *) args)++)),
					      ptmp, 10);
					goto printit;

				case 'b':	/* Unsigned binary */
					radix = 2;
					goto usproc;

				case 'o':	/* Unsigned octal */
					radix = 8;
					goto usproc;

				case 'x':	/* Unsigned hexadecimal */
				case 'X':
					radix = 16;
					/* fall thru */

				case 'u':	/* Unsigned decimal */
usproc:
					ultoa((unsigned long)((lval)
						?(*((unsigned long *) args)++)
						:(*((unsigned int  *) args)++)),
					      ptmp, radix);
					if (*fmt == 'x')
						strlwr(ptmp, ptmp);
					goto printit;

				case 'c':	/* Character */
					ptmp[0] = *args++;
					ptmp[1] = '\0';
					goto nopad;

				case 's':	/* String */
					ptmp = *((char **) args)++;
nopad:
					sign = '\0';
					pad  = ' ';
printit:
					cnt += _prtfld(op, put, ptmp, ljustf,
						       sign, pad, width, preci);
					break;

				default:	/* unknown character */
					goto charout;
			}
		}
		else {
charout:
			(*put)(*fmt, op);		/* normal char out */
			++cnt;
		}
		++fmt;
	}
	return(cnt);
}
