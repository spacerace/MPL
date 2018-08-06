/*
 *	vf.h -- header for ViewFile program
 */

#define OVERLAP		2
#define MAXSTR		40
#define MSGROW		0
#define MSGCOL		40
#define HEADROW		1
#define TOPROW		2
#define NROWS		23
#define SHIFTWIDTH	20
#define N_NODES		256
#define TABWIDTH	8
#define MAXSCAN		14

typedef enum {
	FORWARD, BACKWARD
} DIRECTION;

/* doubly-linked list structure */
typedef struct dnode_st {
	struct dnode_st *d_next;
	struct dnode_st *d_prev;
	unsigned int d_lnum;	/* file-relative line number */
	unsigned short d_flags;	/* miscellaneous line flags */ 
	char *d_line;		/* pointer to text buffer */
} DNODE;

/* line flags */
#define STANDOUT	0x1
