/*
 *	sbuf.h -- header file for buffered screen interface
 */

#define SB_OK	0
#define SB_ERR	(-1)

/* screen buffer constants */
#define SB_ROWS	25
#define SB_COLS	80
#define SB_SIZ	SB_ROWS * SB_COLS

/* screen character/attribute buffer element definition */
struct BYTEBUF {
	unsigned char ch;	/* character */
	unsigned char attr;	/* attribute */
};

union CELL {
	struct BYTEBUF b;
	unsigned short cap;	/* character/attribute pair */
};

/* screen buffer control structure */
struct BUFFER {
	/* current position */
	short row, col;

	/* pointer to screen buffer array */
	union CELL *bp;

	/* changed region per screen buffer row */
	short lcol[SB_ROWS];	/* left end of changed region */
	short rcol[SB_ROWS];	/* right end of changed region */

	/* buffer status */
	unsigned short flags;
};

/* buffer flags values */
#define SB_DELTA	0x01
#define SB_RAW		0x02
#define SB_DIRECT	0x04
#define SB_SCROLL	0x08

/* coordinates of a window (rectangular region) on the screen buffer */
struct REGION {
	/* current position */
	short row, col;

	/* window boundaries */
	short r0, c0;	/* upper-left corner */
	short r1, c1;	/* lower-right corner */

	/* scrolling region boundaries */
	short sr0, sc0;	/* upper-left corner */
	short sr1, sc1;	/* lower-right corner */
	
	/* window buffer flags */
	unsigned short wflags;
};
