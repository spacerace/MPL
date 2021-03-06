/***********************************************************************
	SORTDIR.C
	(c) Rex Conn 12/10/84

SORTDIR does a physical sort of your disk directory.  It will perform
either a disk-based sort (for the root directory) or a file sort (for
subdirectories).  It requires DOS 2.0/2.1 to run, and should handle non-
standard media ok.  (I've tested it on a Bernoulli Box and on quad
density diskettes, but if you have strange media, test SORTDIR before
extensive use.)  SORTDIR was created using Computer Innovations C86.

SORTDIR will sort system files first, then hidden files, then the volume
name, then any subdirectory names, and finally the normal files, all in
straight ASCII sequence order.

To run, just type SORTDIR at the DOS prompt.  There are no options;
SORTDIR will only sort your current directory.

Good luck, and use at your own risk!

p.s.:  SORTDIR will NOT work with DOS 3.0 and DOS 3.1

************************************************************************/

#include "stdio.h"

/* structure for directory entries */
struct dnode {
	unsigned char att;
	unsigned char direntry[32];
} *dir[1024];

/* FCB structure */
struct filecontrol {
	unsigned char extended[7];
	unsigned char drive[1];
	unsigned char filename[11];
	int blocknum;
	int lrecsize;
	long filesize;
	int fdate;
	unsigned char reserved[10];
	unsigned char lrecnum;
	long recnum;
} fcb;

struct regval srv,rrv;
struct segments segregs;

unsigned char buf[80],*ptr,buffer[512],bdos(),*strrchr(),*malloc();

int	i,j,n,nextfree;
int	dirsector,numsectors,disk;

/* comparison routine for QSORT */
comp(a,b)
unsigned char **a,**b;
{
	return strcmp(*a,*b);
}

main()
{
	bdos(9,"Sortdir ver. 1.1\r\n$");
	buf[0] = '\\';
	segread(&segregs);
	srv.ds = segregs.sds;
	/* get current directory */
	srv.ah = 0x47;
	srv.dx = 0;
	srv.si = buf+1;
	sysint21(&srv,&rrv);
	/* if in root, sort disk */
	if (buf[1] == '\0') {
		/* determine type of disk to get directory */
		disk = bdos(0x19);
		srv.ah = 0x32;
		srv.dx = 0;
		sysint21(&srv,&rrv);
		dirsector = peek(rrv.bx+16,rrv.ds);
		i = peek(rrv.bx+9,rrv.ds);
		numsectors = (i % 32) ? (i / 32) + 1 : i / 32;
		/* read directory block from disk */
		nextfree = 0;
		for (i=dirsector;i<dirsector + numsectors;i++) {
			getsect(disk,buffer,0,i,1);
			for (n=0;n<512;n+=32) {
				if (buffer[n] == '\0') {
					i = dirsector + numsectors;
					break;
				}
				dir[nextfree] = malloc(sizeof(struct dnode));
				movmem(buffer+n,dir[nextfree]->direntry,32);
				subsort(nextfree);
				nextfree++;
			}
		}
		/* sort the block */
		qsort(dir,nextfree,PTRSIZE,comp);
		/* write it back to the disk */
		for (j=0,i=dirsector;i<dirsector + numsectors;i++) {
			for (n=0;n<512;n++)
				buffer[n] = '\0';
			for (n=0;n<512;n+=32) {
				movmem(dir[j]->direntry,buffer+n,32);
				if (++j >= nextfree)
					break;
			}
			getsect(disk,buffer,1,i,1);
			if (j >= nextfree)
				break;
		}
		srv.ah = 0x32;			/* force BPB read */
		srv.dx = 0;			/* default disk?? */
		sysint21(&srv,&rrv);
		pokeb(rrv.bx+23,rrv.ds,-1);
		sysint21(&srv,&rrv);
	} else {		/* else sort subdirectory file */
		srv.ah = 0x3B;		/* change directory */
		srv.dx = "..";
		sysint21(&srv,&rrv);
		fcb.extended[0] = 0xFF;
		fcb.extended[6] = 16;
		ptr = strrchr(buf,'\\') + 1;
		for (i=0;i<11;i++) {
			if ((*ptr == '.') && (i == 8))
				ptr++;
			if ((*ptr) && (*ptr != '.'))
				fcb.filename[i] = *ptr++;
			else
				fcb.filename[i] = ' ';
		}
		srv.ah = 0x0F;		/* open the file */
		srv.dx = &fcb;
		sysint21(&srv,&rrv);
		if (rrv.al == 0xFF)
			error("Can't open subdirectory\n$");
		fcb.lrecsize = 32;	/* directory entry size */
		fcb.lrecnum = 0;

		fcb.filesize = 32768;
		bdos(0x1A,buffer);
		nextfree = 0;
		srv.ah = 0x14;		/* sequential read */
		do {
			sysint21(&srv,&rrv);
			if (buffer[0] == '\0')	/* end of directory */
				break;
			dir[nextfree] = malloc(sizeof(struct dnode));
			movmem(buffer,dir[nextfree]->direntry,32);
			subsort(nextfree);
			nextfree++;
		} while (rrv.al == 0);
		qsort(dir,nextfree,PTRSIZE,comp);
		fcb.blocknum = 0;		/* reset block number */
		fcb.lrecnum = 0;
		for (i=0;i<nextfree;i++) {
			srv.ah = 0x15;		/* sequential write */
			bdos(0x1A,dir[i]->direntry);
			sysint21(&srv,&rrv);
		}
		srv.ah = 0x10;			/* close the subdirectory */
		srv.dx = &fcb;
		sysint21(&srv,&rrv);
		srv.ah = 0x3B;			/* change directory */
		srv.dx = buf;
		sysint21(&srv,&rrv);
	}
}

/* sort labels first, then hidden files, then subdirs, then everything else */
subsort(a)
int a;
{
	if (dir[a]->direntry[0] == '.')		/* root directory entry? */
		dir[a]->att = 1;
	else if (dir[a]->direntry[11] & 4)	/* system file */
		dir[a]->att = 2;
	else if (dir[a]->direntry[11] & 2)	/* hidden file? */
		dir[a]->att = 3;
	else if (dir[a]->direntry[11] & 8)	/* volume label? */
		dir[a]->att = 4;
	else if (dir[a]->direntry[11] & 16)	/* subdirectory? */
		dir[a]->att = 5;
	else
		dir[a]->att = 6;
}

/* print error message and abort */
error(s)
unsigned char *s;
{
	bdos(9,s);
	_exit(1);
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                            