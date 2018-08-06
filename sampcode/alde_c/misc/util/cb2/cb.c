#define VERS "CB  08/25/83  Vers 1.2 for IBM-PC\n\n"
/*
	Pretty-printer for C Programs
 
	Swiped from the CIPG's UNIX system and modified to run
		under BDS C by William C. Colley, III
 
	Mods made July 1980
 
	*** REVISIONS ***

	Modified to run on the IBM-PC with C86 08/16/83  J.E.Will
 
	Put indent level option in  08/23/83   J.E.Will

	Change interface for single file name, old file is
	renamed .BAK after program terminates. 08/25/83  J.E.Will

	*** END ***

*/

#include <stdio.h>

int s_level[10];
int c_level;
int sp_flg[20][10];
int s_ind[20][10];
int s_if_lev[10];
int s_if__flg[10];
int if_lev;
int if_flg;
int level;
int indent;		/* number of spaces to indent */
static int ind[10] = {
	0,0,0,0,0,0,0,0,0,0};
int e_flg;
int paren;
static int p_flg[10] = {
	0,0,0,0,0,0,0,0,0,0};
char l_char;
char p_char;
int a_flg;
int ct;
int s_tabs[20][10];
int q_flg;
char *w_if_[2];
char *w_else[2];
char *w_for[2];
char *w_ds[3];
int j;
char string[256];
char cc;
int s_flg;
int b_flg;
int peek;
int tabs;
char last_char;
char c;
char *w_kptr;

FILE *inpfil,*outfil,*fopen();

int comment(), putcoms(), getnl(), gotelse(), get_string(), lookup(),
	indent_puts(), getchr(), p_tabs();

char *index();

/* code starts here, Jack */

main(argc,argv)
int argc;
char *argv[];
{
	int k;
	printf(VERS);		/* tell the world about version */

	/*  Initialize everything here.  */

	if (argc < 2 || argc > 4)
	{
		printf("Usage:  CB input.fil [indent]\n\n");
		printf("input.fil  - 'C' input source file in current directory\n");
		printf("indent	 - where 'indent' is a number from 1 to 8 equal\n");
		printf("			 to number of spaces for each indent level\n");
		printf("			 Default 4 if not supplied on cmd line\n");
		printf("NOTES:\n");
		printf("	- Original source is in input.BAK\n\n");
		exit(99);
	}

	/* get name and check that it"s not a back-up file */

	strcpy(string,argv[1]);
	if((w_kptr = index(string,'.')) != 0)
	{
		if ((strcmp(w_kptr,".bak") == 0) || (strcmp(w_kptr,".BAK") == 0))
		{
			printf("Input file can not be a .BAK file!\n");
			exit(99);
		}
	}

	if ((inpfil = fopen(argv[1],"r")) == 0)
	{
		printf("File not found: %s\n",argv[1]);
		exit(99);
	}

	unlink("CB.$$$");	/* delete any existing temp file */

	if ((outfil = fopen("CB.$$$","wr")) == 0)
	{
		printf("Could not create CB.$$$, the temp output file\n");
		exit(99);
	}
	/* process indent option */

	indent = 4;		/* set default indent */

	if (argc == 3)
	{
		indent = atoi(argv[2]);
		if ((indent < 1) || (indent > 8))
		{
			printf("Indent option value out of range, valid from 1 to 8.");
			exit(99);
		}
	}
	c_level = iflev = level = e_flg = paren = 0;
	a_flg = q_flg = j = b_flg = tabs = 0;
	if_flg = peek = -1;
	s_flg = 1;
	w_if_[0] = "if";
	w_else[0] = "else";
	w_for[0] = "for";
	w_ds[0] = "case";
	w_ds[1] = "default";
	w_if_[1] = w_else[1] = w_for[1] = w_ds[2] = 0;

	/*  End of initialization.  */

	while((c = getchr()) != 032)
	{
	switch(c)
	{
	default:
		string[j++] = c;
		if(c != ',')l_char = c;
		break;
	case ' ':
	case '\t':
		if(lookup(w_else) == 1)
		{
		gotelse();
		if(s_flg == 0 || j > 0)string[j++] = c;
		indent_puts();
		s_flg = 0;
		break;
		}
		if(s_flg == 0 || j > 0)string[j++] = c;
		break;
	case '\n':
		if(e_flg = lookup(w_else) == 1)gotelse();
		indent_puts();
		fprintf(outfil,"\n");
		s_flg = 1;
		if(e_flg == 1)
		{
		p_flg[level]++;
		tabs++;
		}
		else
		if(p_char == l_char)
			a_flg = 1;
		break;
	case '{':
		if(lookup(w_else) == 1)gotelse();
		s_if_lev[c_level] = iflev;
		s_if_flg[c_level] = if_flg;
		if_lev = if_flg = 0;
		c_level++;
		if(s_flg == 1 && p_flg[level] != 0)
		{
		p_flg[level]--;
		tabs--;
		}
		string[j++] = c;
		indent_puts();
		getnl();
		indent_puts();
		fprintf(outfil,"\n");
		tabs++;
		s_flg = 1;
		if(p_flg[level] > 0)
		{
		ind[level] = 1;
		level++;
		s_level[level] = c_level;
		}
		break;
	case '}':
		c_level--;
		if((if_lev = s_if_lev[c_level]-1) < 0)if_lev = 0;
		if_flg = s_if_flg[c_level];
		indent_puts();
		tabs--;
		p_tabs();
		if((peek = getchr()) == ';')
		{
		fprintf(outfil,"%c;",c);
		peek = -1;
		}
		else fprintf(outfil,"%c",c);
		getnl();
		indent_puts();
		fprintf(outfil,"\n");
		s_flg = 1;
		if(c_level < s_level[level])if(level > 0)level--;
		if(ind[level] != 0)
		{
		tabs -= p_flg[level];
		p_flg[level] = 0;
		ind[level] = 0;
		}
		break;
	case '"':
	case '\'':
		string[j++] = c;
		while((cc = getchr()) != c)
		{
		string[j++] = cc;
		if(cc == '\\')
		{
			string[j++] = getchr();
		}
		if(cc == '\n')
		{
			indent_puts();
			s_flg = 1;
		}
		}
		string[j++] = cc;
		if(getnl() == 1)
		{
		l_char = cc;
		peek = '\n';
		}
		break;
	case ';':
		string[j++] = c;
		indent_puts();
		if(p_flg[level] > 0 && ind[level] == 0)
		{
		tabs -= p_flg[level];
		p_flg[level] = 0;
		}
		getnl();
		indent_puts();
		fprintf(outfil,"\n");
		s_flg = 1;
		if(if_lev > 0)
		if(if_flg == 1)
		{
			if_lev--;
			if_flg = 0;
		}
		else if_lev = 00;
		break;
	case '\\':
		string[j++] = c;
		string[j++] = getchr();
		break;
	case '?':
		q_flg = 1;
		string[j++] = c;
		break;
	case ':':
		string[j++] = c;
		if(q_flg == 1)
		{
		q_flg = 0;
		break;
		}
		if(lookup(w_ds) == 0)
		{
		s_flg = 0;
		indent_puts();
		}
		else
		{
		tabs--;
		indent_puts();
		tabs++;
		}
		if((peek = getchr()) == ';')
		{
		fprintf(outfil,";");
		peek = -1;
		}
		getnl();
		indent_puts();
		fprintf(outfil,"\n");
		s_flg = 1;
		break;
	case '/':
		string[j++] = c;
		if((peek = getchr()) != '*')break;
		string[j++] = peek;
		peek = -1;
		comment();
		break;
	case ')':
		paren--;
		string[j++] = c;
		indent_puts();
		if(getnl() == 1)
		{
		peek = '\n';
		if(paren != 0)a_flg = 1;
		else if(tabs > 0)
		{
			p_flg[level]++;
			tabs++;
			ind[level] = 0;
		}
		}
		break;
	case '#':
		string[j++] = c;
		while((cc = getchr()) != '\n')string[j++] = cc;
		string[j++] = cc;
		s_flg = 0;
		indent_puts();
		s_flg = 1;
		break;
	case '(':
		string[j++] = c;
		paren++;
		if(lookup(w_for) == 1)
		{
		while((c = get_string()) != ';');
		ct=0;
cont:
		while((c = get_string()) != ')')
		{
			if(c == '(') ct++;
		}
		if(ct != 0)
		{
			ct--;
			goto cont;
		}
		paren--;
		indent_puts();
		if(getnl() == 1)
		{
			peek = '\n';
			p_flg[level]++;
			tabs++;
			ind[level] = 0;
		}
		break;
		}
		if(lookup(w_if) == 1)
		{
		indent_puts();
		s_tabs[c_level][if_lev] = tabs;
		sp_flg[c_level][if_lev] = p_flg[level];
		s_ind[c_level][if_lev] = ind[level];
		if_lev++;
		if_flg = 1;
		}
	}
	}

	/* eof processing */

	fprintf(outfil,"\032");	/* throw in an EOF mark */
	fclose(outfil);
	fclose(inpfil);

	/* get origional name with a .BAK on it */

	strcpy(string,argv[1]);
	if((w_kptr = index(string,'.')) == 0)
	strcat(string,".BAK");
	else
	strcpy(w_kptr,".BAK");
	unlink(string);		/* kill any .BAK file */
	rename(argv[1],string);	/* origional is now .BAK */
	rename("CB.$$$",argv[1]);	/* new now has origional name */
	exit(0);			/* normal exit */
}
/* expand indent into tabs and spaces */
p_tabs()
{
	int i,j,k;

	i = tabs * indent;		/* calc number of spaces */
	j = i/8;			/* calc number of tab chars */
	i -= j*8;			/* calc remaining spaces */

	for (k=0;k < j;k++) fprintf(outfil,"\t");
	for (k=0;k < i;k++) fprintf(outfil," ");
}
/* get character from stream or return the saved one */
getchr()
{
	if(peek < 0 && last_char != ' ' && last_char != '\t')p_char = last_char;
	last_char = (peek<0) ? getc(inpfil):peek;
	if (last_char == -1) last_char = 0x1a;
	peek = -1;
	return(last_char == '\r' ? getchr():last_char);
}
/* put string with indent logic */
indent_puts()
{
	if(j > 0)
	{
	if(s_flg != 0)
	{
		if((tabs > 0) && (string[0] != '{') && (a_flg == 1)) tabs++;
		p_tabs();
		s_flg = 0;
		if((tabs > 0) && (string[0] != '{') && (a_flg == 1)) tabs--;
		a_flg = 0;
	}
	string[j] = '\0';
	fprintf(outfil,"%s",string);
	j = 0;
	}
	else
	{
	if(s_flg != 0)
	{
		s_flg = 0;
		a_flg = 0;
	}
	}
}
/* search table for character string and return index number */
lookup(tab)
char *tab[];
{
	char r;
	int l,kk,k,i;
	if(j < 1)return(0);
	kk=0;
	while(string[kk] == ' ')kk++;
	for(i=0; tab[i] != 0; i++)
	{
	l=0;
	for(k=kk;(r = tab[i][l++]) == string[k] && r != '\0';k++);
	if(r == '\0' && (string[k] < 'a' || string[k] > 'z'))return(1);
	}
	return(0);
}
/* read string between double quotes */
get_string()
{
	char ch;
beg:
	if((ch = string[j++] = getchr()) == '\\')
	{
	string[j++] = getchr();
	goto beg;
	}
	if(ch == '\'' || ch == '"')
	{
	while((cc = string[j++] = getchr()) != ch)
      if(cc == '\\')
         string[j++] = getchr();
	goto beg;
	}
	if(ch == '\n')
	{
	indent_puts();
	a_flg = 1;
	goto beg;
	}
	else return(ch);
}
/* else processing */
gotelse()
{
	tabs = s_tabs[c_level][if_lev];
	p_flg[level] = sp_flg[c_level][if_lev];
	ind[level] = s_ind[c_level][if_lev];
	if_flg = 1;
}
/* read to new_line */
getnl()
{
	while((peek = getchr()) == '\t' || peek == ' ')
	{
	string[j++] = peek;
	peek = -1;
	}
	if((peek = getchr()) == '/')
	{
	peek = -1;
	if((peek = getchr()) == '*')
	{
		string[j++] = '/';
		string[j++] = '*';
		peek = -1;
		comment();
	}
	else string[j++] = '/';
	}
	if((peek = getchr()) == '\n')
	{
	peek = -1;
	return(1);
	}
	return(0);
}
/* special edition of put string for comment processing */
putcoms()
{
	if(j > 0)
	{
	if(s_flg != 0)
	{
		p_tabs();
		s_flg = 0;
	}
	string[j] = '\0';
	fprintf(outfil,"%s",string);
	j = 0;
	}
}
/* comment processing */
comment()
{
rep:
	while((c = string[j++] = getchr()) != '*')
	if(c == '\n') putcoms();

gotstar:
	if((c = string[j++] = getchr()) != '/')
	{
	if(c == '*') goto gotstar;
	if(c == '\n') putcoms();
	goto rep;
	}
}


char *index(s, c)
char *s;
char c;
{
	while(*s != c) s++;
	return s;
}
