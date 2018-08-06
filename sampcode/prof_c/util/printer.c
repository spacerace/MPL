/*
 *	printer -- interface functions for printer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <local\std.h>
#include <local\printer.h>

PRINTER prt;	/* printer data */

/*
 *	setprnt -- install printer codes from configuration
 *	file for printer (defaults to Epson MX/FX series)
 */

#define NSELEM	13

int
setprnt()
{
	int n;
	char *s, line[MAXLINE];
	FILE *fp, *fconfig(char *, char *);

	/* use local or global config file, if any */
	if ((fp = fconfig("CONFIG", "printer.cnf")) != NULL) {
		n = 0;
		while (fgets(line, MAXLINE, fp) != NULL) {
			if ((s = strtok(line, " \t\n")) == NULL)
				return (-1);
			switch (n) {
			case 0:
				strcpy(prt.p_init, s);
				break;
			case 1:
				strcpy(prt.p_bold, s);
				break;
			case 2:
				strcpy(prt.p_ds, s);
				break;
			case 3:
				strcpy(prt.p_ital, s);
				break;
			case 4:
				strcpy(prt.p_cmp, s);
				break;
			case 5:
				strcpy(prt.p_exp, s);
				break;
			case 6:
				strcpy(prt.p_ul, s);
				break;
			case 7:
				strcpy(prt.p_xbold, s); 
				break;
			case 8:
				strcpy(prt.p_xds, s);
				break;
			case 9:
				strcpy(prt.p_xital, s);
				break;
			case 10:
				strcpy(prt.p_xcmp, s);
				break;
			case 11:
				strcpy(prt.p_xexp, s);
				break;
			case 12:
				strcpy(prt.p_xul, s);
				break;
			default:
				/* too many lines */
				return (-1);
			}
			++n;
		}
		if (n != NSELEM)
			/* probably not enough lines */
			return (-1);
	}
	
	/* or use Epson defaults */
	strcpy(prt.p_init, "\033@");	/* hardware reset */
	strcpy(prt.p_bold, "\033E");	/* emphasized mode */
	strcpy(prt.p_ds, "\033G");	/* double-strike mode */
	strcpy(prt.p_ital, "\0334");	/* italic mode */
	strcpy(prt.p_cmp, "\017");	/* condensed mode */
	strcpy(prt.p_exp, "\016");	/* expanded mode */
	strcpy(prt.p_ul, "\033-1");	/* underline mode */
	strcpy(prt.p_xbold, "\033F");	/* cancel emphasized mode */
	strcpy(prt.p_xds, "\033H");	/* cancel double-strike mode */
	strcpy(prt.p_xital, "\0335");	/* cancel italic mode */
	strcpy(prt.p_xcmp, "\022");	/* cancel condensed mode */
	strcpy(prt.p_xexp, "\024");	/* cancel expanded mode */
	strcpy(prt.p_xul, "\033-0");	/* cancel underline mode */

	return (0);
}

/*
 *	clrprnt -- clear printer options to default values
 *	(clears individual options to avoid the "paper creep"
 *	that occurs with repeated printer resets and to avoid
 *	changing the printer's notion of top-of-form position)
 */

int
clrprnt(fout)
FILE *fout;
{
	fputs(prt.p_xbold, fout);	/* cancel emphasized mode */
	fputs(prt.p_xds, fout);		/* cancel double-strike mode */
	fputs(prt.p_xital, fout);	/* cancel italic mode */
	fputs(prt.p_xcmp, fout);	/* cancel condensed mode */
	fputs(prt.p_xexp, fout);	/* cancel expanded mode */
	fputs(prt.p_xul, fout);		/* cancel underline mode */
} /* end clrprnt() */

/*
 *	setfont -- set the printing font to the type specified
 *	by the argument (may be a compound font specification)
 */

int
setfont(ftype, fout)
int ftype;	/* font type specifier */
FILE *fout;	/* output stream */
{
	clrprnt(fout);
	if ((ftype & CONDENSED) == CONDENSED)
		if ((ftype & DOUBLE) == DOUBLE ||
			(ftype & EMPHASIZED) == EMPHASIZED)
			return FAILURE;
		else if (*prt.p_cmp)
			fputs(prt.p_cmp, fout);
	if (*prt.p_ds && (ftype & DOUBLE) == DOUBLE)
		fputs(prt.p_ds, fout);
	if (*prt.p_bold && (ftype & EMPHASIZED) == EMPHASIZED)
		fputs(prt.p_bold, fout);
	if (*prt.p_exp && (ftype & EXPANDED) == EXPANDED)
		fputs(prt.p_exp, fout);
	if (*prt.p_ital && (ftype & ITALICS) == ITALICS)
		fputs(prt.p_ital, fout);
	if (*prt.p_ul && (ftype & UNDERLINE) == UNDERLINE)
		fputs(prt.p_ul, fout);

	return SUCCESS;
} /* end setfont() */
