/*
	HEADER:		CUG236;
	TITLE:		C Source Formatter;
	DATE:		04/04/1987;
	DESCRIPTION:	"Formats a C source program with proper indents for
			each statement.";
	VERSION:	2.1;
	KEYWORDS:	Pretty Printer;
	FILENAME:	CB.C;
	SEE-ALSO:	CB.DOC;
	COMPILERS:	vanilla;
	AUTHORS:	W. C. Colley III, J. W. Kindschi Jr.;
*/

/*
	Modified for Portable C
	by: William C. Colley, III (4 APR 1987)

	Modified for Lattice C Ver 1.01
	by: John W. Kindschi Jr. (10-30-83)

	Swiped from CPIG'S UNIX system and modified to
	run under BDS C by William C. Colley, III


To use the program type the following command line:

	A>cb input.fil [output.fil]

	Where input.fil is the file to be pretty printed and [output.fil]
	is the destination file. If no output file is specified, then
	the output goes to standard output.
*/

#include <stdio.h>

/*
 * Portability Note:  The AZTEC C compilers handle the binary/text file
 * dichotomy differently from most other compilers.  Uncomment the following
 * pair of #defines if you are running AZTEC C:
 */

/*
#define getc(f)		agetc(f)
#define putc(c,f)	aputc(c,f)
*/

char cc, lchar, pchar, string[200];
char *wif[] =	{   "if",   NULL		};
char *welse[] = {   "else", NULL		};
char *wfor[] =	{   "for",  NULL		};
char *wds[] =	{   "case", "default",	NULL	};

int clevel, ct, iflev, ind[10], level, paren, sifflg[10], siflev[10];
int sind[20][10], slevel[10], spflg[20][10], stabs[20][10];
int aflg, bflg, eflg, ifflg = -1, pflg[10], qflg, sflg = 1;
int c, j, lastchar, peek = -1, tabs;
FILE *f1, *f2 = stdout;

void comment(), gotelse(), ptabs(), put_str();

int main(argc,argv)
int argc;
char *argv[];
{
    int getchr(), get_nl(), get_str(), lookup();

    /* Initialize everything here */

    if (argc < 2 || argc > 3) {
	fprintf(stderr,"Usage:  CB input.fil { output.fil }\n");  return !0;
    }
    if (!(f1 = fopen(*++argv,"r"))) {
	fprintf(stderr,"ERROR:  Cannot find file %s\n",*argv);  return !0;
    }
    if (argc == 3 && !(f2 = fopen(*++argv,"w"))) {
	fprintf(stderr,"ERROR:  Cannot create file %s\n",*argv);  return !0;
    }

    /* End of Initialization */

    while ((c = getchr()) != EOF) {
	switch (c) {
	    default:	string[j++] = c;
			if (c != ',') lchar = c;
			break;

	    case ' ':
	    case '\t':	if (lookup(welse) == 1) {
			    gotelse();
			    if (sflg == 0 || j > 0) string[j++] = c;
			    put_str();	sflg = 0;  break;
			}
			if (sflg == 0 || j > 0) string[j++] = c;
			break;

	    case '\n':	if (eflg = lookup(welse) == 1) gotelse();
			put_str();  fprintf(f2,"\n");  sflg = 1;
			if (eflg == 1) {  pflg[level]++;  tabs++; }
			else if(pchar == lchar) aflg = 1;
			break;

	    case '{':	if (lookup(welse) == 1) gotelse();
			siflev[clevel] = iflev;	 sifflg[clevel] = ifflg;
			iflev = ifflg = 0;  clevel++;
			if (sflg == 1 && pflg[level] != 0) {
			    pflg[level]--;  tabs--;
			}
			string[j++] = c;  put_str();  get_nl();	 put_str();
			fprintf(f2,"\n");  tabs++;  sflg = 1;
			if (pflg[level] > 0) {
			    ind[level] = 1;  level++;  slevel[level] = clevel;
			}
			break;

	    case '}':	clevel--;
			if ((iflev = siflev[clevel]-1) < 0) iflev = 0;
			ifflg = sifflg[clevel];	 put_str();  tabs--;  ptabs();
			if ((peek = getchr()) == ';') {
			    fprintf(f2,"%c;",c);  peek = -1;
			}
			else fprintf(f2,"%c",c);
			get_nl();  put_str();  fprintf(f2,"\n");  sflg = 1;
			if (clevel < slevel[level] && level > 0) level--;
			if (ind[level] != 0) {
			    tabs -= pflg[level];  pflg[level] = ind[level] = 0;
			}
			break;

	    case '"':
	    case '\'':	string[j++] = c;
			while ((cc = getchr()) != c) {
			    string[j++] = cc;
			    if (cc == '\\') string[j++] = getchr();
			    if (cc == '\n') { put_str();  sflg = 1; }
			}
			string[j++] = cc;
			if (get_nl() == 1) { lchar = cc;  peek = '\n'; }
			break;

	    case ';':	string[j++] = c;  put_str();
			if (pflg[level] > 0 && ind[level] == 0) {
			    tabs -= pflg[level];  pflg[level] = 0;
			}
			get_nl();  put_str();  fprintf(f2,"\n");  sflg = 1;
			if(iflev > 0 && ifflg == 1){ iflev--;  ifflg = 0; }
			else iflev = 0;
			break;

	    case '\\':	string[j++] = c;  string[j++] = getchr();  break;

	    case '?':	qflg = 1;  string[j++] = c;  break;

	    case ':':	string[j++] = c;
			if (qflg == 1) { qflg = 0;  break; }
			if (!lookup(wds)) { sflg = 0;  put_str(); }
			else { tabs--;	put_str();  tabs++; }
			if ((peek = getchr()) == ';') {
			    fprintf(f2,";");  peek = -1;
			}
			get_nl();  put_str();  fprintf(f2,"\n");  sflg = 1;
			break;

	    case '/':	string[j++] = c;
			if ((peek = getchr()) != '*') break;
			string[j++] = peek;  peek = -1;	 comment();  break;

	    case ')':	paren--;  string[j++] = c;  put_str();
			if (get_nl() == 1) {
			    peek = '\n';
			    if (paren != 0) aflg = 1;
			    else if (tabs > 0) {
				pflg[level]++;	tabs++;	 ind[level] = 0;
			    }
			}
			break;

	    case '#':	string[j++] = c;
			while ((cc = getchr()) != '\n') string[j++] = cc;
			string[j++] = cc;  sflg = 0;  put_str();  sflg = 1;
			break;

	    case '(':	string[j++] = c;  paren++;
			if (lookup(wfor) == 1) {
			    while ((c = get_str()) != ';');
			    ct = 0;
cont:			    while ((c = get_str()) != ')')
				if(c == '(') ct++;
			    if (ct != 0) { ct--;  goto cont; }
			    paren--;  put_str();
			    if (get_nl() == 1) {
				peek = '\n';  pflg[level]++;
				tabs++;	 ind[level] = 0;
			    }
			    break;
			}
			if (lookup(wif) == 1) {
			    put_str();
			    stabs[clevel][iflev] = tabs;
			    spflg[clevel][iflev] = pflg[level];
			    sind[clevel][iflev] = ind[level];
			    iflev++;  ifflg = 1;
			}
	}
    }
    if (f2 != stdout && (ferror(f2) || fclose(f2))) {
	fprintf(stderr,"ERROR:  Disk full\n");	return !0;
    }
    fclose(f1);	 return 0;
}

void ptabs()
{
    int i;

    for (i=0; i < tabs; i++) fprintf(f2,"\t");
}

int getchr()
{
    if (peek < 0 && lastchar != ' ' && lastchar != '\t') pchar = lastchar;
    lastchar = (peek < 0) ? getc(f1) : peek;  peek = -1;
    return lastchar;
}

void put_str()
{
    if (j > 0) {
	if (sflg != 0) {
	    ptabs();  sflg = 0;
	    if (aflg == 1) {
		aflg = 0;
		if (tabs > 0) fprintf(f2,"    ");
	    }
	}
	string[j] = '\0';  fprintf(f2,"%s",string);  j = 0;
    }
    else if (sflg != 0) { sflg = 0;  aflg = 0; }
}

int lookup(tab)
char *tab[];
{
    char r;
    int i,kk,k,l;

    if (j < 1) return 0;
    for (kk = 0; string[kk] == ' '; ++kk);
    for (i = 0; tab[i] != 0; i++) {
	l = 0;
	for (k=kk; (r = tab[i][l++]) == string[k] && r != '\0'; ++k);
	if (r == '\0' && (string[k] < 'a' || string[k] > 'z')) return 1;
    }
    return 0;
}

int get_str()
{
    char ch;

beg:
    if ((ch = string[j++] = getchr()) == '\\') {
	string[j++] = getchr();	 goto beg;
    }
    if (ch == '\'' || ch == '"') {
	while ((cc = string[j++] = getchr()) != ch)
	    if (cc == '\\') string[j++] = getchr();
	goto beg;
    }
    if (ch == '\n') { put_str();  aflg = 1;  goto beg; }
    else return ch;
}

void gotelse()
{
    tabs = stabs[clevel][iflev];  pflg[level] = spflg[clevel][iflev];
    ind[level] = sind[clevel][iflev];  ifflg = 1;
}

int get_nl()
{
    while ((peek = getchr()) == '\t' || peek == ' ') {
	string[j++] = peek;  peek = -1;
    }
    if ((peek = getchr()) == '/') {
	peek = -1;
	if ((peek = getchr()) == '*') {
	    string[j++] = '/';	string[j++] = '*';
	    peek = -1;	comment();
	}
	else string[j++] = '/';
    }
    if ((peek = getchr()) == '\n') { peek = -1;	 return 1; }
    return 0;
}

void comment()
{
rep:
    while ((c = string[j++] = getchr()) != '*')
	if (c == '\n') { put_str();  sflg = 1; }
gotstar:
    if ((c = string[j++] = getchr()) != '/') {
	if (c == '*') goto gotstar;
	goto rep;
    }
}
