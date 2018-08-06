/*
 *	box.h -- header for box-drawing functions
 */

typedef struct box_st {
	short ul, ur, ll, lr;	/* corners */
	short tbar, bbar;	/* horizontal bars */
	short lbar, rbar;	/* vertical bars */
} BOXTYPE;

/* box types */
#define BOXASCII	0
#define BOX11		1
#define BOX22		2
#define BOX12		3
#define BOX21		4
#define BOXBLK		5
