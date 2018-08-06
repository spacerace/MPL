/* lump together a group of files. (Use UNLUMP to break apart */

#include "stdio.h"

FILE *infile, *outfile;

main(argc,argv)
int argc;
char *argv[];
{
   int c, i;

   if(argc<3)  { usage(); exit(0); }

   argc--;
   if( exists(argv[1]) )    { usage(); exit(1);  }
   if( (outfile=fopen(argv[1], "w")) == NULL )    { usage();  exit(1); }
   for( i=2; i<=argc; i++ ) {
      if( (infile=fopen(argv[i],"r")) == NULL )  { inerr(argv[i]); exit(2); }
      fprintf(outfile, "*FILE: %s\n", argv[i]);
      printf("*FILE: %s\n", argv[i]);
      while ( (c=getc(infile)) >= 0)
         putc(c,outfile);
      fclose(infile);
   }

}


usage()
{
   printf("Usage:  lump outfile infile1 ... \n");
   printf("   Lump all input files into the output file,\n");
   printf("   each being preceeded with a line identifying itself:\n");
   printf("      *FILE: filename.ext\n\n");
}


inerr(s)
char *s;
{
   printf("\nError opening file %s\n", s);
}


exists(s)
char *s;
{
   FILE *f;
   if( (f=fopen(s,"r")) == NULL )  return 0;
   fclose(f);
   return 1;
}
