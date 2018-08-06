/**********************************************************

      XC  -  A 'C' Concordance Utility

      Version 1.0   January, 1982

      Copyright (c) 1982 by Philip N. Hisley

	      Philip N. Hisley
	      548H Jamestown Court
	      Edgewood, Maryland 21040
	      (301) 679-4606

      Released for non-commercial distribution only




      Converted to IBM/PC CI/C86 by David N. Smith, May/June 1983
      with enhancements and Lattice compiler support in December 1983.

	      David N. Smith
	      44 Ole Musket Lane
	      Danbury, CT 06810
	      (203) 748-5934

      Changes Copyright (c) 1983 by David N. Smith

      PC Enhancements include:

	   1)  Nested #INCLUDE statements
	   2)  Single spaced cross-reference list
	   3)  Removal of tabbing on output device
	       (Since many printers don't support it)
	   4)  #INCLUDE statements with both "--" and <-->
	       syntax and with a full fileid in the quotes.
	   5)  Multiple input filenames on command line.



      Abstract:

      'XC' is a cross-reference utility for 'C' programs.
      Its has the ability to handle nested include files
      to a depth of 8 levels and properly processes nested
      comments as supported by BDS C. Option flags support
      the following features:

      - Routing of list output to disk
      - Cross-referencing of reserved words
      - Processing of nested include files
      - Generation of listing only

      Usage: xc <filename> <flag(s)>

      Flags: -i 	   = Enable file inclusion
	     -l 	   = Generate listing only
	     -r 	   = Cross-ref reserved words
	     -o <filename> = Write output to named file


***********************************************************/

#include "e:stdio.h"

/* Compiler specific stuff */
#define Lattice

#ifdef Lattice
#include "e:ctype.h"
#endif
/* end compiler specific section */

#ifndef  TRUE
#define  TRUE	     1
#define  FALSE	     0
#endif

#define  ERROR	    -1
#define  MAX_REF     5		/* maximum refs per ref-block */
#define  MAX_LEN    20		/* maximum identifier length  */
#define  MAX_WRD   749		/* maximum number of identifiers */
#define  MAX_ALPHA  53		/* maximum alpha chain heads */
#define  REFS_PER_LINE	10	/* maximum refs per line */
#define  LINES_PER_PAGE 60
#define  MAXCOL     78		/* default maximum column number for listing line */
#define  MINCOL     30		/* minimum value for -w option */
#define  FF 0x0C		/* formfeed */

struct	rf_blk {
		 int  ref_item[MAX_REF];
		 int  ref_cnt;
	       } onerf;

struct	id_blk {
		 char  id_name[MAX_LEN];
		 struct id_blk *alpha_lnk;
		 struct rf_blk *top_lnk;
		 struct rf_blk *lst_lnk;
	       } oneid;

struct id_blk *id_vector[MAX_WRD];

struct alpha_hdr { struct id_blk *alpha_top;
		   struct id_blk *alpha_lst;
		 };

struct alpha_hdr alpha_vector[MAX_ALPHA];

int	linum;		/* line number */
int	edtnum; 	/* edit line number */
int	fil_cnt;	/* active file index */
int	wrd_cnt;	/* token count */
int	pagno;		/* page number */
int	id_cnt; 	/* number of unique identifiers */
int	rhsh_cnt;	/* number of conflict hits */
int	filevl; 	/* file level  */
int	paglin; 	/* page line counter */
int	dummy;		/* dummy integer */
int	maxcol=MAXCOL;	/* maximum right column for listing line */
int	prt_ref;
char	act_fil[MAX_LEN];
char	lst_fil[MAX_LEN];
char	gbl_fil[MAX_LEN];
FILE	*f_lst_fil;
int	i_flg,
	o_flg,
	r_flg,
	l_flg;

long atoi();


/*************************************************************************/

main(p_argc, p_argv)
int	p_argc;
char	**p_argv;

{
    char  *arg;
    int argc;
    char **argv;
    char c;
    int i;

    argc = p_argc;
    argv = p_argv;
    if (argc < 2) use_err();
    i_flg=r_flg=o_flg=l_flg=FALSE;
    while(--argc != 0)
     { if(*(arg=*++argv) == '-')
	{switch(*++arg)
	 {
	   case 'i':
	   case 'I': i_flg++;
		     break;
	   case 'r':
	   case 'R': r_flg++;
		     break;
	   case 'l':
	   case 'L':  l_flg++;
		      break;
	   case 'o':
	   case 'O': { o_flg++;
		       if(--argc == 0) use_err();
		       strcpy(lst_fil,*++argv);
		       if(lst_fil[0] == '-') use_err();
			break;}
	   case 'w':
	   case 'W': { if(--argc == 0) use_err();
		       i = atoi(*++argv);
		       if( i<=MINCOL || i>=255 ) use_err();
		       maxcol = i;
		       break;
		       }
	   default: use_err();
	 }
       }
     }

     if(o_flg)
       {if( (f_lst_fil=fopen(lst_fil,"w")) == NULL)
	{ printf("ERROR: Unable to create list file - %s\n",lst_fil);
	  exit(0);}
       printf("XC ... 'C' Concordance Utility  v1.0\n\n");
	}

    prt_ref = FALSE;
    for(linum=0;linum < MAX_WRD;linum++) {
       id_vector[linum] = NULL; }
    for(linum=0;linum < MAX_ALPHA;linum++)
     {
       alpha_vector[linum].alpha_top =
       alpha_vector[linum].alpha_lst = NULL;
     }
    fil_cnt = wrd_cnt = linum = 0;
    filevl=paglin=pagno=edtnum=0;
    id_cnt=rhsh_cnt=0;

    argc = p_argc;  argc--;
    argv = p_argv;
    while(argc--) {
       strcpy(gbl_fil,*++argv);
       if(*gbl_fil == '-')  break;
       proc_file(gbl_fil,dummy);
       }
    if(!l_flg) {
      gbl_fil[0] = '\0';
      prnt_tbl();
      printf("\nAllowable Symbols: %d\n",MAX_WRD);
      printf("Unique    Symbols: %d\n",id_cnt);}
    if(o_flg) {
     nl();
     /* if(fprintf(f_lst_fil,"%c",CPMEOF) == ERROR) lst_err(); */
     fclose(f_lst_fil);
    }
}

/*************************************************************************/

lst_err()

{ printf("\nERROR: Write error on list output file - %s\n",
   lst_fil);
  exit(0);
}

/*************************************************************************/

use_err()

       { printf("\nERROR: Invalid parameter specification\n\n");
       printf("Usage: xc <filename>... <flag(s)>\n\n");
       printf("Flags: -i            = Enable file inclusion\n");
       printf("       -l            = Generate listing only\n");
       printf("       -r            = Cross-reference reserved words\n");
       printf("       -o <outfile>  = Write output to named file\n");
       printf("       -w width      = Width of output page; default=78\n");
       printf("Flags must follow all input file names");
       exit(0); }

/*************************************************************************/

proc_file(filnam,incnum)
char	*filnam;
int	 incnum;	/* prev. included line number (return to caller) */

{
  char	token[MAX_LEN]; /* token buffer */
  int	eof_flg;	/* end-of-file indicator */
  int	tok_len;	/* token length */
  FILE	*infile;	/* input file */


  strcpy(act_fil,filnam);
  edtnum=0;

  if((infile=fopen(filnam,"r")) == NULL)
      {printf("\nERROR: Unable to open input file: %s\n",filnam);
	return;}  /* ignore error */
  if(filevl++ == 0) prt_hdr();
  eof_flg = FALSE;
  do {
       if(get_token(infile,token,&tok_len,&eof_flg,0))
	  if(chk_token(token))
	     {
	     if(strcmp(token,"#include") == 0)
		{
		get_include_fileid(token,infile);
		if(!i_flg) continue;
		else
		   {
		   nl();
		   edtnum=proc_file(token,edtnum);
		   strcpy(act_fil,filnam);
		   continue;
		   }
		}
	    put_token(token,linum);
	    }
     } while (!eof_flg);

     filevl -= 1;
     fclose(infile);

     return( incnum );
}

/*************************************************************************/

get_include_fileid(token,infile)
char *token;
FILE *infile;
{

   char c, term;

   while ( (term=getc(infile)) == ' ' )  echo(term);
   echo(term);
   if ( term=='<' ) term='>';   /* terminator is > or " */
   if ( (term!='>') && (term!='"') )
      {
      printf("Error scanning #INCLUDE fileid: %c\n", term);
      exit(1);
      }

   do {
      if ( (c = getc(infile)) != ' ')
	 {
	 *token++ = c;
	 echo(c);
	 }
      else
	 echo(c);
      }
      while ( c!=term );

   *--token = '\0';

}

/*************************************************************************/

echo(c)
char c;
{
   static int col = 11;
   int i;
   echochar(c);
   if( c == '\n' )
      col = 11;
   else if( ++col > maxcol )  {
      col = 11;
      paglin++;
      echochar('\n');
      for( i=1; i<=11; i++ )  echochar(' ');
      }
}

echochar(c)
char c;
{
   if(o_flg)  {
      if(fprintf(f_lst_fil,"%c",c) == ERROR)  lst_err();
      }
   else
      printf("%c",c);
}
/*************************************************************************/

get_token(infile,g_token,g_toklen,g_eoflg,g_flg)

FILE	*infile;
char	*g_token;
int	*g_toklen;
int	*g_eoflg;
int	g_flg;

/*
	'getoken' returns the next valid identifier or
	reserved word from a given file along with the
	character length of the token and an end-of-file
	indicator
*/

{
int	c;
char	*h_token;
char	tmpchr;

h_token = g_token;

gtk:
*g_toklen = 0;
g_token = h_token;

/*
	Scan and discard any characters until an alphabetic or
	'_' (underscore) character is encountered or an end-of-file
	condition occurs
*/

while( (!isalpha(*g_token = rdchr(infile,g_eoflg,g_flg)))
  && !*g_eoflg
  && *g_token != '_'
  && *g_token != '0'
  && *g_token != '#');
if(*g_eoflg) return(FALSE);
*g_toklen += 1;

/*
	Scan and collect identified alpanumeric token until
	a non-alphanumeric character is encountered or and
	end-of-file condition occurs
*/

if(g_flg) tmpchr = '.';
     else tmpchr = '_';
while( (isalpha(c=rdchr(infile,g_eoflg,g_flg))
	|| isdigit(c)
	|| c == '_'
	|| c == tmpchr)
     && !*g_eoflg)
	{
	 if(*g_toklen < MAX_LEN)
	  { *++g_token = c;
	  *g_toklen += 1; }
	}

/*
	Check to see if a numeric hex or octal constant has
	been encountered ... if so dump it and try again
*/

	if (*h_token == '0') goto gtk;


/*
	Tack a NULL character onto the end of the token
*/

 *++g_token = NULL;

/*
	Screen out all #token strings except #include
*/

if (*h_token == '#' && strcmp(h_token,"#include")) goto gtk;

 return(TRUE);
}

/*************************************************************************/

 fil_chr(infile,f_eof)
   FILE *infile;
   int *f_eof;
   { int fc;
   fc=getc(infile);
/* if(fc == ERROR) {  /* omit since ERROR and EOF have save value for GETC  */
/*   printf("\nERROR: Error while processing input file - %s\n",            */
/*   act_fil);								    */
/*   exit(0);								    */
/*   }									    */
    if (fc == EOF) { *f_eof = TRUE;
				     fc = NULL; }
    return(fc);
}

/*************************************************************************/

rdchr(infile,r_eoflg,rd_flg)

int	*r_eoflg;
FILE	*infile;
int	rd_flg;

/*
	'rdchr' returns the next valid character in a file
	and an end-of-file indicator. A valid character is
	defined as any which does not appear in either a
	commented or a quoted string ... 'rdchr' will correctly
	handle comment tokens which appear within a quoted
	string
*/

{
int	c;
int	q_flg;		/* double quoted string flag */
int	q1_flg; 	/* single quoted string flag */
int	cs_flg; 	/* comment start flag */
int	ce_flg; 	/* comment end flag */
int	c_cnt;		/* comment nesting level */
int	t_flg;		/* transparency flag */

q_flg = FALSE;
q1_flg = FALSE;
cs_flg = FALSE;
ce_flg = FALSE;
t_flg = FALSE;
c_cnt  = 0;

rch:

/*
	Fetch character from file
*/

c=fil_chr(infile,r_eoflg);
if(*r_eoflg) return(c);   /* EOF encountered */
if(c == '\n')
		nl();
	     else
		echo(c);

if(rd_flg) return(c);

if(t_flg) { t_flg = !t_flg;
	    goto rch;}

if(c == '\\') { t_flg = TRUE;
		goto rch;}
/*
	If the character is not part of a quoted string
	check for and process commented strings...
	nested comments are handled correctly but unbalanced
	comments are not ... the assumption is made that
	the syntax of the program being xref'd is correct
*/

if (!q_flg && !q1_flg) {
     if (c == '*' && c_cnt && !cs_flg) { ce_flg = TRUE;
		      goto rch;}
     if (c == '/' && ce_flg) { c_cnt -= 1;
			  ce_flg = FALSE;
			  goto rch; }
     ce_flg = FALSE;
     if (c == '/') { cs_flg = TRUE;
		goto rch; }
     if (c == '*' && cs_flg) { c_cnt += 1;
			  cs_flg = FALSE;
			  goto rch; }
     cs_flg = FALSE;
     if (c_cnt) goto rch;
}

/*
	Check for and process quoted strings
*/

if ( c == '"' && !q1_flg) { q_flg =  !q_flg; /* toggle quote flag */
			goto rch;}
if (q_flg) goto rch;

if (c == '\'') { q1_flg = !q1_flg; /* toggle quote flag */
		goto rch; }
if (q1_flg) goto rch;

/*
	Valid character ... return to caller
*/

return(c);
}

/*************************************************************************/

chk_token(c_token)
char	*c_token;

{
  char	u_token[MAX_LEN];
  int	i;

    {
      if(r_flg) return(TRUE);
      i = 0;
      do { u_token[i] = toupper(c_token[i]); }
	 while (c_token[i++] != NULL);

      switch(u_token[0]) {
	case 'A': if (strcmp(u_token,"AUTO") == 0) return(FALSE);
		  break;
	case 'B': if (strcmp(u_token,"BREAK") == 0) return(FALSE);
		  break;
	case 'C': if (strcmp(u_token,"CHAR") == 0) return (FALSE);
		  if (strcmp(u_token,"CONTINUE") == 0) return (FALSE);
		  if (strcmp(u_token,"CASE") == 0) return (FALSE);
		  break;
	case 'D': if(strcmp(u_token,"DOUBLE") == 0) return(FALSE);
		  if(strcmp(u_token,"DO") == 0) return(FALSE);
		  if(strcmp(u_token,"DEFAULT") == 0) return(FALSE);
		  break;
	case 'E': if(strcmp(u_token,"EXTERN") == 0) return(FALSE);
		  if(strcmp(u_token,"ELSE") == 0) return(FALSE);
		  if(strcmp(u_token,"ENTRY") == 0) return(FALSE);
		  break;
	case 'F': if(strcmp(u_token,"FLOAT") == 0) return(FALSE);
		  if(strcmp(u_token,"FOR") == 0) return(FALSE);
		  break;
	case 'G': if(strcmp(u_token,"GOTO") == 0) return(FALSE);
		  break;
	case 'I': if(strcmp(u_token,"INT") == 0) return(FALSE);
		  if(strcmp(u_token,"IF") == 0) return(FALSE);
		  break;
	case 'L': if(strcmp(u_token,"LONG") == 0) return(FALSE);
		  break;
	case 'R': if(strcmp(u_token,"RETURN") == 0) return(FALSE);
		  if(strcmp(u_token,"REGISTER") == 0) return(FALSE);
		  break;
	case 'S': if(strcmp(u_token,"STRUCT") == 0) return(FALSE);
		  if(strcmp(u_token,"SHORT") == 0) return(FALSE);
		  if(strcmp(u_token,"STATIC") == 0) return(FALSE);
		  if(strcmp(u_token,"SIZEOF") == 0) return(FALSE);
		  if(strcmp(u_token,"SWITCH") == 0) return(FALSE);
		  break;
	case 'T': if(strcmp(u_token,"TYPEDEF") == 0) return(FALSE);
		  break;
	case 'U': if(strcmp(u_token,"UNION") == 0) return(FALSE);
		  if(strcmp(u_token,"UNSIGNED") == 0) return(FALSE);
		  break;
	case 'W': if(strcmp(u_token,"WHILE") == 0) return(FALSE);
		  break; }
	}
  return(TRUE);
}

/*************************************************************************/

/*
   Install parsed token and line reference in linked structure
*/

put_token(p_token,p_ref)

char *p_token;
int  p_ref;

{
  int  hsh_index;
  int  i;
  int  j;
  int  d;
  int  found;
  struct id_blk *idptr;
  struct rf_blk *rfptr;
  struct id_blk *alloc_id();
  struct rf_blk *alloc_rf();
  struct rf_blk *add_rf();

  if(l_flg) return;
  j=0;
  for (i=0; p_token[i] != NULL; i++)  /* Hashing algorithm is far from */
  {				      /* optimal but is adequate for a */
      j = j * 10 + p_token[i];	      /* memory-bound index vector!    */
   }
  hsh_index = abs(j) % MAX_WRD;
  found = FALSE;
  d = 1;
  do {
       idptr = id_vector[hsh_index];
       if(idptr == NULL) {
	 id_cnt++;
	 idptr = id_vector[hsh_index] = alloc_id(p_token);
	 chain_alpha(idptr,p_token);
	 idptr->top_lnk = idptr->lst_lnk = alloc_rf(p_ref);
	 found = TRUE;
       }
       else
       if(strcmp(p_token,idptr->id_name) == 0) {
	 idptr->lst_lnk = add_rf(idptr->lst_lnk,p_ref);
	   found = TRUE;
	 }
       else
       { hsh_index += d;
	 d += 2;
	 rhsh_cnt++;
	 if (hsh_index >= MAX_WRD)
	    hsh_index -= MAX_WRD;
	 if (d == MAX_WRD) {
	    printf("\nERROR: Symbol table overflow\n");
	    exit(0);
	 }
       }
      } while (!found);
}

/*************************************************************************/

chain_alpha(ca_ptr,ca_token)

struct id_blk *ca_ptr;
char  *ca_token;

{
  char	c;
  int	f;
  struct id_blk *cur_ptr;
  struct id_blk *lst_ptr;

  c = ca_token[0];
  if(c == '_') c = 0;
    else
      if (isupper(c))  c=1+((c-'A')*2);
      else	       c=2+((c-'a')*2);

  if(alpha_vector[c].alpha_top == NULL)
    { alpha_vector[c].alpha_top =
      alpha_vector[c].alpha_lst = ca_ptr;
      ca_ptr->alpha_lnk = NULL;
      return;
    }

/* check to see if new id_blk should be inserted between
   the alpha_vector header block and the first id_blk in
   the current alpha chain
*/

 if(strcmp(alpha_vector[c].alpha_top->id_name,ca_token) >0)
  {
    ca_ptr->alpha_lnk=alpha_vector[c].alpha_top;
    alpha_vector[c].alpha_top=ca_ptr;
    return;
  }

  if(strcmp(alpha_vector[c].alpha_lst->id_name,ca_token) < 0)
    { alpha_vector[c].alpha_lst->alpha_lnk = ca_ptr;
      ca_ptr->alpha_lnk = NULL;
      alpha_vector[c].alpha_lst=ca_ptr;
      return;
    }

  cur_ptr = alpha_vector[c].alpha_top;
  while(strcmp(cur_ptr->id_name,ca_token) < 0)
   { lst_ptr = cur_ptr;
     cur_ptr = lst_ptr->alpha_lnk;
   }

  lst_ptr->alpha_lnk = ca_ptr;
  ca_ptr->alpha_lnk = cur_ptr;
  return;
}

/*************************************************************************/

struct id_blk *alloc_id(aid_token)
  char	*aid_token;

  {
    int  ai;
    struct id_blk *aid_ptr;

     if((aid_ptr = alloc(sizeof(struct id_blk))) == 0) {
       printf("\nERROR: Unable to allocate identifier block\n");
       exit(0);
     }
     ai=0;
     do {
	  aid_ptr->id_name[ai] = aid_token[ai];
	} while (aid_token[ai++] != NULL);
     return (aid_ptr);
}

/*************************************************************************/

struct rf_blk *alloc_rf(arf_ref)

  int  arf_ref;

  {
    int ri;
    struct rf_blk *arf_ptr;

    if((arf_ptr = alloc(sizeof(struct rf_blk))) == 0) {
      printf("\nERROR: Unable to allocate reference block\n");
      exit(0);
    }
    arf_ptr->ref_item[0] = arf_ref;
    arf_ptr->ref_cnt = 1;
    for(ri=1;ri<MAX_REF;ri++)
      arf_ptr->ref_item[ri] = NULL;
    return (arf_ptr);
  }

/*************************************************************************/

struct rf_blk *add_rf(adr_ptr,adr_ref)

  struct rf_blk *adr_ptr;
  int adr_ref;

  {
    struct rf_blk *tmp_ptr;

    tmp_ptr = adr_ptr;
    if(adr_ptr->ref_cnt == MAX_REF) {
      tmp_ptr = adr_ptr->ref_cnt = alloc_rf(adr_ref);
    }
    else
    { adr_ptr->ref_item[adr_ptr->ref_cnt++] = adr_ref;
    }
    return (tmp_ptr);
  }

/*************************************************************************/

prnt_tbl()
{  int prf_cnt;
   int pti;
   int pref;
   int lin_cnt;
   struct id_blk *pid_ptr;
   struct rf_blk *ptb_ptr;

  prt_ref = TRUE;
  prt_hdr();
  for (pti=0;pti<MAX_ALPHA;pti++)
  { if ((pid_ptr = alpha_vector[pti].alpha_top) != NULL)
      { do
     { if(o_flg)
	 {if(fprintf(f_lst_fil,"%-20.19s: ",pid_ptr->id_name) == ERROR)
	    lst_err();}
       else
	  printf("%-20.19s: ",pid_ptr->id_name);
       ptb_ptr=pid_ptr->top_lnk;
       lin_cnt=prf_cnt=0;
       do { if(prf_cnt == MAX_REF)
	      { prf_cnt=0;
		ptb_ptr = ptb_ptr->ref_cnt;
	      }
	   if(ptb_ptr > MAX_REF)
	    {
	     if((pref=ptb_ptr->ref_item[prf_cnt++]) != 0)
		{ if(o_flg)
		 {if(fprintf(f_lst_fil,"%4d ",pref) == ERROR) lst_err();}
		else
		printf("%4d ",pref);
		if (++lin_cnt == REFS_PER_LINE)
		    { nl();
		      if(o_flg)
			 {if(fprintf(f_lst_fil,"                      ") == ERROR) lst_err();}
		      else
			 printf("                      ");
		      lin_cnt=0;
		    }
		}
	      } else pref=0;
	   } while (pref);
	nl();
	} while ((pid_ptr=pid_ptr->alpha_lnk) != NULL);
      }
   }/*for*/

   echo( '\n' );

}

/*************************************************************************/

prt_hdr()

{
  if (pagno++ != 0)    { echo( '\n' ); echo( FF ); }
  if (o_flg)
   {if(fprintf(f_lst_fil,
	  "XC ... 'C' Concordance Utility   %-20s       Page %d",
     gbl_fil,pagno) == ERROR) lst_err();}
  else
   printf("XC ... 'C' Concordance Utility   %-20s       Page %d",
     gbl_fil,pagno);
  echo('\n');
  paglin = 3;
  nl();
}

/*************************************************************************/

nl()

{
  echo('\n');
  if(++paglin >= LINES_PER_PAGE) prt_hdr();
    else
  if(!prt_ref) {
    if(o_flg) {
       if(fprintf(f_lst_fil,"%-4d %4d: ",    ++linum,++edtnum) == ERROR)
       lst_err();
       }
    else
       printf("%-4d %4d: ",    ++linum,++edtnum);
    if(o_flg)
       if(linum % 60 == 1)
	  printf("\n<%d> ",linum);
       else {
	  printf(".");
#if Lattice
	  fflush(stdout);
#endif
	  }

    }
 return;
}

/*************************************************************************/

#ifndef Lattice
abs(i)
int i;
{
   if (i<0) return(-i);
   else     return( i);
}
#endif

/*************************************************************************/

#ifdef Lattice
/*    cvt ascii to int or long	 */

long atoi(s)
char *s;
{
   long n;
   int sign = 0;

   while(*s==' ' || *s=='\t')  ++s;   /* move over blanks & tabs */
   if (*s=='-')
      sign=1;
   else
      if (*s!='+')  --s;
   ++s; 			      /* skip sign */
   for( n=0; *s>='0' && *s<='9'; )
      n = n * 10 + (*s++ - '0');
   if (sign)   n = -n;
   return n;
}
#endif

/*************************************************************************/

alloc(i)
int i;
{
   return malloc(i);
}
