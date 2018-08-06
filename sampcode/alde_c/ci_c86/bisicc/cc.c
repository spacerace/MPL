/*		cc.c		program to simulate (sortof) the cc command in Unix

		usage:	cc [flags] file ...
			flags:	all C86 flags are passed thru directly except -c.
				
				-c			compile only, don't link				  
				-lpath	path/name of new lib to add to lib list
				-C			nested comments (-c for compiler)
				-o file	name of output.exe, assumed first name in list
				-M file	name of map file (linker)
				-H 		/h option (linker)
				-D			/d option (linker)
				-L			/l option (linker)
				-P			/p option (linker)
				-S size	/s option (linker)
				-A			/a option (assembler)
				-B			/d option (assembler)
				-E			/e option (assembler)
				-N			/n option (assembler)
				-O			/o option (assembler)
				-R			/r option (assembler)
				-F			/X	option (assembler)
				-X name	name for CREF file (assembler)

				may have multiple files in list and they can contain
				wildcards.
*/

#include <stdio.h>
#include <ctype.h>

#define TRUE -1
#define FALSE 0
/* flags */
/* internal */
int link=TRUE;
char libs[255] = "\0";
/* compiler */
int cf1=FALSE;
int cf2=FALSE;
int cfa=FALSE;
int cfb=FALSE;
int cfc=FALSE;
char cfd[255];		
int cfe=FALSE;
int cfh=FALSE;
int cfi=FALSE;
int cfj=FALSE;
int cfn=FALSE;
int cfp=FALSE;
int cfs=FALSE;
int cft=FALSE;
int cfu=FALSE;
/* linker */
int lfh=FALSE;
int lfd=FALSE;
int lfl=FALSE;
int lfp=FALSE;
int lfs=FALSE;
/* assembler */
int afa=FALSE;
int afb=FALSE;
int afe=FALSE;
int afn=FALSE;
int afo=FALSE;
int afr=FALSE;
int aff=FALSE;

char outfile[255] = "\0";
char mapfile[255] = "\0";
char creffile[255] = "\0";

char biglib[255] = "/lib/c86b2s.lib";
char smllib[255] = "/lib/c86s2s.lib";
char include[255] = "/lib/";

char copts[255] = "\0";
char aopts[255] = "\0";
char lopts[255] = "\0";

main(ac,av)	
int ac;
char *av[];
{
	char *s;
	char *ts;
	int i,nd,j,nf;
	char *filelist[30];
	char tstr[255];
	char *first,*next;
	char *switchar();

	nd=0;
	while(--ac > 0 && (*++av)[0] == '-') {
		for (s = av[0]+1; *s != '\0'; s++) {
			switch (*s) {
			case '1':		/* 80186 code */
				cf1=TRUE;
				strcat(copts,"-1");
				break;
			case '2':		/* 80286 code */
				cf2=TRUE;
				strcat(copts,"-2");
				break;
			case 'a':		/* gen asm */
				cfa=TRUE;
				strcat(copts,"-a ");
				break;
			case 'A':		/* */
				afa=TRUE;
				strcat(aopts,switchar());
				strcat(aopts,"A");
				break;
			case 'b':		/* big model */
				cfb=TRUE;
				strcat(copts,"-b ");
				break;
			case 'B':		/* both passes */
				afb=TRUE;	/* gens a -D */
				strcat(aopts,switchar());
				strcat(aopts,"D");
				break;
			case 'c':		/* compile only */
				link = FALSE;
				break;
			case 'C':		/* nested comments */
				cfc=TRUE;
				strcpy(copts,"-c ");
				break;
			case 'd':		/* compiler define */
				ts = ++s;
				while(*++s != ' ');
				*s++ = '\0';
				strcpy(cfd,ts);
				strcat(copts,"-d ");
				strcat(copts,cfd);
				strcat(copts," ");
				break;		
			case 'D':		/* */
				lfd=TRUE;
				strcat(lopts,switchar());
				strcat(lopts,"D");
				break;
			case 'e':		/* */
				cfe = TRUE;
				strcat(copts,"-e ");
				break;
			case 'E':
				afe=TRUE;
				strcat(aopts,switchar());
				strcat(aopts,"E");
				break;
			case 'F':
				aff=TRUE;	/* to /x */
				strcat(aopts,switchar());
				strcat(aopts,"X");
				break;
			case 'h':
				ts=++s;
				cfh = TRUE;
				while(*++s != ' ');
				*s++ = '\0';
				strcat(copts,"-h");
				strcat(copts,ts);
				break;
			case 'H':
				lfh=TRUE;
				strcat(lopts,switchar());
				strcat(lopts,"H");
				break;
			case 'i':
				cfi=TRUE;
				strcat(copts,"-i ");
				break;
			case 'j':
				ts=++s;
				*++s = '\0';
				strcat(copts,"-j");
				strcat(copts,ts);
				strcat(copts," ");
				break;
			case 'l':
				ts=++s;
				while(*++s != ' ');
				*s++ = '\0';
				strcat(libs,ts);
				strcat(libs,"+");
				break;
			case 'L':
				lfl=TRUE;
				strcat(lopts,switchar());
				strcat("L");
				break;
			case 'M':
				ts=++s;
				while (*++s != ' ');
				*s++ = '\0';
				strcpy(mapfile,ts);
				break;
			case 'n':
				cfn=TRUE;
				strcat(copts,"-n ");
				break;
			case 'o':
				ts=++s;
				while (*++s != ' ');
				*s++ = '\0';
				strcpy(outfile,ts);
				break;
			case 'O':
				afo=TRUE;
				strcat(aopts,switchar());
				strcat(aopts,"O");
				break;
			case 'p':
				cfp=TRUE;
				strcat(copts,"-p ");
				break;
			case 'P':
				lfp=TRUE;
				strcat(lopts,switchar());
				strcat(lopts,"P");
				break;
			case 'R':
				afr=TRUE;
				strcat(aopts,switchar());
				strcat(aopts,"R");
				break;
			case 's':
				cfs=TRUE;
				strcat(copts,"-s ");
				break;
			case 't':
				cft=TRUE;
				strcat(copts,"-t ");
				break;
			case 'u':
				cfu=TRUE;
				strcat(copts,"-u ");
				break;
			case 'X':
				ts=++s;
				while (*++s != ' ');
				*s++ = '\0';
				strcpy(creffile,ts);
			}
		}
	}
	if (ac < 1) usage();					/* not enough args */

	readenv();								/* get environment info */

	if (!cfh) {								/* where to find includes */
		strcat(copts,"-h");				/* if they don't spec them */
		strcat(copts,include);
	}
	for (i=0;i<30;i++) 
		filelist[i] = alloc(128);		/* grab space for the filenames */

	/* parse off filenames */

	for (i=nf=0;i<ac;i++) {
		if (strchr(av[i],'*') || strchr(av[i],'?')) { 	/* any wildcards? */
			strcpy(tstr,av[i]);
			first = filedir(tstr,0);
			for(next=first;*next != '\0';) {
				strcpy(filelist[nf++],lower(next));
				next += strlen(next)+1;
			}
			free(first);
		} 
		else {
			strcpy(filelist[nf++],lower(av[i]));			/* no wildcards */
		}
	}

	/* now have a list of files to compile, assemble and/or link */
	/* test them for validity and compile them */
			
	for (i=0;i<nf;i++) {
		if (!(ts=strrchr(filelist[i],'.'))) {	/* no ext? */
			strcat(filelist[i],".c");				/* tack on the default */
		}
		if (strcmp(strrchr(filelist[i],'.'),".c")==0) {
			_cc(filelist[i]);							/* compile it */
		} 
		else if (strcmp(strrchr(filelist[i],'.'),".asm")==0)	{  
			_as(filelist[i]);							/* assemble it */
		}
		else if (strcmp(strrchr(filelist[i],'.'),".obj")!=0) {
			fputs(filelist[i],stderr);
			fubar("invalid file type ",filelist[i]);
		}
	}

	/* done compiling/assembling, time to link */

	if (link) {
		if (*outfile == '\0') {					/* no -o spec, */
			strcpy(outfile,filelist[0]);		/* assume 1st file in list */
			ts = strrchr(outfile,'.');
			*ts = '\0';			  					/* zap ext */
		}								
		_link(outfile,filelist,nf);
	}
	puts("\ndone.");
	exit(0);
}

char *switchar()
{
	struct regs {int ax,bx,cx,dx,si,di,ds,es; } r;
	char t[3];

	r.ax = 0x3700;
	r.dx = 0;
	sysint21(&r,&r);
	t[0] = r.dx&0xff;
	t[1] = '\0';
	return(t);
}

_cc(file)			/* compile file with current options */
char *file;
{
	static char *passes[] = {"cc1 ","cc2 ","cc3 ","cc4 ","\0"};
	int i,fr;
	char comlin[255];
	char ts[255];
		
	for (i=0;*passes[i] != '\0';i++) {
		strcpy(comlin,passes[i]);		/* get pass */
		if (i==0) {
			strcat(comlin,copts);			/* add options on pass 1 */
			strcat(comlin," ");
		}
		strcat(comlin,file);
		puts(comlin);
		fr=fork(comlin);
		if (fr) {
			fubar("error compiling: ",file);
		}
	}
}

_as(file)			/* assemble file with current options */
char *file;
{
	char comlin[255];
	char ts[255];
	int fr;

	strcpy(comlin,"masm ");
	strcat(comlin,file);
	strcat(comlin,",,");
	if (*creffile != '\0') strcat(comlin,creffile);
	else strcat(comlin,",");
	strcat(comlin,aopts);
	strcat(comlin,";");
	puts(comlin);
	if ((fr=fork(comlin)))	{
		fubar("error assembling ",file);
	}
}

_link(out,filelist,nf)		/* link the files in filelist into out */
char *out;
char *filelist[];
int nf;
{
	char comlin[255];
	int i,fr;
	char *ts;
		
	strcpy(comlin,"link ");
	for (i=0;i<nf;i++) {
		ts = strchr(filelist[i],'.');
		*ts = '\0';
		strcat(comlin,filelist[i]);
		if (i < nf-1)					/* don't want */
			strcat(comlin,"+");		/* plus on last file in list */
	}
	strcat(comlin,",");
	strcat(comlin,out);
	strcat(comlin,",");
	if (*mapfile != '\0') 
		strcat(comlin,mapfile);
	strcat(comlin,",");
	if (libs[0] != '\0')			/* any user libs? */
		strcat(comlin,libs);		/* add them */
	if (cfb) 						/* big model? */
		strcat(comlin,biglib);	/* add system library */
	else
		strcat(comlin,smllib);
	strcat(comlin,lopts);		/* add linker options */
	strcat(comlin,";");
	puts(comlin);
	fr=fork(comlin);
	if (fr) {
		fubar("error linking");
	}
}

readenv()			/* reads the environment variables */
{
	struct {int ax,bx,cx,dx,si,di,ds,es;} r;
	char *ep,*envfind();
	
	if (ep = envfind("CCBLIB")) {
		strcpy(biglib,ep);
		free(ep);
	}
	if (ep = envfind("CCSLIB")) {
		strcpy(smllib,ep);
		free(ep);
	}
	if (ep = envfind("CCINCLUDES")) {
		strcpy(include,ep);
		free(ep);
	}
}

fubar(s,s2)
char *s;
char *s2;
{
	fputs("cc: ",stderr);
	fputs(s,stderr);
	puts(s2);
	exit(1);
}

usage()
{
puts("	usage: cc [flags] file ...");
puts("	flags: all C86 flags are passed thru directly except -c.");
puts("	");
puts("	-c      compile only, don't link				  ");
puts("   -lpath  path/name of library to add to lib list");
puts("	-C      nested comments (-c for compiler)");
puts("	-ofile  name of output.exe, assumed first name in list");
puts("	-Mfile  name of map file (linker)");
puts("	-H      /h option (linker)");
puts("	-D      /d option (linker)");
puts("	-L      /l option (linker)");
puts("	-P      /p option (linker)");
puts("	-Ssize  /s option (linker)");
puts("	-A      /a option (assembler)");
puts("	-B      /d option (assembler)");
puts("	-E      /e option (assembler)");
puts("	-N      /n option (assembler)");
puts("	-O      /o option (assembler)");
puts("	-R      /r option (assembler)");
puts("	-F      /X option (assembler)");
puts("	-Xname  name for CREF file (assembler)");
puts("");
puts("	may have multiple files in list and they can contain");
puts("	wildcards.");
puts("");
exit(1);
}
