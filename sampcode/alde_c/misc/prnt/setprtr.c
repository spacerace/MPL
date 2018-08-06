/* --------------------------------------------------------------------- */
/*	SETPRTR - Send printer setup codes to the IBM (MX-80) printer	 */
/*	   Written for public domain distribution in			 */
/*	   Caprock Systems Small-c:pc					 */
/*	   by	Michael Burton						 */
/*		320 Highland Dr. Apt 203				 */
/*		Glen Burnie, MD  21061					 */
/*		(301)768-8358						 */
/*									 */
/*	Version 1.0	09 May 1983					 */
/* --------------------------------------------------------------------- */

#define CMPRSON 	15	/* Compress code			 */
#define CMPRSOFF	146	/* Uncompress code			 */
#define DSOFF		200	/* Double strike off code		 */
#define ESC		27	/* Escape code				 */
#define CR		13	/* Carriage return			 */
#define LF		10	/* Linefeed				 */
#define BS		08	/* Backspace				 */
#define TRUE		01

int cmd;

/* Beginning of SETPRTR main program	*/

main()
{
	clrscreen();
	putline("     IBM/EPSON MX-80 Printer Setup");
	putline("            by Michael Burton ");
	putline(" ");
	putline("A] Double width on  L] Double width off");
	putline("B] Compressed on    M] Compressed off");
	putline("C] Underlining on   N] Underlining off");
	putline("D] Emphasized on    P] Emphasized off");
	putline("E] Doublestrike on  Q] Doublestrike off");
	putline("F] Superscript on   R] Superscript off");
	putline("G] Subscript on     S] Subscript off");
	putline("H] Italics on       T] Italics off");
	putline("I] Perf skip set    U] Perf skip off");
	putline("J] Column width set V] Page length set");
	putline("K] Printer reset    W] Return to DOS");

	while (TRUE)
	{
		putcurs(16,0);
		puts("Selection?  ");
		putchar(BS);
		cmd = getchar() | 32;

		if (cmd == 'a')
		{
			prt3chr(ESC,'W',1);
			finmes("Double Width Print on");
		}
		if (cmd == 'b')
		{
			prtchr(CMPRSON);
			finmes("Compressed Print on");
		}
		if (cmd == 'c')
		{
			prt3chr(ESC,'-',1);
			finmes("Underline on");
		}
		if (cmd == 'd')
		{
			prt2chr(ESC,'E');
			finmes("Emphasized Print on");
		}
		if (cmd == 'e')
		{
			prt2chr(ESC,'G');
			finmes("Double Strike Print on");
		}
		if (cmd == 'f')
		{
			prt3chr(ESC,'S',0);
			finmes("Superscript on");
		}
		if (cmd == 'g')
		{
			prt3chr(ESC,'S',1);
			finmes("Subscript on");
		}
		if (cmd == 'h')
		{
			prt2chr(ESC,'4');
			finmes("Italics on");
		}
		if (cmd == 'i')
		{
	getinall("Perf skip how many lines? ",0,128,'N',"Perf skip set");
		}
		if (cmd == 'j')
		{
	getinall("No. of columns? ",0,232,'Q',"Column width set");
		}
		if (cmd == 'k')
		{
			prt2chr(ESC,'@');
			finmes("Printer Reset");
		}
		if (cmd == 'l')
		{
			prt3chr(ESC,'W',0);
			finmes("Double Width Print off");
		}
		if (cmd == 'm')
		{
			prtchr(CMPRSOFF);
			finmes("Compressed Print off");
		}
		if (cmd == 'n')
		{
			prt3chr(ESC,'-',0);
			finmes("Underline off");
		}
		if (cmd == 'p')
		{
			prt2chr(ESC,'F');
			finmes("Emphasized Print off");
		}
		if (cmd == 'q')
		{
			prt2chr(ESC,DSOFF);
			finmes("Double Strike Print off");
		}
		if (cmd == 'r')
		{
			prt2chr(ESC,DSOFF);
			finmes("Superscript off");
		}
		if (cmd == 's')
		{
			prt2chr(ESC,DSOFF);
			finmes("Subscript off");
		}
		if (cmd == 't')
		{
			prt2chr(ESC,'5');
			finmes("Italics off");
		}
		if (cmd == 'u')
		{
			prt2chr(ESC,'O');
			finmes("Perf skip off");
		}
		if (cmd == 'v')
		{
	getinall("How many lines? ",0,128,'C',"Page length set");
		}
		if (cmd == 'w') exit();
		if (cmd == 'x') prtline();
		if (cmd < 'a' | cmd > 'x')
			bell();
		else
			delay(6000);
	}
}

/*  End of SETPRTR main program 	*/
/*  Beginning of support functions	*/

delay(n)	/* Wait a while */
	int n;
{
	int i;

	i = 0;
	while (i++ < n) {
	}
	while (i-- > 0) {
	}
	clearline(20);
}

prtline()	/* Diagnostic. Print a line on the printer. */
{
	char buff[41];
	int i;

	i = 0;
	clearline(16);
	putcurs(16,0);
	gets(buff);
	while (buff[i] >0)
	{
		pcdos(5,buff[i++]);
	}
	pcdos(5,CR);
	pcdos(5,LF);
	clearline(16);
}

/*  Printer functions		*/

getinall(s,l,u,c,t)
	int *s,l,u,c,*t;
{
	int buff[41], n;

	while (TRUE)
	{
		clearline(16);
		putcurs(16,0);
		puts(s);
		gets(buff);
		n = atoi(buff);
		clearline(16);
		if (n <= l | n >= u)
			bell();
		else
			break;
	}
	prt3chr(ESC,c,n);
	finmes(t);
}
prt3chr(s,t,u)	/* Output 3 chars to the printer */
	char s,t,u;
{
	prt2chr(s,t);
	prtchr(u);
}

prt2chr(s,t)	/* Output 2 chars to the printer */
	char s,t;
{
	prtchr(s);
	prtchr(t);
}

prtchr(s)	/* Output a char to the printer */
	char s;
{
	finmes("Ready Printer");
	pcdos(5,s);
	clearline(20);
}

/* Display functions		*/

putline(s)	/* Send a line to display with cr/lf */
	char *s;
{
	puts(s);
	putchar(CR);
}

finmes(s)	/* Send a finish message to line 20 */
	char *s;
{
	putcurs(20,0);
	puts(s);
}

clearline(n)	/* Clear 40 columns of line n */
	int n;
{
	putcurs(n,0);
	puts("                                       ");
}

putcurs(r,c)	/* Put cursor at a specified row and column */
	int r,c;
{
	int a,b,d;

	a = 2;
	b = 0;
	d = 0;
	int10(a,d,b,d,d,d,r,c);
}

atoi(s) 	/* Convert a string to integer */
	char *s;
{
	int n;
	n = 0;
	while (*s >= '0' && *s <= '9')
	{
		n = 10 * n + *s - '0';
		s++;
	}
	return(n);
}
