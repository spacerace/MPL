/* UNLUMP:  Break apart files lumped together by LUMP */

#include "stdio.h"

#define  BUFL  512

FILE *infile, *outfile;

main(argc,argv)
int argc;
char *argv[];
{
   int c, i;
   char filename[61];
   char buf[BUFL];
   int opened = 0;

   if(argc<2)  { usage(); exit(0); }

   argc--;
   if( (infile=fopen(argv[1], "r")) == NULL )     inerr();
   while( fgets(buf, BUFL, infile) != NULL )  {
      filename[0] = '\0';
      sscanf( buf, "*FILE: %60s", filename );
      if( filename[0] != '\0' )  {
         if(opened)  fclose(outfile);
         printf( "*FILE: %s\n", filename );
         if( (outfile=fopen(filename,"w")) == NULL )    outerr();
         opened = 1;
         continue;
         }
      if( !opened )  fmterr();
      if( fputs(buf, outfile) == EOF )  outerr();
      }
   if( ferror(infile) )  inerr();
   fclose(infile);
}


usage()
{
   printf("Usage:  unlump infile \n");
   printf("   UnLump the input file into output files named in it.\n");
   printf("   Files are separated by  a line with the format:\n");
   printf("      *FILE: filename.ext\n\n");
}


inerr()
{
   printf("\nError opening or reading input file \n");
   fclose(infile);
   exit(1);
}


outerr()
{
   printf("\nError opening or writing output file \n");
   fclose(outfile);
   exit(1);
}


fmterr()
{
   printf("\nFile does not have proper LUMP format\n");
   exit(1);
}
