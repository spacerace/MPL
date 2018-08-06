/***********************************************************/
/*    XC.C						   */
/*							   */
/*    Last modified:	22:50	20 May	84	RSW	   */
/*							   */
/*    Abstract:						   */
/*							   */
/*    'XC' is a	cross-reference	utility	for 'C'	programs.  */
/*    Its has the ability to handle nested include files   */
/*    to a depth of 8 levels and properly processes nested */
/*    comments as supported by BDS C. Option flags support */
/*    the following features:				   */
/*							   */
/*    -	Routing	of list	output to disk			   */
/*    -	Cross-referencing of reserved words		   */
/*    -	Processing of nested include files		   */
/*    -	Generation of listing only			   */
/*							   */
/*    Usage: xc	<filename> <flag(s)>			   */
/*							   */
/*    Flags: -i		   = Enable file inclusion	   */
/*	     -l		   = Generate listing only	   */
/*	     -c		   = Compressed	print listing	   */
/*	     -r		   = Cross-ref reserved	words	   */
/*	     -o	<filename> = Write output to named file	   */
/*							   */
/*************************************************************************/
/*  5-20-84  Changes			R.S. White			 */
/*     - made compilation for Lattice C	ver 2.0	or Computer Innovations	 */
/*	      CI-C86 optional by setting #define labels	accordingly.	 */
/*     - made printer selection	for compressed print selectable	by	 */
/*	      setting #define labels accordingly.  Added Epson FX and	 */
/*	      Gemini-10X printer control strings.			 */
/*************************************************************************/
/*  1-01-84  Enhancements		S.R. Jacobson			 */
/*     - changed -e option to -c option	and added two strings:		 */
/*	      cprint_hdr sets the printer to compressed	print mode	 */
/*	      cprint_trlr resets the printer to	"normal" mode		 */
/*	   This	makes it easier	for non-epson printer users		 */
/*     -  removed all the BDS C	comments (this makes it	easier to modify */
/*	      in the future.						 */
/*     -  changed name of the utility to the "print" utility		 */
/*     -  if the -i flag is not	used, the second line number will not	 */
/*	      appear.							 */
/*     -  if a form feed is detected in	the source file, a new page is	 */
/*	      started.	This is	useful for structuring the source	 */
/*	      code by placing each major section on a separate page.	 */
/*	      You do this by embedding a form feed (control L) in a	 */
/*	      comment.	The text following the form feed will be placed	 */
/*	      on the top of the	new page.				 */
/*     -  tab characters in the	file are expanded to every 8 positions.	 */
/*	      This may be changed by changing the TABCNT define.	 */
/*     -  added	the date and time to the printed output. (C-86 only)	 */
/*     -  fixed	error in hashing algorithm.				 */
/*     -  control characters (other than \n, \f, or \t)	are stripped	 */
/*	      from the input stream (converted to blanks).  This	 */
/*	      prevents extraneous control chars	from interfering with	 */
/*	      operation	of xc.						 */
/*     -  cleaned up the structuring of	a number of routines.		 */
/*     -  added	the file latest	update date and	time to	the printed	 */
/*	      output. ***DOS 2.0 ONLY ***				 */
/*************************************************************************/
/*   9-26-83   Microsoft C 1.04	 Conversion    WHR			 */
/*     -  \t between line numbers and text to fix indenting problem.	 */
/*     -  added	option -e for output to	Epson in condensed print.	 */
/*     -  toupper() and	isupper() are macros, not functions.		 */
/*     -  eliminate side effect	in toupper(*++arg) in main().		 */
/*     -  change alloc() to malloc().					 */
/*     -  add #define NAMES that are not in stdio.h			 */
/*     -  MS-C requires	() in statement	A?(c=B):(c=C)	error or not??	 */
/*									 */
/*   4-30-83   Computer	Innovations C-86 1.31 Conversion    WHR		 */
/*     -  #include filename changed to allow a disk drive prefix, D:	 */
/*     -  convert if(fprintf(...) == ERROR) lst_err(); to fprintf(..);	 */
/*     -  convert if(fopen(...)	== ERROR) statements to	== NUL0.	 */
/*     -  C86 requires () in statement A?(c=B):(c=C)	error or not??	 */
/*     -  remove getc()	== ERROR check in fil_chr().			 */
/*     -  convert file conventions from	BDS C to C-86.			 */
/*     -  comment out BDS unique statements, mark revised statements.	 */
/*	  keep all BDS statements to document conversion effort.	 */
/*									 */
/**  4-19-83   BDS C Version file XC.CQ	copied from Laurel RCPM	   WHR	 */
/*************************************************************************/
/*   **	Original Version: **				   */
/*							   */
/*    Version 1.0   January, 1982			   */
/*							   */
/*    Copyright	(c) 1982 by Philip N. Hisley		   */
/*							   */
/*    Released for non-commercial distribution only	   */
/*							   */
/*    Please report bugs/fixes/enhancements to:		   */
/*							   */
/*	      Philip N.	Hisley				   */
/*	      548H Jamestown Court			   */
/*	      Edgewood,	Maryland 21040			   */
/*	      (301) 679-4606				   */
/*	      Net Addr:	PNH@MIT-AI			   */
/*							   */
/*							   */
/***********************************************************/

/*  DEFINITIONS AND VARIABLES	*/				/*SRJ*/

#include <stdio.h>					    /* WHR */

#define	LATTICE	1	/* set to use Lattice C	ver 2.0	compiler */ /*RSW*/
#define	CIC86	0	/* disable Computer Innovations	compiler */ /*RSW*/

#define	GEMIN10X 0	/* enable Gemini-10X printer codes	*/  /*RSW*/
#define	EPSONMX	 0						    /*RSW*/
#define	EPSONFX	 0						    /*RSW*/
#define	IDSPRISM 1						    /*RSW*/

#if LATTICE
#include <ctype.h>	/* Lattice C ver 2.0 - macros */	    /*RSW*/
#endif

#define	 TABCNT	    8		/* expand tabs to every	8 characters */	 /*SRJ*/
#define	 NUL0	    0
#define	 FALSE	    0					      /* WHR */
#define	 TRUE	    1					      /* WHR */
#define	 CPMEOF	  0x1A		      /* end of	file */	      /* WHR */
#define	 ERROR	  (-1)					      /* WHR */

#define	 MAX_REF    5		/* maximum refs	per ref-block */

#define	 MAX_LEN 39  /*	maximum	identifier length -n opt Lattice C */ /*RSW*/
#define	 MAX_WRD   749		/* maximum number of identifiers */
#define	 MX_ALPHA  53		/* maximum alpha chain heads */	      /*RSW*/
#define	 REFS_LIN  8		/* maximum refs	per line */	      /*RSW*/
#define	 LINE_PAG  60	/* note	Lattice	C gets confused	if -n  */     /*RSW*/
			/* option is used and #define labels   */
			/* are longer than 8 characters	       */

struct	id_blk {
		 char  id_name[MAX_LEN];
		 struct	id_blk *alpha_lnk;
		 struct	rf_blk *top_lnk;
		 struct	rf_blk *lst_lnk;
	       } oneid;

struct	rf_blk {
		 int  ref_item[MAX_REF];
		 int  ref_cnt;
	       } onerf;

struct id_blk *id_vector[MAX_WRD];

struct alpha_hdr { struct id_blk *alpha_top;
		   struct id_blk *alpha_lst;
		 };

struct alpha_hdr alpha_vector[MX_ALPHA];

struct	regval	{
		unsigned int ax,bx,cx,dx,si,di,ds,es;
		} reg; /*structure for sysint*/			   /*SRJ*/

struct	regv	{
		unsigned int csx,ssx,dsx,esx;
		} regx;	   /* structure	for segread*/		   /*SRJ*/


int	y,mo,d;			/* year,month and day for date */  /*SRJ*/
int	h,mi,s;			/* hours, minutes, seconds for time */ /*SRJ*/

char	*months[]=		/* month names for date	*/		/*SRJ*/
		{"bad month","January","February","March","April","May","June",
		"July","August","September","October","November","December"};

int	ln_chr=0;		/* # of	characters in the line (for tab)SRJ*/

unsigned int	handle;			/* dos file handle */		/*SRJ*/
unsigned int file_d,file_mo,file_y,file_h,file_mi; /* file m/d/y and h:m  SRJ*/

FILE	*tb;			/* temp	fd */				/*SRJ*/


int	linum;		/* line	number */
int	edtnum;		/* edit	line number */
int	fil_cnt;	/* active file index */
int	wrd_cnt;	/* token count */
int	pagno;		/* page	number */
int	id_cnt;		/* number of unique identifiers	*/
int	rhsh_cnt;	/* number of conflict hits */
int	filevl;		/* file	level  */
int	paglin;		/* page	line counter */
int	prt_ref;
char	act_fil[MAX_LEN];
char	lst_fil[MAX_LEN];
char	gbl_fil[MAX_LEN];
FILE   *l_buffer;					     /*	  WHR */
int	i_flg,o_flg,r_flg,l_flg;
int	debug;
int	c_flg;						     /*	WHR */


/* The following two strings are the compressed	print header and the
trailer	that returns the printer to the	normal mode.  These must
be changed for each printer.  They are currently set up	for the	  */

#if GEMIN10X								/*RSW*/
char cprint_hdr[]="\x1bB\3\1bQ\x84"; /*	header string for Gemini-10X */
char cprint_trlr[]="\x1bB\2"; /* ELITE mode trailer string for Gemini-10X */
#endif

#if IDSPRISM								/*RSW*/
char	cprint_hdr[] = "\037";  /* header string for Prism 80 */
char	cprint_trlr[] =	"\035";	/* trailer string for Prism 80 */
#endif

#if EPSONMX								/*RSW*/
char	cprint_hdr[] = "\x1b\x40\x0F\x1BQ\x84";				/*WHR*/
char	cprint_trlr[] =	"\x1b\x40";    /* trailer string for Epson MX */
#endif

#if EPSONFX								/*RSW*/
char cprint_hdr[]="\x1b!\4\x1bQ\x84"; /* header	string for Epson FX */	/*WHR*/
char cprint_trlr[]="\x1b!\1"; /*ELITE mode trailer string for Epson FX*/ /*RSW*/
#endif


/* end of printer strings */
/*  MAIN PROGRAM	*/					/*SRJ*/


main(argc,argv)
     int     argc;
     char    **argv;

     {char  *arg;
     char *strcpy();
/*** int  toupper();   ***/		     /*	Microsoft C   macro */
     char cc;				     /*	Microsoft C */

/*  The	the parameters	*/

     if	(argc <	2) use_err();
     i_flg=r_flg=o_flg=l_flg=FALSE;
     debug = FALSE;
     strcpy(gbl_fil,*++argv);
     --argc;
     if(gbl_fil[0] == '-')
	  use_err();
     while(--argc != 0)
     {	  if(*(arg=*++argv) == '-')
    /*****{    switch( toupper(*++arg) )     *** side effect in	Microsoft C */
	  {    switch( cc=*++arg, toupper(cc) )		     /*	Microsoft C */
	       {   case	'I':  i_flg++;
			      break;
		   case	'R':  r_flg++;
			      break;
		   case	'L':  l_flg++;
			      break;
		   case	'O': {o_flg++;
			      if(--argc	== 0) use_err();
			      strcpy(lst_fil,*++argv);
			      if(lst_fil[0] == '-') use_err();
			      if(debug)	printf("lst_fil=>%s<",lst_fil);
			      break;
			     }
		   case	'D':  debug++;
			      break;
		   case	'C':  c_flg++;			/* SRJ */ /* WHR */
			      o_flg++;
			      strcpy(lst_fil,"PRN:");	/* SRJ */
			      break;
		   default:   use_err();
	       }
	  }
     else use_err();
     }
     if(debug) printf("\ni_flg=%d, r_flg=%d, l_flg=%d",	i_flg,r_flg,l_flg);
     if(debug) printf("\no_flg=%d, debug=%d", o_flg,debug);
     if	(o_flg)
	  {if (	(l_buffer = fopen(lst_fil,"w"))	== NUL0)   /***	output file ***/
	      {printf("ERROR: Unable to create list file - %s\n",lst_fil);
	       exit(0);
	      }
	  printf("\nXC ....... 'C' Print Utility  v1.1\n");		/*RSW*/
	  if (c_flg) fprintf(l_buffer,"%s",cprint_hdr);
	  }
      else								/*SRJ*/
	  l_buffer=stdout;	/* if no output	file, use stdout */	/*SRJ*/

/* THE FOLLOWING CODE GETS THE DATE AND	THE TIME FROM DOS.  IT USES THE
ROUTINES SYSINT	AND SEGREAD PROVIDED BY	THE CI C-86 LIBRARY.  THIS IS CI C-86
DEPENDENT CODE AND MUST	BE CHANGED FOR OTHER COMPILERS		 */	/*SRJ*/
/* added Lattice C ver 2.00 int86 calls	to do the same */		/*RSW*/

segread(&regx);		/* get the segment registers */			/*SRJ*/
reg.ds=regx.dsx;	/* save	the ds in the sysint data */		/*SRJ*/
reg.es=regx.esx;	/* save	the es in the sysint data */		/*SRJ*/

/* get the date	*/							/*SRJ*/
reg.ax=0x2A00;			/* date	request	code */			/*SRJ*/

#if CIC86
sysint(0x21,&reg,&reg);		/* DOS interrupt */			/*SRJ*/
#endif

#if LATTICE
int86(0x21,&reg,&reg);		/* Lattice C ver 2.0 interupt */	/*RSW*/
#endif

y=reg.cx;			/* store year */			/*SRJ*/
mo=reg.dx>>8;			/* store month */			/*SRJ*/
d=reg.dx & 0xFF;		/* get the day */			/*SRJ*/

/* get the time	*/							/*SRJ*/
reg.ax=0x2C00;			/* time	request	code */			/*SRJ*/

#if CIC86
sysint(0x21,&reg,&reg);		/* DOS interrupt */			/*SRJ*/
#endif

#if LATTICE
int86(0x21,&reg,&reg);		/* Lattice C ver 2.0 interupt */	/*RSW*/
#endif

h=reg.cx>>8;			/* get hours */				/*SRJ*/
mi=reg.cx & 0xFF;		/* get minutes */			/*SRJ*/
s=reg.dx>>8;			/* get seconds */			/*SRJ*/

/* NOW WE GET THE FILE LATEST UPDATE DATE AND TIME */

/* first try to	open the file */

if ((tb	= fopen(gbl_fil,"r")) == NUL0)	/*open the input file */	/*SRJ*/
	{printf("\nERROR: Unable to open input file: %s\n",gbl_fil);	/*SRJ*/
	exit(10);							/*SRJ*/
	}
fclose(tb);			/* close the file */			/*SRJ*/

/* note-we have	no way to get an error indication back from the	dos call
below (file open),  so we have to open it to see if it is there	before
we can use the proceedure below	*/					/*SRJ*/

/* now get the file date and time */					/*SRJ*/

/* open	the file   -  we assume	the file exists	*/			/*SRJ*/
reg.ax=0x3D00;			/* open	request	code */			/*SRJ*/

/* reg.dx=gbl_fil;   */		/* address of file name	*/		/*SRJ*/
reg.dx = &gbl_fil;		/* address of file name	*/		/*RSW*/

#if CIC86
sysint(0x21,&reg,&reg);		/* DOS interrupt */			/*SRJ*/
#endif

#if LATTICE
int86(0x21,&reg,&reg);		/* Lattice C ver 2.0 interupt */	/*RSW*/
#endif

handle=reg.ax;			/* store file handle */			/*SRJ*/

/* get the date	and time */						/*SRJ*/
reg.ax=0x5700;			/* time	request	code */			/*SRJ*/
reg.bx=handle;			/* file	handle in bx */			/*SRJ*/

#if CIC86
sysint(0x21,&reg,&reg);		/* DOS interrupt */			/*SRJ*/
#endif

#if LATTICE
int86(0x21,&reg,&reg);		/* Lattice C ver 2.0 interupt */	/*RSW*/
#endif


/* unpack the file time	*/						/*SRJ*/
file_h=(reg.cx >> 11) &	0x1F;		/* get hours */			/*SRJ*/
file_mi=(reg.cx	>> 5) &	0x3f;		/* get minutes */		/*SRJ*/

/* unpack the file date	*/						/*SRJ*/
file_d=reg.dx &	0x1F;			/* get the day */		/*SRJ*/
file_mo= (reg.dx >> 5) & 0x0F;		/* get the month */		/*SRJ*/

file_y=( (reg.dx >> 9) & 0x7F )+80;	/* get the month */		/*SRJ*/

/* close the file */							/*SRJ*/

reg.bx=handle;								/*SRJ*/
reg.ax=0x3D00;								/*SRJ*/

#if CIC86
sysint(0x21,&reg,&reg);		/* DOS interrupt */			/*SRJ*/
#endif

#if LATTICE
int86(0x21,&reg,&reg);		/* Lattice C ver 2.0 interupt */	/*RSW*/
#endif



/* END OF compiler DEPENDENT CODE  */					/*RSW*/


     prt_ref = FALSE;
     for(linum=0;linum < MAX_WRD;linum++)
	  id_vector[linum] = NUL0;

     for(linum=0;linum < MX_ALPHA;linum++)
	  alpha_vector[linum].alpha_top	=
	       alpha_vector[linum].alpha_lst = NUL0;

     fil_cnt = wrd_cnt = linum = 0;
     filevl=paglin=pagno=edtnum=0;
     id_cnt=rhsh_cnt=0;
     proc_file(gbl_fil);
     if(!l_flg)
	 {prnt_tbl();
	 printf("\nAllowable Symbols: %d\n",MAX_WRD);
	 printf("Unique    Symbols: %d\n",id_cnt);
	 }
     if(o_flg)
	  {nl();
	  fprintf(l_buffer,"\nAllowable Symbols: %d\n",MAX_WRD);   /* WHR */
	  fprintf(l_buffer,"Unique    Symbols: %d\n",id_cnt);	   /* WHR */
	  fprintf(l_buffer,"\f%c",CPMEOF);			/* WHR */
	  fprintf(l_buffer,"%s",cprint_trlr); /* output	trailer	*/ /* SRJ */
	  fflush(l_buffer);
	  fclose(l_buffer);
	  }
}
/*   ERROR MESSAGE PRINT ROUTINES */				/*SRJ*/

lst_err()

{printf("\nERROR: Write error on list output file - %s\n",lst_fil);
exit(0);
}


use_err()

{printf("\nERROR: Invalid parameter specification\n\n");
printf("Usage: xc <filename> <flag(s)>\n\n");
printf("Flags: -i             = Enable file inclusion\n");
printf("       -l             = Generate listing only\n");
printf("       -c             = Compressed print option\n");  /*SRJ*/
printf("       -r             = Cross-reference reserved words\n");
printf("       -o <filename>  = Write output to named file\n");
exit(0);
}

/*   PROCESS THE FILE	*/					/*SRJ*/

proc_file(filnam)

char	*filnam;

{FILE  *buffer;	   /* allocated	buffer pointer */	    /* WHR */
char  token[MAX_LEN]; /* token buffer */
int   eof_flg;	   /* end-of-file indicator */
int   tok_len;	   /* token length */
int   incnum;	   /* included line number */
char *strcpy();

strcpy(act_fil,filnam);
if ((buffer = fopen(filnam,"r")) == NUL0)  /***	input file ***/	/* WHR */
	{printf("\nERROR: Unable to open input file: %s\n",filnam);
	exit(0);
	}
if(filevl++ == 0)
	{prt_hdr();
	nl();
	}
eof_flg	= FALSE;

do
	{if(get_token(buffer,token,&tok_len,&eof_flg,0))
		{if (debug) printf("token: %s   length: %d\n",token,tok_len);
		if (chk_token(token))

		     /*	#include processing changed to accept drive:   WHR */
			{if (strcmp(token,"#include") == 0)
				{if (get_token(buffer,token,&tok_len,&eof_flg,1))
					{if (debug) printf("**token: %s   length: %d\n",
						  token,tok_len);
					if (!i_flg)
						continue;
					  else
						{incnum=edtnum;
						edtnum=0;
						nl();
						proc_file(token);
						edtnum=incnum;
						strcpy(act_fil,filnam);
						continue;
						}
					}
				}
			put_token(token,linum);
			}
		}
	}
while (!eof_flg);

filevl -= 1;
fclose(buffer);
return;
}
/*   GET A TOKEN    */						/*SRJ*/

get_token(g_buffer,g_token,g_toklen,g_eoflg,g_flg)

FILE	*g_buffer;
char	*g_token;
int	*g_toklen;
int	*g_eoflg;
int	g_flg;

/*
*	 'getoken' returns the next valid identifier or
*	 reserved word from a given file along with the
*	 character length of the token and an end-of-file
*	 indicator
*
*/

{
     int     c;
     char    *h_token;
     char    tmpchr;
     char    tmpchr2;		      /* WHR fix for B:filename.ext */

     h_token = g_token;

 gtk:			       /* top of loop, get new token */
     *g_toklen = 0;
     g_token = h_token;

    /*
     *	Scan and discard any characters	until an alphabetic or
     *	'_' (underscore) character is encountered or an	end-of-file
     *	condition occurs
     */

     while(  (!isalpha(*g_token	= rdchr(g_buffer,g_eoflg,g_flg)))
	     &&	 !*g_eoflg	&&  *g_token !=	'_'
	     &&	*g_token != '0'	&&  *g_token !=	'#' );
     if(*g_eoflg) return(FALSE);

     *g_toklen += 1;

    /*
     *	 Scan and collect identified alpanumeric token until
     *	 a non-alphanumeric character is encountered or	and
     *	 end-of-file condition occurs
     */

     if(g_flg) {
	  tmpchr  = '.';
	  tmpchr2 = ':';		   /* WHR fix for B:filename.ext */
     }
     else {
	  tmpchr  = '_';
	  tmpchr2 = '_';		   /* WHR fix for B:filename.ext */
     }
     while( (isalpha(c=rdchr(g_buffer,g_eoflg,g_flg)) ||
	    isdigit(c) || c == '_' || c	== tmpchr || c == tmpchr2)
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


     if	(*h_token == '0') goto gtk;


    /*
     *	    Tack a NUL0	character onto the end of the token
     */

     *++g_token	= NUL0;

    /*
     *	    Screen out all #token strings except #include
     */

     if	(*h_token == '#' && strcmp(h_token,"#include"))	goto gtk;

     return (TRUE);
}
/*  READ A CHARACTER FROM THE FILE AND	PROCESS	IT */		/*SRJ*/


rdchr(r_buffer,r_eoflg,rd_flg)
     int     *r_eoflg;
     FILE    *r_buffer;
     int     rd_flg;

/*
	'rdchr'	returns	the next valid character in a file
	and an end-of-file indicator. A	valid character	is
	defined	as any which does not appear in	either a
	commented or a quoted string ... 'rdchr' will correctly
	handle comment tokens which appear within a quoted
	string
*/

{
     int     c;
     int     q_flg;	     /*	double quoted string flag */
     int     q1_flg;	     /*	single quoted string flag */
     int     cs_flg;	     /*	comment	start flag */
     int     ce_flg;	     /*	comment	end flag */
     int     c_cnt;	     /*	comment	nesting	level */
     int     t_flg;	     /*	transparency flag */

     q_flg = FALSE;
     q1_flg = FALSE;
     cs_flg = FALSE;
     ce_flg = FALSE;
     t_flg = FALSE;
     c_cnt  = 0;

rch:


    /*
     *	 Fetch character from file
     */

     c = fil_chr(r_buffer,r_eoflg) & 0x7F;	/* read	only 7 bit ascii */ /*SRJ*/

     if	(*r_eoflg) return(c);	/* EOF encountered */

     switch(c)			/* test	the returned character */   /*SRJ*/
	{case '\n':		/* carriage return */		    /*SRJ*/
		nl();		/* output new line and line numbers */ /*SRJ*/
		ln_chr=0;	/* set to first	char in	line */		/*SRJ*/
		c=' ';		/* return a space */			/*SRJ*/
		break;

	case '\f':		/* form	feed */			    /*SRJ*/
		prt_hdr();	/* output header and start new page*/ /*SRJ*/
		fprintf(l_buffer,"     "); /* space over line #s */   /*SRJ*/
		ln_chr=0;	/* set to first	char in	line */		/*SRJ*/
		c=' ';		/* set char to blank */			/*SRJ*/
		break;

	case '\t':		/* tab character */		/*SRJ*/
		do		/* expand the tabs */		/*SRJ*/
			fputc(' ',l_buffer);			/*SRJ*/
		while( (++ln_chr % TABCNT) != 0);		/*SRJ*/
		c=' ';		/* set char to blank */			/*SRJ*/
		break;


	default:
		ln_chr++;		/* move	to next	char pos */  /*SRJ*/
		if( iscntrl(c) )	/* insure no control chars */ /*SRJ*/
			c=' ';
		fputc(c,l_buffer);	/* and output next char	*/   /*SRJ*/
	}

     if	(rd_flg) return(c);

     if	(t_flg)	{
	  t_flg	= !t_flg;
	  goto rch;
     }

     if	(c == '\\') {
	  t_flg	= TRUE;
	  goto rch;
     }
	/*
	If the character is not	part of	a quoted string
	check for and process commented	strings...
	nested comments	are handled correctly but unbalanced
	comments are not ... the assumption is made that
	the syntax of the program being	xref'd is correct
	*/

     if	(!q_flg	 &&  !q1_flg) {
	  if (c	== '*'	&&  c_cnt  &&  !cs_flg)	{
	       ce_flg =	TRUE;
	       goto rch;
	  }
	  if (c	== '/'	&&  ce_flg) {
	       c_cnt -=	1;
	       ce_flg =	FALSE;
	       goto rch;
	  }
	  ce_flg = FALSE;
	  if (c	== '/')	{
	       cs_flg =	TRUE;
	       goto rch;
	  }
	  if (c	== '*'	&&  cs_flg) {
	       c_cnt +=	1;
	       cs_flg =	FALSE;
	       goto rch;
	  }
	  cs_flg = FALSE;

	  if (c_cnt) goto rch;
     }

	/*
	Check for and process quoted strings
	*/

     if	( c == '"'  &&  !q1_flg) {      /* toggle quote flag */
	  q_flg	=  !q_flg;
	  if(debug) printf("q_flg toggled to: %d\n" ,q_flg);
	  goto rch;
     }
     if	(q_flg)	goto rch;

     if	(c == '\'') {        /* toggle quote flag */
	  q1_flg = !q1_flg;
	  if(debug) printf("q1_flg toggled to: %d\n" ,q1_flg);
	  goto rch;
     }
     if	(q1_flg) goto rch;

	/*
	Valid character	... return to caller
	*/

     return (c);
}/*rdchr.
-----------------------------------------------*/
fil_chr(f_buffer,f_eof)
     FILE *f_buffer;
     int *f_eof;
{
     int fc;
     fc=getc(f_buffer);
     if	(fc == CPMEOF || fc == EOF) {
	  *f_eof = TRUE;
	  fc = NUL0;
     }
     return(fc);
}
/*   CHECK THE	TOKEN  */				/*SRJ*/


chk_token(c_token)
     char    *c_token;
{
     char  u_token[MAX_LEN];
     int   i;

     {
      if (r_flg) return(TRUE);
      i	= 0;
      do { u_token[i] =	toupper(c_token[i]);
      }	while (c_token[i++] != NUL0);

      switch(u_token[0]) {
	case 'A': if (strcmp(u_token,"AUTO") ==	0) return(FALSE);
		  break;
	case 'B': if (strcmp(u_token,"BREAK") == 0) return(FALSE);
		  break;
	case 'C': if (strcmp(u_token,"CHAR") ==	0) return (FALSE);
		  if (strcmp(u_token,"CONTINUE") == 0) return (FALSE);
		  if (strcmp(u_token,"CASE") ==	0) return (FALSE);
		  break;

	case 'D': if(strcmp(u_token,"DOUBLE") == 0) return(FALSE);
		  if(strcmp(u_token,"DO") == 0)	return(FALSE);
		  if(strcmp(u_token,"DEFAULT") == 0) return(FALSE);
		  break;
	case 'E': if(strcmp(u_token,"EXTERN") == 0) return(FALSE);
		  if(strcmp(u_token,"ELSE") == 0) return(FALSE);
		  if(strcmp(u_token,"ENTRY") ==	0) return(FALSE);
		  break;
	case 'F': if(strcmp(u_token,"FLOAT") ==	0) return(FALSE);
		  if(strcmp(u_token,"FOR") == 0) return(FALSE);
		  break;
	case 'G': if(strcmp(u_token,"GOTO") == 0) return(FALSE);
		  break;
	case 'I': if(strcmp(u_token,"INT") == 0) return(FALSE);
		  if(strcmp(u_token,"IF") == 0)	return(FALSE);
		  break;
	case 'L': if(strcmp(u_token,"LONG") == 0) return(FALSE);
		  break;
	case 'R': if(strcmp(u_token,"RETURN") == 0) return(FALSE);
		  if(strcmp(u_token,"REGISTER")	== 0) return(FALSE);
		  break;
	case 'S': if(strcmp(u_token,"STRUCT") == 0) return(FALSE);
		  if(strcmp(u_token,"SHORT") ==	0) return(FALSE);
		  if(strcmp(u_token,"STATIC") == 0) return(FALSE);
		  if(strcmp(u_token,"SIZEOF") == 0) return(FALSE);
		  if(strcmp(u_token,"SWITCH") == 0) return(FALSE);
		  break;
	case 'T': if(strcmp(u_token,"TYPEDEF") == 0) return(FALSE);
		  break;
	case 'U': if(strcmp(u_token,"UNION") ==	0) return(FALSE);
		  if(strcmp(u_token,"UNSIGNED")	== 0) return(FALSE);
		  break;
	case 'W': if(strcmp(u_token,"WHILE") ==	0) return(FALSE);
		  break; }
	}
  return (TRUE);
}/*chk_token.
---------------------------------------------*/
/*   STORE THE	TOKEN  */					/*SRJ*/

  /*
   *	Install	parsed token and line reference	in linked structure
   */

put_token(p_token,p_ref)
     char *p_token;
     int  p_ref;
{
     int  hsh_index;
     int  i;
/*   unsigned long j;	*/						/*SRJ*/
     long int j;					/*RSW*/
     int  d;
     int  found;
     struct id_blk *idptr;
     struct rf_blk *rfptr;
     struct id_blk *alloc_id();
     struct rf_blk *alloc_rf();
     struct rf_blk *add_rf();

     if	(l_flg)	return;
     j=0;
     for (i=0; p_token[i] != NUL0; i++)	 /* Hashing algorithm is far from */
	 j = j * 10 + p_token[i];	 /* memory-bound index vector!	  */

     hsh_index = j % MAX_WRD;						/*SRJ*/

     if(debug) printf("hash index=%d ",hsh_index);

     found = FALSE;
     d = 1;
     do
	  {idptr = id_vector[hsh_index];
	  if (idptr == NUL0)
	       {id_cnt++;
	       idptr = id_vector[hsh_index] = alloc_id(p_token);
	       chain_alpha(idptr,p_token);
	       idptr->top_lnk =	idptr->lst_lnk = alloc_rf(p_ref);
	       found = TRUE;
	       if(debug) printf("empty cell in vector\n");
	       }
	    else
	       if (strcmp(p_token,idptr->id_name) == 0)
		   {idptr->lst_lnk = add_rf(idptr->lst_lnk,p_ref);
		   if(debug) printf("duplicate token\n");
		   found = TRUE;
		   }
		else
		   {hsh_index += d;
		   if(debug) printf("hash clash  hash index=%d\n",hsh_index);
		   d +=	2;
		   rhsh_cnt++;
		   if (hsh_index >= MAX_WRD)
			 hsh_index -= MAX_WRD;
		   if (d == MAX_WRD)
			 {printf("\nERROR: Symbol table overflow\n");
			 exit(0);
			 }
		   }
	  }
      while (!found);

return;
}/*put_token.
--------------------------------------------*/
/*    BUILD THE CHAINS	*/					/*SRJ*/

chain_alpha(ca_ptr,ca_token)
     struct id_blk *ca_ptr;
     char  *ca_token;
{
     char  c;
     int   f;
     struct id_blk *cur_ptr;
     struct id_blk *lst_ptr;
/*** int isupper();   ****/	       /** Microsoft C	 macro **/


     c = ca_token[0];
     if	(c == '_')  c =	0;
     else
     /**  isupper(c) ? c=1+((c-'A')*2) : c=2+((c-'a')*2) ;     error or	not??
      **  A good one for the puzzle book! Is the () required around (c=..)?
      **  C86 and Microsoft C both req the ()'s, BDS C did not.
      **  Is it	required because = has lower precedence	than ?:	????
      **/
	  isupper(c) ? (c=1+((c-'A')*2)) : (c=2+((c-'a')*2)) ;

     if(alpha_vector[c].alpha_top == NUL0)
     {	  alpha_vector[c].alpha_top =
		  alpha_vector[c].alpha_lst = ca_ptr;
	  ca_ptr->alpha_lnk = NUL0;
	  return;
     }

    /*	check to see if	new id_blk should be inserted between
     *	the alpha_vector header	block and the first id_blk in
     *	the current alpha chain
     */

     if(strcmp(alpha_vector[c].alpha_top->id_name,ca_token) >0)
     {	  ca_ptr->alpha_lnk=alpha_vector[c].alpha_top;
	  alpha_vector[c].alpha_top=ca_ptr;
	  return;
     }

     if(strcmp(alpha_vector[c].alpha_lst->id_name,ca_token) < 0)
     {	  alpha_vector[c].alpha_lst->alpha_lnk = ca_ptr;
	  ca_ptr->alpha_lnk = NUL0;
	  alpha_vector[c].alpha_lst=ca_ptr;
	  return;
     }

     cur_ptr = alpha_vector[c].alpha_top;
     while(strcmp(cur_ptr->id_name,ca_token) < 0)
     {	  lst_ptr = cur_ptr;
	  cur_ptr = lst_ptr->alpha_lnk;
     }

     lst_ptr->alpha_lnk	= ca_ptr;
     ca_ptr->alpha_lnk = cur_ptr;
     return;
}/*chain_alpha.
-----------------------------------------*/
/*   GET MEMORY FOR TOKEN TREE	*/				/*SRJ*/
     struct id_blk
*alloc_id(aid_token)
     char  *aid_token;
{
     int  ai;
     struct id_blk *aid_ptr;
     char *malloc();					     /*	Microsoft C */

/*** if((aid_ptr =  alloc(sizeof(oneid))) == 0)	{ ***/	     /*	Microsoft C */
     if((aid_ptr = (struct id_blk *) malloc(sizeof(oneid))) == 0) { /* MS C */

	  printf("\nERROR: Unable to allocate identifier block\n");
	  exit(0);
     }
     ai=0;
     do	{
	  aid_ptr->id_name[ai] = aid_token[ai];
     } while (aid_token[ai++] != NUL0);
     return (aid_ptr);
}/*id_blk.
-----------------------------------------*/

     struct rf_blk
*alloc_rf(arf_ref)
     int  arf_ref;
{
    int	ri;
    struct rf_blk *arf_ptr;
    char * malloc();				       /* Microsoft C */

/** if((arf_ptr	= alloc(sizeof(onerf)))	== 0) {	**/    /* Microsoft C */
    if((arf_ptr	= (struct rf_blk *) malloc(sizeof(onerf))) == 0) { /* MS C */
	 printf("\nERROR: Unable to allocate reference block\n");
	 exit(0);
    }
    arf_ptr->ref_item[0] = arf_ref;
    arf_ptr->ref_cnt = 1;
    for	(ri=1;ri<MAX_REF;ri++)
	  arf_ptr->ref_item[ri]	= NUL0;
    return (arf_ptr);
}/*alloc_rf.
------------------------------------------*/
/*   STORE REFERENCE IN TREE */					/*SRJ*/

     struct rf_blk
*add_rf(adr_ptr,adr_ref)

     struct rf_blk *adr_ptr;
     int adr_ref;
{
     struct rf_blk *tmp_ptr;

     tmp_ptr = adr_ptr;
     if(adr_ptr->ref_cnt == MAX_REF) {
     /*** tmp_ptr = adr_ptr->ref_cnt = alloc_rf(adr_ref);    Microsoft C **/
	  adr_ptr->ref_cnt = alloc_rf(adr_ref);		  /* Microsoft C **/
	  tmp_ptr = (struct rf_blk *) adr_ptr->ref_cnt;	  /* Microsoft C **/
     }
     else
     {	  adr_ptr->ref_item[adr_ptr->ref_cnt++]	= adr_ref;
     }
     return (tmp_ptr);
}/*rf_blk.
------------------------------------------*/
/*   PRINT THE	CROSS REFERENCE	TABLE */				/*SRJ*/

prnt_tbl()
{
     int prf_cnt;
     int pti;
     int pref;
     int lin_cnt;
     struct id_blk *pid_ptr;
     struct rf_blk *ptb_ptr;

     prt_ref = TRUE;
     prt_hdr();
     nl();
     for (pti=0;pti<MX_ALPHA;pti++)
	  {if ((pid_ptr	= alpha_vector[pti].alpha_top) != NUL0)
	       {do
		    {fprintf(l_buffer,"%-14.13s: ",pid_ptr->id_name);/*WHR*/
		    ptb_ptr=pid_ptr->top_lnk;
		    lin_cnt=prf_cnt=0;
		    do
			 {if(prf_cnt ==	MAX_REF)
			      {prf_cnt=0;
			 /*** ptb_ptr =	ptb_ptr->ref_cnt;  Microsoft C **/
			      ptb_ptr =	(struct	rf_blk *)ptb_ptr->ref_cnt;
			      }
			 if(ptb_ptr > MAX_REF)
				 {if((pref=ptb_ptr->ref_item[prf_cnt++]) != 0)
					  {fprintf(l_buffer,"%-4d  ",pref); /*WHR*/
					  if (++lin_cnt	== REFS_LIN )
						{nl();
						fprintf(l_buffer,"%16s",": ");
						lin_cnt=0;
						}
					  }
				 }
				 else
					   pref=0;
			 }
		    while (pref);
		    nl();
		    }
	       while ((pid_ptr=pid_ptr->alpha_lnk) != NUL0);
	       }
	  }
}/*prnt_tbl.
---------------------------------------*/
/*   TOP OF PAGE AND PRINT THE	HEADER */			/*SRJ*/

prt_hdr()

{fprintf(l_buffer,
  "\r\f%-10s [file d/t= %d/%02d/%02d  %d:%02d]   [clock d/t= %d %s %d  %d:%02d:%02d]   Page %d\n\n",
	gbl_fil,file_mo,file_d,file_y,file_h,file_mi,
	d,months[mo],y,h,mi,s,++pagno);		/*SRJ*/

paglin =3;
return;
}
/*  NEW LINE */							/*SRJ*/

nl()

{fputc('\n',l_buffer);	/* output cr/lf	*/			/*SRJ*/

if (++paglin >=	LINE_PAG )
	prt_hdr();
if(!prt_ref)
	{fprintf(l_buffer,"%4d ",++linum);		/*SRJ*/
	if(i_flg)				/*SRJ*/
		fprintf(l_buffer," %4d: ",++edtnum);	/*SRJ*//*WHR*/
	}
/* print the status on the console */
if(o_flg)
	if(linum % 60 == 1)
		printf("\n<%d>\t",linum);
	  else
		printf(".");
return;
}/*nl.
-------------------------------------------*/
/*=============	end of file xc.c ==========================*/
                                                                  