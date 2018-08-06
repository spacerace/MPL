/* : : : : : : : : : : : : : : : : : : : : : : : : : : : : : :
	Program to compare (byte for byte)
	two files and print differences

	H.Moran		10/27/79
	slight mod	2/13/80
   : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : */
/*
	Whatever compiler Mr. Morgan uses it is very non-K & R
	or maybe it's Unix V6 rather than V7 or system III or some
	thing like that...So I've taken the liberty of making it
	ci-c86-able and at least as of 3/20/84 this works for
	ascii files
			Matthew Weinstein

	Finis.... added binary file option...BEWARE USER
	this will open files as binary .... otherwise use
	the -a flag. --3/21/84

			Matthew Weinstein

*/

/* : : : : : : : : : : : : : : : : : : : : : : : : : : : : : :
	Macros for constant definitions
   : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : */
#define TRUE 1
#define FALSE 0
#define EOFF -1		/* end of file marker returned by getc() */
#define NOFILE 0	/* no such file indication given by fopen() */
#include "stdio.h"

/*	-------------------------------------------------------

	Name:		main(argc,argv)
	Result:		---
	Errors:		invocation syntax error or no such file
	Globals:	---
	Macros:		TRUE,FALSE,NOFILE
	Procedures:	fopen(),tolower(),bummer(),htoi(),fcompare()

	Action:		Byte by byte compare of 2 files and
			print their differences on console

	------------------------------------------------------- */


main(argc,argv)
	int argc;
	char *argv[];
{
	FILE *fopen(),*fd1,*fd2;
	int ascii;
	unsigned start_adrs,htoi();
	char mode[3], *ptr, *strcpy();

	strcpy(mode,"rb");
	ascii = FALSE;	/* assign the defaults */
	start_adrs = 0;
	  while( argc > 3 ) 
	 {
	    ptr = argv[--argc];
	    if( *ptr++ != '-' )
	      bummer();
	    switch ( tolower(*ptr++) ) 
	      {
	      case 'a':	ascii = TRUE;
			strcpy(mode,"r");
			break;

	      case 'b':	start_adrs = htoi(ptr);
			break;

	      default:	puts("Unrecognized option. Aborted\n\n");
			bummer();
	      }	/* end switch */
	    }	/* end while */
	if( argc < 3 || argc > 5 )
	  bummer();
	else if( (fd1 = fopen(argv[1],mode)) == NOFILE )
	  printf("No such file %s\n",argv[1]);
	else if( (fd2  = fopen(argv[2],mode)) == NOFILE)
	  printf("No such file %s\n",argv[2]);
	else  fcompare(fd1,fd2,start_adrs,ascii);
	exit();
	}


/*	-------------------------------------------------------

	Name:		fcompare(mfile,cfile,adrs,ascii)
	Result:		---
	Errors:		---
	Globals:	---
	Macros:		EOFF,EOF
	Procedures:	getc(),puts(),strcpy(),printf()

	Action:		compare 2 files and print their differences
			on the console

	------------------------------------------------------- */



fcompare(mfile,cfile,adrs,ascii)
	FILE *mfile;		/* the input file buffer */
	FILE *cfile;		/* the output file buffer */
	unsigned adrs;		/* the address of begin of file */
	int ascii;		/* flag of whether these are ascii files */
	{
	int mc,cc;		/* 1 char buffers */
	char erflg;		/* flag that an error has occurred */
	char *xl();		/* function to translate control chars */
	char str1[6],str2[6];	/* temporaries for strings */
	char xlate[10];	/* string used in ascii control char translation */


	erflg = 0;
	while( ! ( (mc = getc(mfile)) == EOFF || (ascii && mc == EOF)) ) {
	  if( (cc =getc(cfile)) == EOFF || ( ascii && cc == EOF )) {
	    puts("Checkfile shorter than Master file\n");
	    return;
	    }
	  else if( mc != cc ) {
	    if( ! erflg ) {
	      erflg = 1;
	      puts("\nRelative Master Check");
	      puts("\nAddress  File   File   Mismatch");
	      puts("\n-------  ----   ----   --------\n");
	      }	/* end if */
	    if( ascii )	{
	      strcpy(str1,xl(mc,xlate)); /* fudge because parameters are */
	      strcpy(str2,xl(cc,xlate)); /* evaluated before being passed */
	      printf("%4x     %-4s   %-4s   %8b\n",adrs,str1,str2,mc^cc);
	      }
	    else
	      printf("%4x     %2x     %2x     %8b\n",adrs,mc,cc,mc ^ cc);
	    }		/* end else if */
	  else
	    ;
	  adrs++;
	  }		/* end while */
	if(! ( (cc = getc(cfile)) == EOFF || (ascii && cc == EOF) ) )
	  puts("Masterfile shorter than checkfile\n");
	return;
	}		/* end fcompare() */

/*	-------------------------------------------------------

	Name:		err_exit(msg)
	Result:		---
	Errors:		---
	Globals:	---
	Macros:		---
	Procedures:	printf(),exit()

	Action:		Print a message then exit to CP/M 

	------------------------------------------------------- */


err_exit(msg)
	char *msg;
	{
	exit(puts(msg));
	}

/*	-------------------------------------------------------

	Name:		htoi(string)
	Result:		unsigned integer value of ascii hex string
	Errors:		---
	Globals:	---
	Macros:		---
	Procedures:	tolower(),isalpha(),isdigit()

	Action:		---

	------------------------------------------------------- */


unsigned htoi(string)
	char *string;
	{
	unsigned number;
	char c;

	number = 0;
	c = tolower(*string++);
	while( isalpha(c) || isdigit(c) ) {
	  if( c > 'f' )
	    return number;
	  number *= 16;
	  if( isdigit(c) )
	    number += c -'0';
	  else
	    number += c - 'a' + 10;
	  c = tolower(*string++);
	  }
	return number;
	}
/*	-------------------------------------------------------

	Name:		bummer()
	Result:		---
	Errors:		---
	Globals:	---
	Macros:		---
	Procedures:	puts(),exit()

	Action:		Print the invocation syntax error message
			and exit to CP/M

	------------------------------------------------------- */


bummer()
	{
	puts("Correct invocation form is:\n");
	puts(" FILECOMP <master file> <check file> {-a -b<hex-num>}\n\n");
	puts("Where optional arguments are:\n\n");
	puts("-a          => these are ascii files (terminate on 1AH )\n");
	puts("-b<hex-num> => begin of file is address <hex-num> ");
	puts("default is 0\n");
	exit();
	}


/*	-------------------------------------------------------

	Name:		xl(c)
	Result:		pointer to  xlate[]
	Errors:		---
	Globals:
	Macros:		---
	Procedures:	strcpy()

	Action:		Translate the char argument c into a
			4 char string in  xlate[]
			If c is a printable ascii char its
			  translation is itself right blank padded
			Else if c is a standard control char its
			  translation is a string identifying that
			  control char
			Else its translation is "????"

	------------------------------------------------------- */


char *xl(c,xlate)
	int c;
	char *xlate;
	{
	if( c > 0x7f || c < 0 )
	  strcpy(xlate,"????");
	else if( c == 0x7f )
	  strcpy(xlate,"del ");
	else if( c > 0x1f ) {	/* then it is printable */
	  xlate[0] = c;
	  strcpy(xlate+1,"   ");
	  }
	else
	  switch (c) {
	    case 0x7:	strcpy(xlate,"bel");
			break;

	    case 0x8:	strcpy(xlate,"bs");
			break;

	    case 0x9:	strcpy(xlate,"tab");
			break;

	    case 0xa:	strcpy(xlate,"lf");
			break;

	    case 0xc:	strcpy(xlate,"ff");
			break;

	    case 0xd:	strcpy(xlate,"cr");
			break;

	    case 0x1b:	strcpy(xlate,"esc");
			break;

	    default:	xlate[0] = '^';		/* show control chars as */
			xlate[1] = c + 0x40;	/* ^ <char> e.g. ^A is */
			xlate[2] = '\0';	/* control A */
			break;
	    }
	return xlate;
	}
                                                                                                                                