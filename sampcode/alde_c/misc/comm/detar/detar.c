/*
 *	detar.c
 *
 *	Version 1.1
 *
 *	Fixed bug in convert() wich produced errors on files without
 *	a slash '/' in them.
 *
 *	Unix Tape Archive (TAR) Extractor for MSDOS
 *
 *	Public Domain January 1988, Steve Sampson
 *	Based on the public domain TAR program by John Gilmore
 *
 *	Compiled with ECO-C88 V3.21
 */

/* Includes */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "detar.h"

/* Defines */

#define	isodigit(c)	( ((c) >= '0') && ((c) <= '7') )

/* Globals */

FILE	*fdi,
	*fdo;

union	record	*head;		/* the archive file header */
struct	stat	hstat;		/* the decoded stat info   */
struct	{
	int	year;
	int	month;
	int	day;
	int	hour;
	int	min;
	int	sec;
} tm;

long	s_p_min,
	s_p_hour,
	s_p_day,
	s_p_year;

int	ugswidth = 11,
	dir_only = 0,
	days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char	*months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
	*days[] = {"Thu", "Fri", "Sat", "Sun", "Mon", "Tue", "Wed"},
	tempname[102],
	t_buf[26];

/* Forward References, Prototypes */

long	from_oct(int, char *);
char	*ctime(long *);
char	*convert(char *);
void	cv_time(long);
void	decode_header(union record *, struct stat *, int *);
void	dir_tar(void);
void	de_tar(void);
void	demode(unsigned int, char *);
void	read_header(void);
void	exit(int);

/* Main Program */

main(argc, argv)
int	argc;
char	**argv;
{
	char	*p;

	/*
	 *	The operator can ask for directory only
	 *	or de-tar only.
	 */

	argv++;

	if (argc == 3 && **argv == 'd')  {
		dir_only++;
		argc--;
		argv++;
	}

	if (argc != 2)  {
		fprintf(stderr, "Usage: detar [d] filename[.tar]\n");
		fprintf(stderr, "Where d means directory only\n");
		exit(1);
	}

	for (p = *argv; *p; p++)
		*p = (char)toupper(*p);

	/* parse the many possibilities, this stuff not needed with MSC */

	if ((p = strrchr(*argv, '.')) == (char *)NULL)
		goto add;
	else if (strcmp(p, ".TAR") == NULL)
		strcpy(tempname, *argv);
	else if (*(p+1) == '/' || *(p+1) == '\\')  {
add:		strcpy(tempname, *argv);
		strcat(tempname, ".TAR");
	} else  {
		fprintf(stderr, "File '%s' not a TAR archive\n", *argv);
		exit(1);
	}

	if ((fdi = fopen(tempname, "rb")) == (FILE *)NULL)  {
		fprintf(stderr, "Tar File '%s' Not Found\n", tempname);
		exit(1);
	}

	head = (union record *)malloc(512);

	if (dir_only)
		dir_tar();
	else
		de_tar();

	free(head);
	exit(0);
}


/*
 *	Produce a directory of the files contained on the TAR
 */

void dir_tar()
{
	char	modes[11], *timestamp;
	char	uform[11], gform[11];
	char	*user, *group, size[24];
	int	n, pad, header_std;
	long	longie;

	for (;;)  {
		if ((n = fread((char *)head, 1, 512, fdi)) == EOF) {
			fclose(fdi);
			return;
		}

		decode_header(head, &hstat, &header_std);

		/* File type and modes */

		modes[0] = '?';

		switch (head->header.linkflag) {
		case LF_NORMAL:
		case LF_OLDNORMAL:		case LF_LINK:
			modes[0] = '-'; 
			if ('/' == head->header.name[strlen(head->header.name)-1])
				modes[0] = 'd';
			break;
		case LF_DIR:
			modes[0] = 'd';
			break;
		case LF_SYMLINK:
			modes[0] = 'l';
			break;
		case LF_BLK:
			modes[0] = 'b';
			break;		case LF_CHR:
			modes[0] = 'c';
			break;
		case LF_FIFO:
			modes[0] = 'f';
			break;
		case LF_CONTIG:
			modes[0] = '=';
			break;
		}

		demode(hstat.st_mode, modes+1);

		/* Timestamp */

		longie = hstat.st_mtime;
		timestamp = ctime(&longie);
		timestamp[16] = '\0';
		timestamp[24] = '\0';

		/* User and group names */

		if (*head->header.uname && header_std) {
			user  = head->header.uname;
		} else {
			user = uform;
			sprintf(uform, "%d", hstat.st_uid);
		}

		if (*head->header.gname && header_std) {
			group = head->header.gname;
		} else {
			group = gform;
			sprintf(gform, "%d", hstat.st_gid);
		}

		/* Format the file size or major/minor device numbers */

		switch (head->header.linkflag) {
		case LF_CHR:
		case LF_BLK:
			sprintf(size, "%d, %d",
			   (int)from_oct(8, head->header.devmajor),
			   (int)from_oct(8, head->header.devminor));
			break;
		default:
			sprintf(size, "%ld", hstat.st_size);
		}

		/* Figure out padding and print the whole line. */
		pad = strlen(user) + strlen(group) + strlen(size) + 1;

		if (pad > ugswidth)
			ugswidth = pad;

		printf("%s %s/%s %*s%s %s %s %.*s",
			modes,
			user,
			group,
			ugswidth - pad,
			"",
			size,
			timestamp+4, timestamp+20,
			sizeof(head->header.name),
			head->header.name);

		switch (head->header.linkflag) {
		case LF_SYMLINK:
			printf(" -> %s\n", head->header.linkname);
			break;

		case LF_LINK:
			printf(" link to %s\n", head->header.linkname);
			break;

		default:
			printf(" unknown file type '%c'\n", head->header.linkflag);
			break;

		case LF_OLDNORMAL:
		case LF_NORMAL:
		case LF_CHR:
		case LF_BLK:
		case LF_DIR:
		case LF_FIFO:
		case LF_CONTIG:
			putc('\n', stdout);
			break;
		}

		/*
		 *	Seek to next file
		 */

		fseek(fdi, hstat.st_size, 1);

		/*
		 *	File starts on 512 byte boundary
		 */

		fseek(fdi, 512L - (hstat.st_size % 512L), 1);
	}
}


/*
 *	Extract the files from the TAR archive
 *
 */

void de_tar()
{
	long	size;
	int	header_std, c;
	char	*workfile;

	for (;;)  {
		if ( fread((char *)head, 1, 512, fdi) == EOF)  {
			fclose(fdi);
			return;
		}

		decode_header(head, &hstat, &header_std);
		workfile = convert(head->header.name);

		printf("Extracting: %s\n", workfile);

		fdo = fopen(workfile, "w");
		size = hstat.st_size;

		while ((c = getc(fdi)) != EOF)  {

			/*
			 *	Convert Linefeed to MSDOS
			 *	Carriage Return, Linefeed pair
			 *
			 */

			if (c == 0x0A)
				putc('\n', fdo);
			else
				putc(c, fdo);

			if (--size == 0L)  {

			   /*
			    *	Get to next 512 byte boundary
			    */

			   fseek(fdi, 512L - (hstat.st_size % 512L), 1);
			   break;
			}
		}

		fclose(fdo);
	}
}


/* 
 *	Break down the header info into stat info
 *
 *	Set "*stdp" to != 0 or == 0 depending whether
 *	header record is "Unix Standard" or "old" tar format.
 *
 */

void decode_header(header, st, stdp)
register union record	*header;
register struct stat	*st;
register int		*stdp;
{
	st->st_mode  = (int)from_oct( 8, header->header.mode);
	st->st_mtime = from_oct(12, header->header.mtime);
	st->st_size  = from_oct(12, header->header.size);
	
	if (0 == strcmp(header->header.magic, TMAGIC)) {

		/* Unix Standard tar archive */

		*stdp = 1;
		st->st_dev = 0;

	} else {

		/* Old fashioned tar archive */

		*stdp = 0;
		st->st_uid = (int)from_oct(8, header->header.uid);
		st->st_gid = (int)from_oct(8, header->header.gid);
		st->st_dev = 0;
	}
}


/*
 *	Decode the mode string from a stat entry into a 9-char string
 */

void demode(mode, string)
register unsigned mode;
register char	  *string;
{
	register unsigned mask;
	register char	  *rwx = "rwxrwxrwx";

	for (mask = 0400; mask != 0; mask >>= 1) {
		if (mode & mask)
			*string++ = *rwx++;
		else {
			*string++ = '-';
			rwx++;
		}
	}

	if (mode & S_ISUID)
		if (string[-7] == 'x')
			string[-7] = 's';
		else
			string[-7] = 'S';

	if (mode & S_ISGID)
		if (string[-4] == 'x')
			string[-4] = 's';
		else
			string[-4] = 'S';

	if (mode & S_ISVTX)
		if (string[-1] == 'x')
			string[-1] = 't';
		else
			string[-1] = 'T';

	*string = '\0';
}


/*
 *	Quick and dirty octal conversion.
 *
 *	Result is -1 if the field is invalid (all blank, or nonoctal).
 */

long from_oct(digs, where)
register int	digs;
register char	*where;
{
	register long	value;

	while (isspace(*where)) {		/* Skip spaces */
		where++;
		if (--digs <= 0)
			return -1;		/* All blank field */
	}

	value = 0;
	while (digs > 0 && isodigit(*where)) {	/* Scan til nonoctal */
		value = (value << 3) | (*where++ - '0');
		--digs;
	}

	if (digs > 0 && *where && !isspace(*where))
		return -1;			/* Ended on non-space/nul */

	return value;
}


/*
 *	Borrowed from comp.os.minix
 */

char *ctime(time) 
long	*time;
{
	long	tt;

	s_p_min  = 60L;
	s_p_hour = 60L * 60L;
	s_p_day  = 60L * 60L * 24L;
	s_p_year = 60L * 60L * 24L * 365L;

	tt = *time;
	cv_time(tt);

	sprintf(t_buf, "%s %s %02d %02d:%02d:%02d %04d\n",
	   days[(tt / s_p_day) % 7], months[tm.month], tm.day, tm.hour,
	     tm.min, tm.sec, tm.year); 

	return(t_buf);
}


void cv_time(time)
long	time;
{
	tm.year = 0;
	tm.month = 0;
	tm.day = 1;
	tm.hour = 0;
	tm.min = 0;
	tm.sec = 0;

	while (time >= s_p_year)  {
		if (((tm.year + 2) % 4) == 0)
			time -= s_p_day;

		tm.year += 1;
		time -= s_p_year;
	}

	if (((tm.year + 2) % 4) == 0)
		days_per_month[1]++;

	tm.year += 1970;

	while ( time >= (days_per_month[tm.month] * s_p_day))
		time -= days_per_month[tm.month++] * s_p_day;

	while (time >= s_p_day) {
		time -= s_p_day;
		tm.day++;
	}

	while (time >= s_p_hour) {
		time -= s_p_hour;
		tm.hour++;
	}

	while (time >= s_p_min) {
		time -= s_p_min;
		tm.min++;
	}

	tm.sec = (int)time;
}

char *convert(unixname)
register char *unixname;
{
	static   char	temp[16];
	register char	*p;
	register int	have_dot;

	/* First get rid of directory specs */

	if ((p = strrchr(unixname, '/')) != (char *)NULL)
		strcpy(temp, p + 1);
	else
		strcpy(temp, unixname);

	/* Then make a basic MSDOS filename */

	if (p = strchr(temp, '.'))  {
		if (strlen(p) > 4)
			p[4] = '\0';

		if ((p - temp) > 8)  {
			temp[8] = '\0';
			strcat(temp, p);
		}
	}
	else
		temp[8] = '\0';

	/* Now look for two dots in filename, only one allowed in MSDOS */
	/* substitute a dash for second dot */

	for (have_dot = 0, p = temp; *p; p++)  {
		if (*p == '.')  {
			if (have_dot)
				*p = '-';
			else
				have_dot = 1;
		}
	}

	return temp;
}

/* EOF */
