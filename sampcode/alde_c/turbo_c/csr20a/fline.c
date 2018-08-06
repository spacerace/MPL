/*
**  F I R S T   L I N E
**
**  A Structured Programming Tool.
**
**  Copyright 1986 Bob Pritchett
**
**  Created: 03/02/86  Last Updated: 03/02/86
**
*/

#include <stdio.h>
#include <ctype.h>

FILE *in, *out;

main(argc,argv)
 int argc;
 char *argv[];
 {
  char *line;
  if ( argc != 3 )
     help();
  strcpy(argv[1],strupr(argv[1]));
  strcpy(argv[2],strupr(argv[2]));
  if ( strcmp(argv[1],argv[2]) == 0 )
   {
    printf("FLINE: Source and Output File Must be Different Files.\n");
    exit(0);
   }
  if ( ( in = fopen(argv[1],"r") ) == 00 )
   {
    printf("FLINE: Unable to open source file.\n");
    exit(0);
   }
  if ( ( out = fopen(argv[2],"w") ) == 00 )
   {
    printf("FLINE: Unable to open output file.\n");
    exit(0);
   }
  fprintf(out,"FLINE Output File for %s.\n\n",argv[1]);
  while ( fgets(line,100,in) )
   {
    if ( ! isspace(*line) )
       fprintf(out,"%s",line);
   }
  fclose(out);
  fclose(in);
  printf("FLINE: Successful.\n");
  exit(0);
 }

help()
 {
  printf("\n    First Line is a utility for programmers who write formatted\n");
  printf(" source code.  FLine goes through a source file and puts all\n");
  printf(" lines beginning with a non-whitespace character into the\n");
  printf(" output file.  This creates and easy reference file with the\n");
  printf(" name and argument list for all functions contained in the\n");
  printf(" source in addition to all global variables and include files.\n");
  printf("    Call FLine like:\n\n");
  printf("    FLINE <source> <output>\n\n");
  printf("  Version 1.1  Copyright 1986 Bob Pritchett\n");
  printf("           New Dimension Software\n");
  exit(0);
 }
