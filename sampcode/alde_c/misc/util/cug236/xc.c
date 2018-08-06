/*
	HEADER:		CUG236;
	TITLE:		Cross Reference Generator;
	DATE:		04/05/1987;
	DESCRIPTION:	"Generic version of programmer's cross-reference
			generator originally for BDS.";
	VERSION:	2.2;
	KEYWORDS:	Cross Reference;
	FILENAME:	XC.C;
	SEE-ALSO:	CUG126, CUG171;
	COMPILERS:	vanilla;
	AUTHORS:	Phillip N. Hisley, W. H. Rauser, W. C. Colley III;
*/

/************************************************************************/
/*   5 APR 1987	  Portable C Conversion	    WCC				*/
/*     -  Epson init string changed from hex (\x00) to octal (\000).	*/
/*     -  Use () with A?(c=B):(c=C) for portability.  If it's an error, */
/*		it seems to be a very common one.			*/
/*     -  Made use of host compiler's strcmp() and strcpy() as they are */
/*		usually written in assembly and run faster.		*/
/*     -  Globally replaced "alpha_" with "alp_", "alloc_" with		*/
/*		"allo_", and "tmpchr" with "tmpch" to avoid multiply	*/
/*		defined symbol errors on compilers that only support	*/
/*		@$%&*&?>}\~~@# 6-character names.			*/
/*     -  Added abs() macro for compilers that don't have one.          */
/*     -  Used some data initialization to kill off some code.		*/
/*     -  Made miscellaneous small tweaks for portability.		*/
/*     -  Cleaned out old BDS C stuff to improve readability.		*/
/*     -  Cleaned up abortion with use of rf_blk.rf_cnt as a pointer	*/
/*		link to the next rf_blk.  This construct will NOT work	*/
/*		in environments like the MSDOS large model where ints	*/
/*		are smaller than pointers!				*/
/*									*/
/*   9-26-83   Microsoft C 1.04	 Conversion    WHR			*/
/*     -  \t between line numbers and text to fix indenting problem.	*/
/*     -  added option -e for output to Epson in condensed print.	*/
/*     -  toupper() and isupper() are macros, not functions.		*/
/*     -  eliminate side effect in toupper(*++arg) in main().		*/
/*     -  change alloc() to malloc().					*/
/*     -  add #define NAMES that are not in stdio.h			*/
/*     -  MS-C requires () in statement A?(c=B):(c=C)	error or not??	*/
/*									*/
/*   4-30-83   Computer Innovations C-86 1.31 Conversion    WHR		*/
/*     -  #include filename changed to allow a disk drive prefix, D:	*/
/*     -  convert if(fprintf(...) == ERROR) lst_err(); to fprintf(..);	*/
/*     -  convert if(fopen(...) == ERROR) statements to == NULL.	*/
/*     -  C86 requires () in statement A?(c=B):(c=C)	error or not??	*/
/*     -  remove getc() == ERROR check in fil_chr().			*/
/*     -  convert file conventions from BDS C to C-86.			*/
/*     -  comment out BDS unique statements, mark revised statements.	*/
/*	  keep all BDS statements to document conversion effort.	*/
/*									*/
/**  4-19-83   BDS C Version file XC.CQ copied from Laurel RCPM	   WHR	*/
/************************************************************************/
/*							   */
/*    XC  -  A 'C' Concordance Utility			   */
/*							   */
/*    Version 1.0   January, 1982			   */
/*							   */
/*    Copyright (c) 1982 by Philip N. Hisley		   */
/*							   */
/*    Released for non-commercial distribution only	   */
/*							   */
/*    Abstract:						   */
/*							   */
/*    'XC' is a cross-reference utility for 'C' programs.  */
/*    Its has the ability to handle nested include files   */
/*    to a depth of 8 levels and properly processes nested */
/*    comments as supported by BDS C. Option flags support */
/*    the following features:				   */
/*							   */
/*    - Routing of list output to disk			   */
/*    - Cross-referencing of reserved words		   */
/*    - Processing of nested include files		   */
/*    - Generation of listing only			   */
/*							   */
/*    Usage: xc <filename> <flag(s)>			   */
/*							   */
/*    Flags: -i		   = Enable file inclusion	   */
/*	     -l		   = Generate listing only	   */
/*	     -r		   = Cross-ref reserved words	   */
/*	     -o <filename> = Write output to named file	   */
/*							   */
/*    Please report bugs/fixes/enhancements to:		   */
/*							   */
/*	      Philip N. Hisley				   */
/*	      548H Jamestown Court			   */
/*	      Edgewood, Maryland 21040			   */
/*	      (301) 679-4606				   */
/*	      Net Addr: PNH@MIT-AI			   */
/*							   */
/***********************************************************/

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

/*  Portability Note:  8-bit systems often don't have header files
    ctype.h and string.h.  If your system doesn't have these,
    uncomment the #defines NO_STRING_H and NO_CTYPE_H.		   WCC	*/

/*
#define	 NO_STRING_H
#define	 NO_CTYPE_H
*/

/*  Portability Note:  Some older compilers call the function malloc()
    by its older name alloc().	If you have one of these older
    compilers, uncomment the following #define.			WCC	*/

/*
#define	 malloc(x)	alloc(x)
*/

/*  Portability Note:  A few compilers don't know the additional type
    void.  If yours is one of these, uncomment the following #define.	*/

/* #define	void		int					*/

#ifdef	 NO_CTYPE_H						/* WCC */
int isalpha(), isdigit(), isupper(), toupper();
#else
#include <ctype.h>						/* WHR */
#endif

#ifdef	 NO_STRING_H						/* WCC */
int strcmp();  char *strcpy();
#else
#include <string.h>
#endif

#ifndef	 abs							/* WCC */
#define	 abs(x)		((x < 0) ? (-x) : (x))
#endif

/*  Portability Note:  Some stdio.h files define various of these
    constants.	Some don't.  Therefore, each definition is
    compiled only if needed.					   WCC */

#ifndef	 NULL							/* WCC */
#define	 NULL	    0
#endif

#ifndef	 FALSE							/* WCC */
#define	 FALSE	    0						/* WHR */
#endif

#ifndef	 TRUE							/* WCC */
#define	 TRUE	    1						/* WHR */
#endif

#define	 MAX_REF    5		/* maximum refs per ref-block */
#define	 MAX_LEN    20		/* maximum identifier length  */
#define	 MAX_WRD   749		/* maximum number of identifiers */
#define	 MAX_ALPHA  53		/* maximum alpha chain heads */
#define	 REFS_PER_LINE	8	/* maximum refs per line */
#define	 LINES_PER_PAGE 60
#define	 FF 0x0C		 /* formfeed */

/*  Order of the next two structure declarations reversed to avoid
    a forward reference that chokes some compilers.			WCC */

struct rf_blk {
		 int ref_item[MAX_REF];
		 int ref_cnt;
		 struct rf_blk *ref_lnk;
	       } onerf;

struct id_blk {
		 char id_name[MAX_LEN];
		 struct id_blk *alp_lnk;
		 struct rf_blk *top_lnk;
		 struct rf_blk *lst_lnk;
	       } oneid, *id_vector[MAX_WRD];

struct alp_hdr { struct id_blk *alp_top;
		   struct id_blk *alp_lst;
		 } alp_vector[MAX_ALPHA];

int	linum;		/* line number */
int	edtnum;		/* edit line number */
int	fil_cnt;	/* active file index */
int	wrd_cnt;	/* token count */
int	pagno;		/* page number */
int	id_cnt;		/* number of unique identifiers */
int	rhsh_cnt;	/* number of conflict hits */
int	filevl;		/* file level  */
int	paglin;		/* page line counter */
int	prt_ref = FALSE;
char	act_fil[MAX_LEN];
char	lst_fil[MAX_LEN];
char	gbl_fil[MAX_LEN];
FILE   *l_buffer;					     /* WHR */
int	e_flg = FALSE, i_flg = FALSE, o_flg = FALSE;	     /* WHR */
int	r_flg = FALSE, l_flg = FALSE, debug = FALSE;	     /* WCC */
char	Epson[]	 = "\033@\017\033Q\204";		     /* WCC */

void exit();

/*-------------------------------------------*/

void main(argc,argv)
int	argc;
char	**argv;
{
     char  *arg, cc;
     void nl(), prnt_tbl(), proc_file(), use_err();

     if (argc < 2) use_err();
     (void)strcpy(gbl_fil,*++argv);
     --argc;
     if(gbl_fil[0] == '-')
	  use_err();
     while(--argc != 0)
     {	  if(*(arg=*++argv) == '-')
    /*****{    switch( toupper(*++arg) )     *** side effect in Microsoft C */
	  {    switch( cc=*++arg, toupper(cc) )		     /* Microsoft C */
	       {   case 'I':  i_flg++;
			      break;
		   case 'R':  r_flg++;
			      break;
		   case 'L':  l_flg++;
			      break;
		   case 'O': {o_flg++;
			      if(--argc == 0) use_err();
			      (void)strcpy(lst_fil,*++argv);
			      if(lst_fil[0] == '-') use_err();
			      if(debug) printf("lst_fil=>%s<",lst_fil);
			      break;
			     }
		   case 'D':  debug++;
			      break;
		   case 'E':  e_flg++;				  /* WHR */
			      o_flg++;
			      (void)strcpy(lst_fil,"LPT1:");
			      break;
		   default:   use_err();
	       }
	  }
     else use_err();
     }
     if (debug) printf("\ni_flg=%d, r_flg=%d, l_flg=%d", i_flg,r_flg,l_flg);
     if (debug) printf("\no_flg=%d, debug=%d", o_flg,debug);
     if (o_flg)
     {	  if ( (l_buffer = fopen(lst_fil,"w")) == NULL)	  /*** output file ***/
	  {    printf("ERROR Unable to create list file - %s\n",lst_fil);
	       exit(0);
	  }
	  printf("\nXC ....... 'C' Concordance Utility  v2.3\n");
	  printf("    Portable C,  Conversion 5 APR 1987\n\n");	    /* WCC */
	  if (e_flg) fprintf(l_buffer,"%s",Epson);
     }
     for(linum=0;linum < MAX_WRD;linum++) {
	  id_vector[linum] = NULL;
     }
     for(linum=0;linum < MAX_ALPHA;linum++)
     {
	  alp_vector[linum].alp_top =
	       alp_vector[linum].alp_lst = NULL;
     }
     fil_cnt = wrd_cnt = linum = 0;
     filevl=paglin=pagno=edtnum=0;
     id_cnt=rhsh_cnt=0;
     proc_file(gbl_fil);
     if(!l_flg) {
	 prnt_tbl();
	 printf("\nAllowable Symbols: %d\n",MAX_WRD);
	 printf("Unique    Symbols: %d\n",id_cnt);
     }
     if(o_flg) {
	  nl();
	  fprintf(l_buffer,"\nAllowable Symbols: %d\n",MAX_WRD);   /* WHR */
	  fprintf(l_buffer,"Unique    Symbols: %d\n",id_cnt);	   /* WHR */
	  fprintf(l_buffer,"%c",FF);				   /* WHR */
	  fflush(l_buffer);
	  fclose(l_buffer);
     }
}/*main.
----------------------------------------*/

void use_err()
{
     printf("\nERROR: Invalid parameter specification\n\n");
     printf("Usage: xc <filename> <flag(s)>\n\n");
     printf("Flags: -e             = Emit printer initialization string\n");
     printf("       -i             = Enable file inclusion\n");
     printf("       -l             = Generate listing only\n");
     printf("       -r             = Cross-reference reserved words\n");
     printf("       -o <filename>  = Write output to named file\n");
     exit(0);
}/*use_err.
-------------------------------------------*/


void proc_file(filnam)
     char    *filnam;

{
     FILE  *buffer;	   /* allocated buffer pointer */	    /* WHR */
     char  token[MAX_LEN]; /* token buffer */
     int   eof_flg;	   /* end-of-file indicator */
     int   tok_len;	   /* token length */
     int   incnum;	   /* included line number */
     int chk_token(), get_token();
     void nl(), prt_hdr(), put_token();

     (void)strcpy(act_fil,filnam);
     if ((buffer = fopen(filnam,"r")) == NULL)	/*** input file ***/ /* WHR */
     {	  printf("\nERROR: Unable to open input file: %s\n",filnam);
	  exit(0);
     }
     if(filevl++ == 0) prt_hdr();
     eof_flg = FALSE;

     do {
	  if(get_token(buffer,token,&tok_len,&eof_flg,0))
	  {    if (debug) printf("\ntoken: %s   length: %d\n",token,&tok_len);
	       if (chk_token(token))
	       {
		     /* #include processing changed to accept drive:   WHR */
		    if (strcmp(token,"#include") == 0)
		    {	 if (get_token(buffer,token,&tok_len,&eof_flg,1))
			 {    if (debug) printf("\ntoken: %s   length: %d\n",
						  token,&tok_len);
			      if (!i_flg) continue;
			      else
			      {	   incnum=edtnum;
				   edtnum=0;
				   nl();
				   proc_file(token);
				   edtnum=incnum;
				   (void)strcpy(act_fil,filnam);
				   continue;
			      }
			 }
		    }
		    put_token(token,linum);
	       }
	  }
     } while (!eof_flg);

     filevl -= 1;
     fclose(buffer);
 /***	 free(sav_buffer);  */
}/*proc_file.
-------------------------------------------*/

int get_token(g_buffer,g_token,g_toklen,g_eoflg,g_flg)
     FILE    *g_buffer;
     char    *g_token;
     int     *g_toklen;
     int     *g_eoflg;
     int     g_flg;

    /*
     *	 'getoken' returns the next valid identifier or
     *	 reserved word from a given file along with the
     *	 character length of the token and an end-of-file
     *	 indicator
     *
     */

{
     int     c, rdchr();
     char    *h_token;
     char    tmpch;
     char    tmpch2;		      /* WHR fix for B:filename.ext */

     h_token = g_token;

 gtk:			       /* top of loop, get new token */
     *g_toklen = 0;
     g_token = h_token;

    /*
     *	Scan and discard any characters until an alphabetic or
     *	'_' (underscore) character is encountered or an end-of-file
     *	condition occurs
     */

     while(  (!isalpha(*g_token = rdchr(g_buffer,g_eoflg,g_flg)))
	     &&	 !*g_eoflg	&&  *g_token != '_'
	     && *g_token != '0' &&  *g_token != '#' );
     if(*g_eoflg) return(FALSE);

     *g_toklen += 1;

    /*
     *	 Scan and collect identified alpanumeric token until
     *	 a non-alphanumeric character is encountered or and
     *	 end-of-file condition occurs
     */

     if(g_flg) {
	  tmpch	 = '.';
	  tmpch2 = ':';			  /* WHR fix for B:filename.ext */
     }
     else {
	  tmpch	 = '_';
	  tmpch2 = '_';			  /* WHR fix for B:filename.ext */
     }
     while( (isalpha(c=rdchr(g_buffer,g_eoflg,g_flg)) ||
	    isdigit(c) || c == '_' || c == tmpch || c == tmpch2)
	    && !*g_eoflg)		   /* WHR fix for B:filename.ext */
     {	  if(*g_toklen < MAX_LEN)
	  {    *++g_token = c;
	       *g_toklen += 1;
	  }
     }


    /*
     *	    Check to see if a numeric hex or octal constant has
     *	    been encountered ... if so dump it and try again
     */


     if (*h_token == '0') goto gtk;


    /*
     *	    Tack a NULL character onto the end of the token
     */

     *++g_token = NULL;

    /*
     *	    Screen out all #token strings except #include
     */

     if (*h_token == '#' && strcmp(h_token,"#include")) goto gtk;

     return (TRUE);
}/*get_token.
-----------------------------------------*/

int fil_chr(f_buffer,f_eof)
     FILE *f_buffer;
     int *f_eof;
{
     int fc;
     if ((fc = getc(f_buffer)) == EOF) {
	  *f_eof = TRUE;
	  fc = NULL;
     }
     return(fc);
}/*fil_chr.
----------------------------------------*/

int rdchr(r_buffer,r_eoflg,rd_flg)
     int     *r_eoflg, rd_flg;
     FILE    *r_buffer;

/*
	'rdchr' returns the next valid character in a file
	and an end-of-file indicator. A valid character is
	defined as any which does not appear in either a
	commented or a quoted string ... 'rdchr' will correctly
	handle comment tokens which appear within a quoted
	string
*/

{
     int     c;
     int     q_flg;	     /* double quoted string flag */
     int     q1_flg;	     /* single quoted string flag */
     int     cs_flg;	     /* comment start flag */
     int     ce_flg;	     /* comment end flag */
     int     c_cnt;	     /* comment nesting level */
     int     t_flg;	     /* transparency flag */
     void nl();

     q_flg = q1_flg = cs_flg = ce_flg = t_flg = FALSE;
     c_cnt  = 0;

rch:

    /*
     *	 Fetch character from file
     */

     c = fil_chr(r_buffer,r_eoflg);

     if (*r_eoflg) return(c);	/* EOF encountered */
     if (c == '\n')
	   nl();
     else
	  if (o_flg) fprintf(l_buffer,"%c",c);			/* WHR */
	  else printf("%c",c);

     if (rd_flg) return(c);

     if (t_flg) {
	  t_flg = !t_flg;
	  goto rch;
     }

     if (c == '\\') {
	  t_flg = TRUE;
	  goto rch;
     }
	/*
	If the character is not part of a quoted string
	check for and process commented strings...
	nested comments are handled correctly but unbalanced
	comments are not ... the assumption is made that
	the syntax of the program being xref'd is correct
	*/

     if (!q_flg	 &&  !q1_flg) {
	  if (c == '*'	&&  c_cnt  &&  !cs_flg) {
	       ce_flg = TRUE;
	       goto rch;
	  }
	  if (c == '/'	&&  ce_flg) {
	       c_cnt -= 1;
	       ce_flg = FALSE;
	       goto rch;
	  }
	  ce_flg = FALSE;
	  if (c == '/') {
	       cs_flg = TRUE;
	       goto rch;
	  }
	  if (c == '*'	&&  cs_flg) {
	       c_cnt += 1;
	       cs_flg = FALSE;
	       goto rch;
	  }
	  cs_flg = FALSE;

	  if (c_cnt) goto rch;
     }

	/*
	Check for and process quoted strings
	*/

     if ( c == '"'  &&	!q1_flg) {	/* toggle quote flag */
	  q_flg =  !q_flg;
	  if(debug) printf("\nq_flg toggled to: %d\n" ,q_flg);
	  goto rch;
     }
     if (q_flg) goto rch;

     if (c == '\'') {	     /* toggle quote flag */
	  q1_flg = !q1_flg;
	  if(debug) printf("\nq1_flg toggled to: %d\n" ,q1_flg);
	  goto rch;
     }
     if (q1_flg) goto rch;

	/*
	Valid character ... return to caller
	*/

     return (c);
}/*rdchr.
-----------------------------------------------*/


int chk_token(c_token)
     char    *c_token;
{
     char  u_token[MAX_LEN];
     int   i;

     {
      if (r_flg) return(TRUE);
      i = 0;
      do { u_token[i] = toupper(c_token[i]);
      } while (c_token[i++] != NULL);

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
  return (TRUE);
}/*chk_token.
---------------------------------------------*/


  /*
   *	Install parsed token and line reference in linked structure
   */

void put_token(p_token,p_ref)
char *p_token;
int  p_ref;
{
     int  d, found, hsh_index, i, j;
     struct id_blk *idptr, *allo_id();
     struct rf_blk *allo_rf(), *add_rf();
     void chain_alpha();

     if (l_flg) return;
     j=0;
     for (i=0; p_token[i] != NULL; i++)	 /* Hashing algorithm is far from */
     {					 /* optimal but is adequate for a */
	 j = j * 10 + p_token[i];	 /* memory-bound index vector!	  */
     }
     hsh_index = abs(j) % MAX_WRD;

     found = FALSE;
     d = 1;
     do
     {	  idptr = id_vector[hsh_index];
	  if (idptr == NULL)
	  {    id_cnt++;
	       idptr = id_vector[hsh_index] = allo_id(p_token);
	       chain_alpha(idptr,p_token);
	       idptr->top_lnk = idptr->lst_lnk = allo_rf(p_ref);
	       found = TRUE;
	  }
	  else
	  if (strcmp(p_token,idptr->id_name) == 0)
	  {    idptr->lst_lnk = add_rf(idptr->lst_lnk,p_ref);
	       found = TRUE;
	  }
	  else
	  {    hsh_index += d;
	       d += 2;
	       rhsh_cnt++;
	       if (hsh_index >= MAX_WRD)
		    hsh_index -= MAX_WRD;
	       if (d == MAX_WRD)
	       {    printf("\nERROR: Symbol table overflow\n");
		    exit(0);
	       }
	  }
     } while (!found);
}/*put_token.
--------------------------------------------*/


void chain_alpha(ca_ptr,ca_token)
struct id_blk *ca_ptr;
char  *ca_token;
{
     char  c;
     struct id_blk *cur_ptr;
     struct id_blk *lst_ptr;

     c = ca_token[0];
     if (c == '_')  c = 0;
     else
     /**  isupper(c) ? c=1+((c-'A')*2) : c=2+((c-'a')*2) ;     error or not??
      **  A good one for the puzzle book! Is the () required around (c=..)?
      **  C86 and Microsoft C both req the ()'s, BDS C did not.
      **  Is it required because = has lower precedence than ?: ????
      **/
	  isupper(c) ? (c=1+((c-'A')*2)) : (c=2+((c-'a')*2)) ;

     if(alp_vector[c].alp_top == NULL)
     {	  alp_vector[c].alp_top =
		  alp_vector[c].alp_lst = ca_ptr;
	  ca_ptr->alp_lnk = NULL;
	  return;
     }

    /*	check to see if new id_blk should be inserted between
     *	the alp_vector header block and the first id_blk in
     *	the current alpha chain
     */

     if(strcmp(alp_vector[c].alp_top->id_name,ca_token) >0)
     {	  ca_ptr->alp_lnk=alp_vector[c].alp_top;
	  alp_vector[c].alp_top=ca_ptr;
	  return;
     }

     if(strcmp(alp_vector[c].alp_lst->id_name,ca_token) < 0)
     {	  alp_vector[c].alp_lst->alp_lnk = ca_ptr;
	  ca_ptr->alp_lnk = NULL;
	  alp_vector[c].alp_lst=ca_ptr;
	  return;
     }

     cur_ptr = alp_vector[c].alp_top;
     while(strcmp(cur_ptr->id_name,ca_token) < 0)
     {	  lst_ptr = cur_ptr;
	  cur_ptr = lst_ptr->alp_lnk;
     }

     lst_ptr->alp_lnk = ca_ptr;
     ca_ptr->alp_lnk = cur_ptr;
     return;
}/*chain_alpha.
-----------------------------------------*/

struct id_blk *allo_id(aid_token)
char  *aid_token;
{
     int  ai;
     struct id_blk *aid_ptr;
     char *malloc();					     /* Microsoft C */

/*** if((aid_ptr =  alloc(sizeof(oneid))) == 0) { ***/	     /* Microsoft C */
     if((aid_ptr = (struct id_blk *) malloc(sizeof(oneid))) == 0) { /* MS C */

	  printf("\nERROR: Unable to allocate identifier block\n");
	  exit(0);
     }
     ai=0;
     do {
	  aid_ptr->id_name[ai] = aid_token[ai];
     } while (aid_token[ai++] != NULL);
     return (aid_ptr);
}/*id_blk.
-----------------------------------------*/

struct rf_blk *allo_rf(arf_ref)
int  arf_ref;
{
    int ri;
    struct rf_blk *arf_ptr;
    char *malloc();				       /* Microsoft C */

/** if((arf_ptr = alloc(sizeof(onerf))) == 0) { **/    /* Microsoft C */
    if((arf_ptr = (struct rf_blk *) malloc(sizeof(onerf))) == 0) { /* MS C */
	 printf("\nERROR: Unable to allocate reference block\n");
	 exit(0);
    }
    arf_ptr->ref_item[0] = arf_ref;
    arf_ptr->ref_cnt = 1;  arf_ptr->ref_lnk = NULL;
    for (ri=1;ri<MAX_REF;ri++)
	  arf_ptr->ref_item[ri] = NULL;
    return (arf_ptr);
}/*allo_rf.
------------------------------------------*/


struct rf_blk *add_rf(adr_ptr,adr_ref)
struct rf_blk *adr_ptr;
int adr_ref;
{
     if(adr_ptr->ref_cnt == MAX_REF)
	  return adr_ptr->ref_lnk = allo_rf(adr_ref);
     else {
	  adr_ptr->ref_item[adr_ptr->ref_cnt++] = adr_ref;
	  return (adr_ptr);
     }
}/*rf_blk.
------------------------------------------*/

void prnt_tbl()
{
     int lin_cnt, pref, prf_cnt, pti;
     struct id_blk *pid_ptr;
     struct rf_blk *ptb_ptr;
     void nl(), prt_hdr();

     prt_ref = TRUE;
     prt_hdr();
     for (pti=0;pti<MAX_ALPHA;pti++)
     {	  if ((pid_ptr = alp_vector[pti].alp_top) != NULL)
	  {    do
	       {    if(o_flg)					      /* WHR */
			 fprintf(l_buffer,"%-14.13s: ",pid_ptr->id_name);
		    else printf("%-14.13s: ",pid_ptr->id_name);
		    ptb_ptr=pid_ptr->top_lnk;
		    lin_cnt=prf_cnt=0;
		    do
		    {	 if(prf_cnt == MAX_REF)
			 {    prf_cnt=0;
			      ptb_ptr = ptb_ptr->ref_lnk;
			 }
			 if(ptb_ptr)
			 {
			      if((pref=ptb_ptr->ref_item[prf_cnt++]) != 0)
			      {
				   if (lin_cnt++ == REFS_PER_LINE)
				   {	nl();		     /* WHR,WCC */
					if (o_flg) fprintf(l_buffer,"\t\t");
					else printf("\t\t");
					lin_cnt = 1;
				   }
				   if(o_flg) fprintf(l_buffer,"%-4d  ",pref);
				   else printf("%-4d  ",pref);
			      }
			 }
			 else pref=0;
		    } while (pref);
		    nl();
	       } while ((pid_ptr=pid_ptr->alp_lnk) != NULL);
	  }
     }
}/*prnt_tbl.
---------------------------------------*/


void prt_hdr()
{
     void nl();

     if (pagno++ != 0)
     if(o_flg) fprintf(l_buffer,"%c",FF);			/* WHR */
     else printf("%c",FF);

     if(o_flg)
	  fprintf(l_buffer,
	       "XC ... 'C' Concordance Utility : %s\t\t\t\t Page %d",
	       gbl_fil,pagno);				       /* WHR */
     else printf("XC ... 'C' Concordance Utility : %s\t\t\t\t Page %d",
	  gbl_fil,pagno);
     if (o_flg) fprintf(l_buffer,"\n\n");		       /* WHR */
     else printf("\n\n");

     nl();  paglin =3;	return;
}/*prt_hdr.
--------------------------------------*/

void nl()
{
     if (o_flg) fprintf(l_buffer,"\n");			       /* WHR */
     else printf("\n");

     if (++paglin == LINES_PER_PAGE) prt_hdr();
     else if(!prt_ref)
	  {    if (o_flg) fprintf(l_buffer,"%-4d %4d:\t",
				++linum,++edtnum);	       /* WHR */
	       else printf("%-4d %4d:\t",++linum,++edtnum);

	       if(o_flg)
		    if(linum % 60 == 1) printf("\n<%d>\t",linum);
		    else printf(".");
	  }
     return;
}/*nl.
-------------------------------------------*/
/*============= end of file xc.c ==========================*/

