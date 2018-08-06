/* Christmas, 1985 - Bob Pritchett */
/*   Modify and distribute as you will, just leave the credits. */
/*   ...My small gift to the PD world... */

/* NOTE: This source code is NO LONGER in the Public Domain.  I have */
/* left the above message and the other comments to give a history of */
/* the program.  It is currently a part of the C Spot Run C Add-On Library. */

/* Tiny bug and multiple files added the day after... */

/* 01/09/1986 RDP Fixed a bug, (yes, another...) and added error tally. */
/* 02/07/1986 RDP Going for effiecient analysis... */
/* 02/08/1986 RDP More analysis... */
/* 06/15/1986 RDP More work on 2.0.... */
/* 07/16/1986 RDP Continuing on 2.0, hopefully near completion. */
/*                Version 2.1 now.  CheckC.ANL eliminated. */
/* 07/17/1986 RDP More accurate location of some errors. */
/* 07/19/1986 RDP Can trace a new error with the ; check. */
/* 08/05/1986 RDP Attempting to fix ; error.... */
/* 09/16/1986 RDP Oh, what the heck.  Let's add comment beginning variable. */
/*                (This tells what line starts 'EOF with comment open.') */

/*


        Simply put, this program will check your C code's syntax in just
        about every concievable way that does not involve CheckC to track
        variable names or any other identifiers.  If you really care, read
        CHECKC.DOC.  Informal testing has shown that it can narrow down
        almost any syntactical error to within 3 lines, and it supports full
        error recovery.  (When I say syntax errors, I mean {} () [] "" ''
        comments, etc.)  The program follows K&R for syntax, so those of you
        with compilers that allow nested comments, watch out.  (They are
        illegal in K&R, and thus here.)

        This program has grown from a simple 'curly brace checker' into 
        what I feel is a powerful and useful tool for C programmers.  I
        hope you agree, and that it proves to be a useful tool for you.

          Bob Pritchett
          New Dimension Software
            23 Pawtucket Dr.
            Cherry Hill, NJ 08003
            Voice: (609) 424-2595
             Data: (609) 354-9259  (300B/1200B)  (24hrs.)
 
*/

#include <stdio.h> 
#include <ctype.h>	/* For the isalpha() macro */

FILE *data;		/* Input File */

int in_comment;		/* Flag - Positive if in Comments */
int in_single;		/* Flag - Positive if in Single Quotes */
int in_double;		/* Flag - Positive if in Double Quotes */

int paren;		/* Temporary Paren Count */
int tparen;		/* Total (Per File) Paren Count */

int brack;		/* Bracket Count */

int square;		/* Temporary Square Bracket Count */
int tsquare;		/* Total (Per File) Square Bracket Count */

int errors;		/* Error Count (Per File) */

int line;		/* Current Line Number */

int anal;		/* Analysis Flag - Positive if Analysis On */

int tbrack;		/* Extra Bracket Count */

int semi;		/* Semicolon Counter for for Statements */

int cbl;		/* Line on Which Last Comment Began */

main(argc,argv)
 int argc;
 char *argv[];
 {
  int x = 1;		/* First Argument that is a File - 1 is Default */
  int c;		/* Current Character */
  int lastc;		/* Last Character */
  int llast;		/* Character Before lastc */
  int vlast;		/* Character Before llast */
  int forflag;		/* Flag to mark for statement */
  int err_flg = 0;	/* Total errors in all files. */
  printf("\n  CheckC Version 2.1   Copyright 1985, 86 Bob Pritchett\n");
  printf("                 A small C code checker.\n");
  if ( argc < 2 )	/* if no arguments */
   {
    printf("\nCHECKC: Usage is CHECKC [/A] <filename> [<filename>...]\n");
    exit(1);		/* Exit with failure flag */
   }
  if ( argv[1][0] == '/' && ( argv[1][1] == 'A' || argv[1][1] == 'a' ) )
   {
    x = 2;		/* First Argument is an option */
    anal = 1;		/* Analysis on */
   }
  for ( ; x < argc; ++x )	/* For Each File... */
   {
    in_comment = 0;	/* Initialize Data */
    in_single = 0;
    in_double = 0;
    paren = 0;
    tparen = 0;
    brack = 0;
    square = 0;
    tsquare = 0;
    errors = 0;
    line = 1;
    tbrack = 0;
    semi = 0;
    forflag = 0;
    cbl = 0;
    c = 0;
    lastc = 0;
    llast = 0;
    vlast = 0;

    strcpy(argv[x],strupr(argv[x]));	/* Put file name in upper case */
    if ( ( data = fopen(argv[x],"r") ) == 00 )	/* If unable to open source */
     {
      printf("\nCHECKC: Error Opening %s\n",argv[x]);	/* Print message */
      continue;				/* and continue with next file */
     }
    if ( anal )
       printf("\n        Checking and Analyzing %s\n\n",argv[x]);
    else
       printf("\n        Checking %s\n\n",argv[x]);
    while ( c != EOF )
     {
      vlast = llast;
      llast = lastc;
      lastc = c;
      c = getc(data);	/* Get character from data file */

      if ( c == '\n' )	/* Increment here so comment lines are counted. */
       {
        if ( in_single )
         {
          printf("Line %04d:  Missing single quote.  Diagnostics now inaccurate.\n",line);
          in_single = 0;			/* Fix flag for diags. */
          ++errors;
         }
        else if ( in_double )
         {
          printf("Line %04d:  Missing double quote.  Diagnostics now inaccurate.\n",line);
          in_double = 0;			/* Fix flag for diags. */
          ++errors;
         }
        ++line;
        continue;
       }

      if ( in_double )		/* If in double quotes, check for end quote */
       {                                           /* It is an ending " if  */
        if ( ( c == '"' && lastc == '\\' && llast == '\\' ) ||  /*  \\"  or */
             ( c == '"' && lastc != '\\' ) )                    /*  !  \"   */ 
           in_double = 0;	/* If so, turn off quote flag */
        continue;		/* and continue with next character */
       }

      if ( in_comment )		/* If in a Comment */
       {
        if ( c == '/' && lastc == '*' )	/* Check for end comment sign */
           in_comment = 0;		/* If comment ends, change flag */
        continue;			/* and get next character */
       }

      if ( in_single )		/* If in single quotes */
       {			/* check for closing single quote */
        if ( ( c == 39 && lastc == '\\' && llast == '\\' ) ||   /* 39 is ' */
             ( c == 39 && lastc != '\\' ) )
           in_single = 0;                 
        continue;                         
       }

      switch ( c )	/* If not in a comment or quote */
       {		/* check and count all relevant characters */
        case '(':
          ++paren;
          ++tparen;
          break;
        case ')':
          --paren;
          --tparen;
          break;
        case '{':
          ++brack;
          ++tbrack;
          break;
        case '}':
          --brack;
          --tbrack;
          break;
        case '[':
          ++square;
          ++tsquare;
          break;
        case ']':
          --square;
          --tsquare;
          break;
        case '"':
          in_double = 1;
          break;
        case 39:
          in_single = 1;
          break;
        case '*':	/* Must be last so break w/o correct if is ok. */
          if ( lastc == '/' )
           {
            in_comment = 1;
            cbl = line;	/* Latest comment started here. */
           }
          break;
        default:
          break;
       }

      if ( anal )	/* If in analysis mode, attempt to diagnose */
       {		/* problem by the level of brackets etc. and */
        if ( c == '{' )	/* call appropriate error. */
         {
          if ( brack == 1 )
           {
            if ( paren > 0 )
               error(1);
            if ( square > 0 )
               error(7);
           } 
          else if ( brack > 1 )
           {
            if ( paren > 0 )
               error(3);
            if ( square > 0 )
               error(9);
           }
         }
        else if ( c == '}' )
         {
          if ( brack == 0 )
           {
            if ( paren > 0 )
               error(2);
            if ( square > 0 )
               error(8);
           }
          else if ( brack > 0 )
           {
            if ( paren > 0 )
               error(4);
            if ( square > 0 )
               error(10);
           } 
          else
             error(13);
         }

        if ( c == '(' && forflag == 1 )
         {
          semi = 1;		/* Must wait till for statement is begun */
          forflag = 0;		/* before allowing ; counting in statement */
         }

        if ( square < 0 )	/* Never allowed to go under */
           error(11);
        else if ( square > 0 && c == ';' )  /* Not even in for statement */
           error(12);

        if ( c == 'r' && lastc == 'o' && llast == 'f' )
         {
          if ( isalpha(vlast) || vlast == '_' )	/* If it's an identifier */
             break;		/* ..stop here */
          c = getc(data);	/* Peek at next character */
          if ( ! isalpha(c) && c != '_' )  	/* If it's not an identifier */
             forflag = 1;	/* ...it's a for statment */
          ungetc(c,data);	/* Put it back to be processed */
          c = 'r';		/* Put the correct letter in c again */
         }
        else if ( c == ';' && semi > 0 )	/* If counting has started */
           ++semi;		/* and there is a ;, record it */

        if ( paren < 0 )	/* Never allowed to go under */
           error(5);
        else if ( paren > 0 && semi == 0 && c == ';' )
           error(6);
        else if ( paren == 0 )
         {
          if ( semi > 3 )
             error(14);
          else if ( semi == 1 || semi == 2 )
             error(15);
          semi = 0;
         }
 
       }		/* Close of Analysis Stage */

     }			/* Close of While Loop */

    putchar('\n');

    fclose(data);	/* Close source file */

    if ( tparen > 0 )
       printf("CHECKC: %d too many ('s.\n",tparen);
    else if ( tparen < 0 )
       printf("CHECKC: %d too many )'s.\n",abs(tparen));

    if ( tbrack > 0 )
       printf("CHECKC: %d extra }'s or missing {'s.\n",tbrack);
    else if ( tbrack < 0 )
       printf("CHECKC: %d too many {'s.\n",abs(tbrack));

    if ( tsquare > 0 )
       printf("CHECKC: %d too many ['s.\n",tsquare);
    else if ( tsquare < 0 )
       printf("CHECKC: %d too many ]'s.\n",abs(tsquare));

    if ( in_double == 1 )
       printf("CHECKC: EOF with double quote open.\n");

    if ( in_comment == 1 )
       printf("CHECKC: EOF with comment open.  Comment opened on line %04d.\n"
              ,cbl);

    if ( in_single == 1 )
       printf("CHECKC: EOF with single quote open.\n");
 
    errors += in_single + in_comment + in_double + 	/* Total all errors */
              abs( tsquare ) +
              abs( brack ) +
              abs( tparen ); 

    err_flg += errors;		/* Flag positive if any files have errors. */

    printf("\n        %d Errors found in %s\n",errors,argv[x]);

    if ( anal )
       printf("        Check and Analysis of %s Completed\n",argv[x]);
    else 
       printf("        Check of %s Completed\n",argv[x]);

   }
  if ( err_flg != 0 )	/* If errors found in any file */
     exit(1);		/* Exit with 1 in ERRORLEVEL. */
  exit(0);		/* Otherwise exit cleanly. */
 }

/* ----------------------------------------------------------- */

error(x)
 int x;
 {
  printf("Line %04d:  ",line);	/* Line on which error first noticed */
  if ( x == 1 )
     printf("New function started with %d ('s open.",paren);
  else if ( x == 7 )
     printf("New function started with %d ['s open.",square);
  else if ( x == 2 )
     printf("Function ended with %d ('s open.",paren);
  else if ( x == 8 )
     printf("Function ended with %d ['s open.",square);
  else if ( x == 3 )
     printf("Block started with %d ('s open.",paren);
  else if ( x == 9 )
     printf("Block started with %d ['s open.",square);
  else if ( x == 4 )
     printf("Block ended with %d ('s open.",paren);
  else if ( x == 10 )
     printf("Block ended with %d ['s open.",square);
  else if ( x == 5 )
     printf("%d extra )'s or missing ('s.",abs(paren));
  else if ( x == 6 )
     printf("Semicolon with %d extra ('s or missing )'s.",paren);
  else if ( x == 11 )
     printf("%d extra ]'s or missing ['s.",abs(square));
  else if ( x == 12 )
     printf("Semicolon with %d extra ['s or missing ]'s.",square);
  else if ( x == 13 )	/* A bad { } count can ruin the analysis accuracy */
   {
    printf("An extra } or missing {.");
    brack = 0;
   }
  else if ( x == 14 )
     printf("A for statement with %d extra ;'s.",semi-2);
  else if ( x == 15 )
     printf("A for statement without two ;'s.");
  if ( x > 0 && x < 7 )
     paren = 0;
  else if ( x > 6 && x < 13 )
     square = 0;
  putchar('\n');
 }
