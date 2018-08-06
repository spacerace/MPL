/*
 *	ls.h -- header file for ls program
 */

/* structure definition for output buffer elements */
struct OUTBUF {
	unsigned short o_mode;	/* file mode (attributes) */
	long o_size;		/* file size in bytes */
	unsigned int o_date;	/* file modification date */
	unsigned int o_time;	/* file modification time */
	char *o_name;		/* DOS filespec */
};

/* constants for DOS file-matching routines */
#define FILESPEC	13	/* maximum filespec + NUL */
#define RNBYTES		21	/* bytes reserved for next_fm() calls */

/* file modes (attributes) */
#define READONLY	0x0001
#define HIDDEN		0x0002
#define SYSTEM		0x0004
#define VOLUME		0x0008
#define SUBDIR		0x0010
#define ARCHIVE		0x0020

/* structure definition for DOS file-matching routines */
struct DTA {
	unsigned char d_reserved[RNBYTES];	/* buffer for next_fm */
	unsigned char d_attr;		/* file attribute (type) byte */
	unsigned d_mtime;		/* time of last modification */
	unsigned d_mdate;		/* date of last modification */
	long d_fsize;			/* file size in bytes */
	char d_fname[FILESPEC];		/* file spec (filename.ext + NUL) */
};
