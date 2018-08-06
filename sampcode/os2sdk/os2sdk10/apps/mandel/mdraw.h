/**	mdraw.h - mdraw include file
 *	R. A. Garmoe  87/02/05
 */


#define MOUSE			/* compile mouse support */

#define FALSE	0
#define TRUE	~FALSE

#define UP	72		/* up arrow scan code */
#define DOWN	80		/* down arrow scan code */
#define RIGHT	77		/* right arrow scan code */
#define LEFT	75		/* left arrow scan code */


#define RESTORE_MODE	0	/* request for notification to restore mode */
#define RESERVED	0	/* reserved word */
#define REDRAW		1	/* request for notification to redraw */
#define BLOCK		1	/* wait until screen I/O is available */
#define STACKSIZE	2048	/* size of stack to allocate for threads */
#define GIVEUPOWNER	1	/* owner indicator */
#define KILLTHREAD	1	/* kill indicator  */

#define MAXREAL 2000		/* maximum number of real coordinates */
#define MAXLOOP 1000		/* maximum number of iterations */
#define BUFFER	4096		/* number of ints to read at one time */

#define SCANTIME 20		/* scan time in milliseconds */
#define RIPPLE_UP   0		/* ripple palette registers up */
#define RIPPLE_DN   1		/* ripple palette registers down */

#define PAGESIZE 0x8000 	/* size of display page in EGA memory */
#define BLACK	0x00		/* EGA black value	   */
#define BLUE	0x01		/* EGA blue value	   */
#define GREEN	0x02		/* EGA green value	   */
#define CYAN	0x03		/* EGA cyan value	   */
#define RED	0x04		/* EGA red value	   */
#define MAGENTA 0x05		/* EGA magenta value	   */
#define BROWN	0x06		/* EGA brown value	   */
#define WHITE	0x07		/* EGA white value	   */
#define DGRAY	0x08		/* EGA dark gray value	   */
#define LBLUE	0x09		/* EGA light blue	   */
#define LGREEN	0x0a		/* EGA light green	   */
#define LCYAN	0x0b		/* EGA light cyan value    */
#define LRED	0x0c		/* EGA light red value	   */
#define LMAGENTA 0x0d		/* EGA light magenta value */
#define YELLOW	0x0e		/* EGA yellow value	   */
#define IWHITE	0x0f		/* EGA intense white value */

struct cmplx {
	double	 realp; 	/* real part of number */
	double	 imagp; 	/* imaginary part of number */
};


/**	This structure forms a linked list of pointers to image descriptors.
 *	The image descriptor is stored in allocated segments so that
 *	DS is not filled up with the image descriptor.
 */


struct ilist {
	struct ilist *next;	/* pointer to next ilist */
	struct ilist *prev;	/* pointer to previous ilist */
	struct image far *desc; /* pointer to segement containing image data */
};


/**	This structure which is stored in a allocated far segment contains
 *	the data that describes the image.  This is basically the data
 *	stored in the header of the .cnt file.
 */


struct image {
	unsigned savesel[2];	/* selectors for screen saves */
	struct	cmplx ul;	/* coordinates of upper left corner */
	struct	cmplx lr;	/* coordinates of lower right corner */
	int	nimag;		/* number of imaginary coordinates */
	int	lni;
	int	lnr;
	int	mloop;		/* maximum loop count */
	int	nreal;		/* number of real coordinates */

	double	rinc;		/* increment in real coordinate */
	double	iinc;		/* increment in imaginary coordinate */
	double	aspect; 	/* aspect ratio */
	char	cmap[MAXLOOP + 1];   /* mapping from iteration counter to color */
	long	hist[MAXLOOP + 1]; /* histogram counters */
};
