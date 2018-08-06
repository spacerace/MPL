/*
** GENINDEX - Create index from text file.
** 
** Support Program for The Window BOSS
**
** Copyright (c) 1984,1985,1986 - Philip A. Mongelluzzo
** All rights reserved.
**
** Note: The GENINDEX.EXE file was created using the CI86 compiler.
**       If you attempt to recompile this program with either Lattice
**       or MSC you may have to adjust the logic to account for the way
**       the various compilers treat <CR><LF> sequences.  This usually
**       amounts to nothing more than chaning the "rb" to "r" in the
**       fopen statement.  
*/

#include <stdio.h>                      /* Standard stuff */

FILE *fopen();                          /* keep the compilers happy */
long ftell();

main(argc,argv)
int argc;
char *argv[];
{
FILE *fp;
FILE *fo;
char buf[132];
static char fname[132];
unsigned i;
long pos;
char *p;

  if(argc < 2) {                        /* check command line */
    printf("Usage: genindex <filename.ext>");
    exit(1);
  }

  p = argv[1];                          /* parse input filename */
  i = 0;
  while(*p) {
    fname[i++] = *p++;
    if(*p == ' ' || *p == '.') break;
  }
  strcat(fname, ".ndx");                /* create output filename */

  fp = fopen(argv[1], "rb");            /* open input */
  if(!fp) {
    printf("Open failed: %s", argv[1]);
    exit(1);
  }
  printf("Creating: %s\n", fname);      /* say whats going on */
  fo = fopen(fname, "w");               /* open output */

  while(fgets(&buf[0],132,fp)) {        /* read lines till eof */
    if(buf[0] == '%') {                 /* look for subject heading key */
      fputs(buf, fo);                   /* write to index file */
      fputs(buf, stdout);               /* display on console */
      pos = ftell(fp);                  /* get file position */
      printf("%ld\n", pos);             /* display on console */
      fprintf(fo, "%ld\n", pos);        /* write in output file */
    }
  }
  fclose(fp);                           /* close files */
  fclose(fo);
}

/* End */
