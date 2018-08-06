#include <stdio.h>
#include <ctype.h>

char *sbrk();
char *getmem();

/*
	T. Jennings 13 Jan 84


	LU Library Utility for all MSDOS and PC DOS machines. Can be used
to pack multiple files into a single LBR file for modem transfers, extract,
delete or update files within an LBR file, and reorganize one.

	NOTE: Reorganizing is desireable after deleting, adding, or updating
a file in the library. This due to the way LBR files are organized; deleted
files dont remove the data, and new files are added to the end. The LBR will
become much more compact after a Reorganization.

*/

#define ACTIVE 0	/* active, non-deleted file, */
#define UNUSED 0xff	/* never used slot, */
#define DELETED 0xfe	/* deleted file slot, */

#define MAXNAMES 20	/* maximum number of names in command line, */
#define MAXFILES 256	/* max files per LBR file, */

char names[30][MAXNAMES];/* list of names from command line, */
int num_names;		/* number of names in above list, */

/* Fix some annoyances in the Lattice Library. */

long lseek();			/* not an annoyance, */
#define tell(f) lseek(f,0L,1)	/* wierd */
#define seek lseek		/* wrong name, */

/* This is the structure of the LBR file directory. One per slot. 
THIS MUST BE COMPILED ON LATTICE WITH THE -B OPTION!!!! */

struct _ludir {
	char stat;	/* file status, */
	char name[11];	/* FCB type filename, */
	unsigned off;	/* offset to data, (sectors) */
	unsigned len;	/* length (sectors) of data, */
	int fill[8];	/* fill out to 32 bytes */
} ludir; /* (dummy ludir so we can sizeof() it) */

struct _ludir ldir[MAXFILES];
int num_slots;		/* number of slots in LBR file, */

char lbrname[14];	/* LBR file name, */
int lbr;		/* LBR file handle, */
char *buf;		/* buffer from DOS, */
int bufsize;		/* size of buffer, */

main(argc,argv)
int argc;
char *argv[];
{
int i;
char command;

	++argv; --argc;
	if (argc < 2) {

		printf("LU -- T. Jennings 15 Jan 84\n");
		printf("  LU <cmd> lbrfile file file file ...   Where <cmd> is:\n");
		printf("\tT List of files in LBR file,\n");
		printf("\tL Same as T,\n");
		printf("\tA Extract all files from the LBR file,\n");
		printf("\tE Extract file(s) from LBR file,\n");
		printf("\tU Add to or create LBR file,\n");
		printf("\tD Delete a file from LBR file,\n");
		printf("\tR Reorganize the LBR file.\n");
		exit();
	}
	command= *argv[0];		/* get command char, */
	strcpy(lbrname,argv[1]);	/* blindly get LBR name, */
	for (i= 0; i < argc - 2; i++) {
		strcpy(names[i],argv[i + 2]);
		stoupper(names[i]);
	}
	num_names= i;

	allmem();			/* find largest area to allocate */
	bufsize= sizmem() - (MAXFILES * sizeof(ludir)) - 2000;
	if (bufsize < 512) {
		printf("Not enough memory\n");
		exit();
	}
	buf = getmem(bufsize);

	switch (tolower(command)) {
		case 'a':
			unpack();
			break;
		case 't':
		case 'l':
			list();
			break;
		case 'e':
			extract();
			break;
		case 'u':
			update();
			break;
		case 'd':
			delete();
			break;
		case 'r':
			reorg();
			break;
		default:
			printf("Not a command.\n");
			break;
	}
}
/* Update: add or change a file. If the library file does not exist, then
create it and ask for the number of slots. Delete any file of the same
name first, then add it in. */

update() {

int i;

	if (get_directory() == -1) {		/* open or create, */
		if (new_directory() == -1) {
			printf("Error: cant create new directory\n");
			return;			/* error! */
		}
	}
	for (i= 0; i < num_names; i++) {	/* add each file, */
		if (find(names[i]) != -1) printf("Updating existing file %s\n",names[i]);
		else printf("Adding new file %s\n",names[i]);
		kill(names[i]);			/* delete first, */
		if (add_file(names[i]) == 0)	/* add in, if error, */
			break;
	}
	put_directory();			/* close LBR file, */
}
/* Delete all the specified names from the directory. */

delete() {

int i;
	if (get_directory() == -1) {
		printf("Error: LBR file not found\n");
		return;
	}
	for (i= 0; i < num_names; i++) {
		if (find(names[i]) != -1) 
			printf("Deleting %s\n",names[i]);
		else printf("%s not in library\n",names[i]);
		kill(names[i]);
	}
	put_directory();
}

/* List all the files in the LBR file. */

list() {

int i;
long size;
char name[14];
int active,deleted,empty;

	size= 0L; active= deleted= empty= 0;

	if (get_directory() == -1) {
		printf("Error: LBR file not found\n");
		return;
	}
	for (i= 0; i < num_slots; i++) {
		if (ldir[i].stat == ACTIVE) {
			cvt_from_fcb(ldir[i].name,name);
			printf("%-16s %8lu %8u\n",name,
			  (long)ldir[i].len * 128L,ldir[i].off);
			size+= (long)ldir[i].len * 128L;
			++active;
		}
		if (ldir[i].stat == DELETED) ++deleted;
		if (ldir[i].stat == UNUSED) ++empty;
	}
	printf("%-16s %8lu total bytes\n","",size);
	printf("%u active files, %u deleted files, %u empty slots.\n",
		active,deleted,empty);
}

/* Unpack all files from the library. */

unpack() {

int i;
int slot;
long pos,size;
unsigned count;
int file;
char name[14];

	if (get_directory() == -1) {
		printf("Error: LBR file not found\n");
		return;
	}
	for (slot= 1; slot < num_slots; slot++) {
		if (ldir[slot].stat == ACTIVE) {
			cvt_from_fcb(ldir[slot].name,name);
			printf("Extracting %s\n",name);
			file= creat(name,0x8001);	/* make new file, */
			if (file == -1) {		/* check error, */
				printf("Error: cant create %s\n",name);
				break;
			}
			pos= (long)ldir[slot].off * 128L;	/* find offset, */
			size= (long)ldir[slot].len * 128L;	/* and file size, */
			seek(lbr,pos,0);		/* go there, */
			while (size > 0L) {		/* copy out data, */
				count= (size > bufsize ? bufsize : size);
				read(lbr,buf,count);	/* read data, */
				write(file,buf,count);	/* write it, */
				size-= count;		/* update count, */
			}
			close(file);
		}
	}
	close(lbr);
}
/* Extract all specified files. */

extract() {

int i;
int slot;
long pos,size;
unsigned count;
int file;

	if (get_directory() == -1) {
		printf("Error: LBR file not found\n");
		return;
	}
	for (i= 0; i < num_names; i++) {
		slot= find(names[i]);
		if (slot != -1) {
			printf("Extracting %s\n",names[i]);
			file= creat(names[i],0x8001);	/* make new file, */
			if (file == -1) {		/* check error, */
				printf("Error: cant create %s\n",names[i]);
				break;
			}
			pos= (long)ldir[slot].off * 128L;	/* find offset, */
			size= (long)ldir[slot].len * 128L;	/* and file size, */
			seek(lbr,pos,0);		/* go there, */
			while (size > 0L) {		/* copy out data, */
				count= (size > bufsize ? bufsize : size);
				read(lbr,buf,count);	/* read data, */
				write(file,buf,count);	/* write it, */
				size-= count;		/* update count, */
			}
			close(file);
		} else printf("%s not in library\n",names[i]);
	}
	close(lbr);
}

/* Find the specified file in the directory, return its slot number 
or -1 if not found. */

find(name)
char *name;
{
int i;
char fname[14];

	for (i= 0; i < num_slots; i++) {		/* search all slots, */
		cvt_from_fcb(ldir[i].name,fname);	/* convert for compare */
		if (strcmp(name,fname) == 0)		/* if found, */
			return(i);			/* return slot number, */
	}
	return(-1);					/* not found, */
}
/* Delete the specified file. No error return. */

kill(name)
char *name;
{
int slot;

	slot= find(name);				/* find it, */
	if (slot != -1) ldir[slot].stat= DELETED;	/* delete it, */
}

/* Find a free slot in the directory. Return -1 if none. */

free_slot() {

int i;

	for (i= 0; i < num_slots; i++) {
		if (ldir[i].stat == UNUSED) return(i);
		if (ldir[i].stat == DELETED) return(i);
	}
	return(-1);
}
/* Add a file to the library. All files get added to the end. Returns 0 
if couldnt add; either file not found or no room. No check is made that the
file might exist already, etc. */

add_file(name)
char *name;
{

int i,file,slot;
unsigned count;
long size,pos;

	slot= free_slot();			/* find a free slot, */
	if (slot == -1) {			/* if none, return 0 */
		printf("No room in directory\n");
		return(0);
	}
	file= open(name,0x8000);		/* get file, */
	if (file == -1) {
		printf("Cant find %s\n",name);
		return(0);
	}

	seek(lbr,0L,2);				/* seek to end of file, */
	pos= tell(lbr);				/* save position, */
	size= 0L;				/* init file size, */
	while (count= read(file,buf,bufsize)) {	/* copy data, */
		size+= count;			/* get file size, */
		write(lbr,buf,count);		/* write to library, */
	}

	for (i= 0; i < 128; i++)
		buf[i]= 0x1a;			/* clear dummy data, */
	count= 128 - (size % 128L);		/* make LBR file a */
	if (count > 0) write(lbr,buf,count);	/* multiple of 128, */
	cvt_to_fcb(name,ldir[slot].name);	/* install name, */
	ldir[slot].off= pos / 128L;		/* position, (sectors) */
	ldir[slot].len= (size + 127L) / 128L;	/* size, (sectors) */
	ldir[slot].stat= ACTIVE;

	close(file);
	return(1);
}
/* Reorganize an LBR file. This consists of creating a new LBR, and copying
all entries to it. This removes any space comsumed by deleted files, etc. */

reorg() {

struct _ludir odir[MAXFILES];
int old,i,n,count;
long size,pos;
char oldname[14],tempname[14],buff[512];
int old_slots,slot;
char *p,*s;

	if (get_directory() == -1) {
		printf("Error: cant find library\n");
		return;
	}
	p= (char *)&ldir[0];		/* copy old directory, */
	s= (char *)&odir[0];
	for (i= (num_slots * sizeof(ludir)); i; i--)
		*s++= *p++;
	old= lbr;			/* old LBR handle, */
	old_slots= num_slots;
	strcpy(oldname,lbrname);	/* old LBR name, */
	printf("Old library has %u slots\n",old_slots);
	strcpy(lbrname,"lu$$$$$$.tmp");	/* make temp file, */
	new_directory();		/* create new one, */

	slot= 0;
	for (i= 1; i < old_slots; i++) {
		if (odir[i].stat == ACTIVE) {
			if (++slot >= num_slots) {
				printf("Not enough room in new directory\n");
				break;
			}
			cvt_from_fcb(odir[i].name,tempname);
			printf("Copying %s\n",tempname);
			seek(old,(long)odir[i].off * 128L,0);	/* seek to old data, */
			seek(lbr,0L,2);				/* EOF on new file, */
			pos= tell(lbr);				/* current position, */
			size= odir[i].len * 128L;
			while (size > 0L) {
				count= (size > sizeof(buff) ? sizeof(buff) : size);
				read(old,buff,count);
				write(lbr,buff,count);
				size-= count;
			}
			ldir[slot].stat= ACTIVE;		/* update new */
			ldir[slot].len= odir[i].len;		/* directory, */
			cvt_to_fcb(tempname,ldir[slot].name);
			ldir[slot].off= (pos + 127L) / 128L;
		}
	}
	put_directory();		/* close new directory, */
	close(old);			/* close old, */
	unlink(oldname);		/* delete old one, */
	link("lu$$$$$$$.tmp",oldname);	/* back to original name */
}
/* Read the directory. */

get_directory()
{
unsigned size;
int i;

	lbr= open(lbrname,0x8002);
	if (lbr == -1) return(-1);

	if (read(lbr,&ldir[0],sizeof(ludir)) != sizeof(ludir))
		return(-1);

	num_slots= (ldir[0].len * 128) / sizeof(ludir);
	size= (num_slots - 1) * sizeof(ludir);	/* already read one, */
	if (num_slots > MAXFILES) {
		printf("Directory error: %s is bad or not an LBR file\n",lbrname);
		return(-1);
	}
	if (read(lbr,&ldir[1],size) != size) {
		printf("Directory error: is %s an LBR file?\n",lbrname);
		return(-1);
	}
	return(1);
}

/* Create a new directory. When we request the number of slots, always round it
up to the nearest 128 bytes, i.e. 4 slots. Since everything is a multiple
of 128 anyways, all were doing is making otherwose wasted space available. */

new_directory() {

int i;

	num_slots= 0;
	do {	cprintf("Number of slots (4 - 256, 0 to abort): ");
		getstring(buf);
		cprintf("\r\n");
		num_slots= atoi(buf);
	} while (num_slots > 256);
	if (num_slots == 0) return(-1);
	num_slots+= num_slots % (128 / sizeof(ludir));

	lbr= creat(lbrname,0x8002);
	if (lbr == -1) return(-1);
	cvt_to_fcb(".",ldir[0].name);
	ldir[0].len= (num_slots * sizeof(ludir)) / 128;
	ldir[0].stat= ACTIVE;
	for (i= 1; i < MAXFILES; i++) {	/* clear ALL slots, */
		cvt_to_fcb(".",ldir[i].name);
		ldir[i].len= 0;
		ldir[i].off= 0;
		ldir[i].stat= UNUSED;
	}
	put_directory();		/* write to disk, */
	return(get_directory());	/* read in new directory, */
}

/* Write the directory out.  */

put_directory() {

int count;

	count= ldir[0].len * 128;
	seek(lbr,0L,0);			/* to start of file, */
	if (write(lbr,&ldir[0],count) != count)
		printf("LBR close error: library may be trashed\n");
	close(lbr);
}


/* Convert a normal asciz string to MSDOS/CPM FCB format. Make the filename
portion 8 characters, extention 3 maximum. */

cvt_to_fcb(inname,outname)
char *inname;
char outname[];
{
char c;
int i;

	if (inname[1] == ':') 			/* if a drive spec, */
		inname= &inname[2];		/* lop it off, */

	for (i= 0; i < 11; i++)
		outname[i]= ' ';		/* clear out name, */
	outname[i]= '\0';			/* null terminate, */
	for (i= 0; i < 11; i++) {
		if (*inname == '\0')		/* if null, */
			outname[i]= ' ';	/* pad with blanks, */
		else if (*inname == '.') {	/* if a dot, */
			++inname;		/* skip it, */
			i= 7;			/* skip to extention, */
		} else {
			outname[i]= toupper(*inname);
			++inname;
		}
	}
	return;
}
/* Convert a CP/M like filename to a normal ASCIZ name. Filter out characters
undersireable on MSDOS. */

cvt_from_fcb(inname,outname)
char *inname,*outname;
{
int i;
char c;

	for (i= 8; i != 0; i--) {		/* do name portion, */
		c= toupper(*inname);
		++inname;
		if (c != ' ') 			/* if not space, */
			*outname++= c;		/* set it, */
	}					/* do all 8 chars, */

	if (*inname != ' ') {			/* if there is an extention, */
		*outname++= '.';		/* add the dot, */
		for (i= 3; i != 0; i--) {	/* do extention, */
			c= toupper(*inname);
			++inname;
			if (c == ' ')
				break;
			*outname++= c;
		}
	}
	*outname= '\0';				/* terminate it, */
	return;
}
/* Get an input string; return NULL if error or empty line. Provide the
usual minimum editing capabilities.  */

getstring(string)
char string[];
{
int count;
char c;
char *p;
int pi;
	count= 0;

	while (1) {

		c= getch();			/* get a character, */
		switch (c) {
		case 0x0d:			/* process it, */
		case 0x0a:
		case 0x1b:
			string[count]= 0x00;	/* terminate string, */
			return(count);		/* return string length */
			break;

		case 0x08:
		case 0x7f:			/* delete character */
		case 0x13:
			if (count) {
				--count;	/* one less char, */
				putch('\010');
				putch(' ');
				putch('\010');
			}
			break;
			case 0x18:
		case 0x15:			/* delete line */
		case 0x19:
		case 0x03:
			while (count) {
				--count;
				putch('\010');
				putch(' ');
				putch('\010');
			}
			break;
		case 0x04:			/* retype character, */
			if (string[count])
				putch(string[count++]);
			break;
		case 0x12:			/* retype line, */
			while (string[count]) {
				putch(string[count++]);
			}
			break;

		default:			/* insert character */
			if ( (c > 0x1f) && (c < 0x7f) && (count < 80) ) {
				string[count++] =toupper(c);
				string[count]= 0x00;
				putch(c);
			} else
				putch(0x07);
			break;
		}
	}
}
/* ATOI() function missing from Lattice C. From Kernighan and Richie. */

atoi(s)
char s[];
{
int i,n;

	n= 0;
	for (i= 0; s[i] >= '0' && s[i] <= '9'; ++i) 
		n= 10*n + s[i]-'0';
	return(n);
}
/* Convert a string to all upper case. */

stoupper(s)
char *s;
{
	while (*s) {
		*s= toupper(*s);
		++s;
	}
}
/* Missing Lattice C function: File rename

	error= link(oldname,newname); */

link(old,new)
char *old,*new;
{
char fcb[36];
int i;
char *p;

	for (i= 0; i < sizeof(fcb); i++)	/* clear it out first, */
		fcb[i]= '\0';

	cvt_to_fcb(old,&fcb[1]);		/* first name, */
	cvt_to_fcb(new,&fcb[17]);		/* new name, */
	return(bdos(23,&fcb) == 0xff);		/* do it. */
}

 