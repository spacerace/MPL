
/* Qlib.c
   Property of Borland International.
   Creates files readable by the Borland linker TLINK.
   Written by Roger Schlafly. (Compuserve: 76067,511)
   Compile in Turbo C with
     tcc -mc -w -p -O -Z -M -Id:\c -Ld:\c qlib
   (assuming Turbo C is in the directory d:\c.)

This programs creates .LIB files that work with TLINK.
It can be used as a primitive substitute for the librarians
sold by Microsoft, Phoenix, Lattice.

The Microsoft .LIB format is not documented and is not known
to Borland at this time.  The Borland linker TLINK accepts
files created with the Microsoft librarian, but it is actually
quite simple minded in what it accepts to be a .LIB file.

.LIB files created by QLIB work with TLINK, but not with the
Microsoft linker.  QLIB does have any of the various options for
modifying an existing .LIB file; it will only create one from
scratch.  QLIB is, however, faster than the Microsoft librarian.

Borland may someday have a librarian which supports the Microsoft
format.  If and when that happens, TLINK may no longer support
the format that QLIB uses.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#define Static static

Static FILE *rspf = 0;
Static FILE *libf = 0;
Static FILE *fp = 0;
Static char name[81];

#define TAB \011

char delimiters[] = " \n\011,;\r";
#define iswhite(x)	(strchr(delimiters,(x)) != 0)

int gettoken(void)
/* return 1 on success
   put token in 'name'
*/
{	int i, c;
	for (i=0; i<80; ++i)
	{	c = getc(rspf);
		if (c == -1) if (i == 0) return 0; else break;
		if (i == 0 && iswhite(c)) { i = -1; continue;}
		name[i] = (char) c;
		if (iswhite(c)) break;}
	name[i] = 0;
	return 1;}

char *libname;

void fatal(char *s)
{
	puts(s);
	/* don't leave partially written .LIB file on disk */
	if (libf != NULL) fclose(libf);
	if (libname != NULL) unlink(libname);
	exit(1);
}

Static int cdecl handler(void)
/* control-break handler
*/
{
  fatal("Interrupted.");
  return 0;}

void iofail(char *s)
{
	printf("%s",s);
	fatal(": disk error");
}

char *append(char *s, char *t)
{
	int n;
	char *p;
	n = strlen(s) + strlen(t) + 1;
	p = malloc(n);
	if (p == NULL) fatal("insufficient memory");
	strcpy(p,s);
	strcat(p,t);
	return p;
}

char *buf;
#define MAX (unsigned) 30000

unsigned char begin[] = { 0xA4, 1, 0, 0, };
unsigned char end[]   = { 0xA6, 1, 0, 0, };

int n;
char *fname;
int argi;

cdecl main(argc,argv)
char *argv[];
{
	if (argc < 3)
	{
		puts("Borland quick librarian, version 0.20.");
		puts("Usage: QLIB  libname  objfiles");
		puts("where objfiles is a series of OBJ file names,");
		puts("or is @name, where name is an ascii file of OBJ file names.");
		puts("where objfiles is an ascii file of OBJ file names.");
		puts(".LIB and .OBJ extensions added automatically.");
		puts("QLIB creates a .LIB consisting of those .OBJ files.");
		exit(0);
	}

	ctrlbrk(handler);
	libname = append(argv[1],".LIB");
	libf = fopen(libname,"wb");
	if (libf == NULL) printf("%s",libname), fatal(": cannot open");
	printf("Creating %s\n",libname);
	if (argv[2][0] == '@')
	{
		rspf = fopen(argv[2]+1,"r");
		if (rspf == NULL)
			printf("%s",argv[2]), fatal(": cannot open");
	}
	argi = rspf == NULL ? 2 : 0;

	buf = malloc(MAX);
	if (buf == NULL) fatal("insufficient memory");

	fwrite(begin,1,4,libf);

while (1)
{
	if (argi)
	{
		if (argi >= argc) break;
		strcpy(name,argv[argi++]);
	}
	else if (!gettoken()) break;

	fname = append(name,".OBJ");
	fp = fopen(fname,"rb");
	if (fp == NULL) iofail(fname);
#ifdef	Portable
	if (fseek(fp,0L,2)) iofail(fname);
	n = ftell(fp);
	if (fseek(fp,0L,0)) iofail(fname);
	n = fread(buf,1,MAX,fp);
#else	/* TURBO C */
	/* manual says n = 0 if at EOF, but don't believe it. */
	n = _read(fileno(fp),buf,MAX);
#endif
	if (n == MAX) fatal("OBJ file too large");
	if (fclose(fp)) iofail(fname);
	printf("Adding %s, %d bytes.\n",fname,n);
	fwrite(buf,1,n,libf);
}
	fwrite(end,1,4,libf);
	if (fclose(libf)) iofail(libname);
	exit(0);
}
