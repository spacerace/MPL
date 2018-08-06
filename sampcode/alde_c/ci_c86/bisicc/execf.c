/*		execf.c		execute another program and return status */

#include <stdio.h>

struct {unsigned int ax,bx,cx,dx,si,di,ds,es;} r;
unsigned char cline[128],*ep,*envfind();
int status=-1;

#define NUMINTS 30
static char *internals[] = {"copy","ren","rename","del","erase","chdir","cd",\
"md","mkdir","dir","rd","rmdir","type","cls","ctty","date","time",\
"path","prompt","set","ver","verify","vol","echo","rem","goto","if",\
"shift","pause","for"};

fork(cmdl)				/* parses off name and cmdl for execf */
char *cmdl;
{
	char *sp;
	char *name;
	char comlin[128];
	int er;

	sp = strcpy(name,cmdl);
	strcat(name," ");					/* pad for find */
	while(*++sp != ' ');				/* find 1st space */
	*sp++ = '\0';						/* term name str */
	strcpy(comlin,sp);				/* get command line */
	er= execf(name,comlin);
	return(er);
}

execf(name,cmdl)		/* executes name with command line cmdl */
char *name;
char *cmdl;
{
	int i,er;
	char tp[255];
	char *ts;
	char *path;
	char *findexecf();
	char lcmdl[255];		/* local command line */
	
	lcmdl[0] = ' ';			/* tack on a space at the beginning */
	strcpy(lcmdl+1,cmdl);	/* to keep DOS happy */
	name = lower(name);		/* convert to lowercase for search */
	for (i=0;i<NUMINTS;i++) {
		if (!strcmp(name,internals[i]))	/* check for an internal command */
			return (system(strcat(name,lcmdl)));	/* use c86 system() if internal */
	}
	/* it's not an internal command, process it */
	
	path = alloc(255);
	curdrv(path);			/* get current drive */
	getpath(path);		/* and path */
	ep = envfind("PATH");
	if (*ep != ';') strcat(path,";");	/* if none at beginning already */
	strcat(path,ep);							/* all one path string */
	free(ep);
	while (*path) {							/* until we hit EOS */
		strcpy(tp,path);
		i = 0;
		while (*path++ != ';') i++;		/* find end of path */
		tp[i] = '\0';							/* term path string */
		if (tp[i-2] != '\\' || tp[i-2] != '/') strcat(tp,"/"); 
		if ((ts=findexecf(tp,name))) {
			er = _execf(ts,lcmdl);
			if (!er) {
				r.ax = 0x4d00;
				sysint21(&r,&r);
				er = r.ax&0xff;
			}
			return(er);
		}
	}
	return(-1);								/* command not found, ret err */
}

char *findexecf(path,name)				/* tries to find an execable name along path */
char *path,*name;
{
	char tn[255];
	char *first,*next;
	static char *exet[] = {".COM",".EXE",".BAT"};
	int i;

	name = upper(name);
	strcpy(tn,path);
	strcat(tn,name);
	strcat(tn,".*");			/* add wildcard to find all */
	first = filedir(tn,0);	/* look for all normal files */
	strcpy(tn,name);
	for (next=first;*next != '\0';) {
		for (i=0;i<3;i++) {
			if (!(strcmp(strcat(tn,exet[i]),next))) return(strcat(path,tn));
			strcpy(tn,name);		/* nope, try again */
		}
		next += strlen(next)+1;		/* look at next name in list */
	}
	return(NULL);						/* didn't find one */
}

_execf(path,cmdl)				/* execs path(incl. name.ext) with cmdl */
char *path,*cmdl;
{
	int er;
	struct {
		int env_seg;
		char *line_off,*line_seg;
		char *fcb1_off,*fcb1_seg;
		char *fcb2_off,*fcb2_seg;
	} ctrl;
	
	cline[0] = strlen(cmdl);		/* put in length */
	strcpy(cline+1,cmdl);			/* copy in command line */
	strcat(cline,"\r");				/* add a CR */
	segread(&r.si);					/* get current cs */
	ctrl.env_seg=0;					/* no environment to pass */
	ctrl.line_off=cline;				/* offset of command line */
	ctrl.line_seg=r.ds;				/* segment of command line */
	er = loadexec(path,r.ds,&ctrl,r.ds,0);	/* launch! */
	return(er);
}

/* curdrv - get current default drive */

curdrv(sp)
char *sp;
{
	struct { int ax, bx, cx;
   	      char *dx, *si, *di, *ds, *es;
      	 } r;

	r.ax = 0x1900;							/* DOS interrupt 19 */
	sysint(0x21, &r, &r);				/* gets current drive number */
	*sp++ = r.ax + 'a';					/* convert to symbolic drive name */
	*sp++ = ':';
	*sp++ = '\0';
	sp--;
	return;
}

/* getpath - get path to directory on indicated drive */

getpath(sp)
char *sp;
{
	struct { int ax, bx, cx, dx;
   	      char *si;
      	   int di, ds, es;
	       } r;
	extern int _showds();

	strcat(sp, "/");				/* append root file symbol to drive name */
	
	r.ax = 0x4700;					/* DOS interrupt 47 gets path string */
	r.dx = *sp - '`';				/* convert drive name to index */
	r.ds = _showds();
	r.si = sp + 3;					/* paste string after root symbol */
	sysint(0x21, &r, &r);
	return;
}

_showds()
{
	struct{int cs, ss, ds, es;} r;

	segread(&r);
	return(r.ds);
}
