/***	Mandel.c - compute and display Mandelbrot set
 *	R. A. Garmoe  86/12/10
 */



/**	Mandel compute the elements of the Mandelbrot set
 *
 *		z = z**2 + c
 *
 *	where z and c are in the complex plane and z = 0 + 0i for the first
 *	iteration.  The elements of the set along with the description of the
 *	computation are written to the .cnt file.
 */




/**	Call
 *
 *	mandel -v {-u a+bi} {-l c+di} {-r num} {-c num} {-i itr} {-a f} {-f name}
 *		{-d display}
 *
 *	where
 *
 *		a	compute each point with an aspect ratio of f where
 *			f is :
 *				f.ffff	floating point number describing the
 *					ratio of width vs height of each pixel
 *
 *		c	number of points across the real coordinate.  This
 *			value cannot exceed 2000 and is defaulted to 640.
 *
 *		d	set display to device.	The default display is EGA.
 *				CGA	Color Graphics Adapter
 *					columns = 350
 *					rows	= 200
 *					aspect	= 2.4
 *
 *				EGA	Extended Graphics Adapter
 *					columns = 640
 *					rows	= 350
 *					aspect	= 2.4
 *
 *				PGA	Professional Graphics Adapter
 *					columns = 640
 *					rows	= 480
 *					aspect	= 1.0
 *
 *		f	name for the .cnt file
 *			If name is not specified, "mandel" is used.
 *
 *		i	iteration limit.  This value cannot exceed 1000 and
 *			the default is 256
 *
 *		l	lower right corner as c+di
 *
 *
 *		r	number of points down the imaginary coordinate.  This
 *			value is defaulted to 350.
 *
 *		u	upper left corner as a+bi
 *
 *		v	display information about each scan line as processed
 *
 *	The .cnt file has the format
 *		int	number of points along the real axis
 *		int	number of points along the imaginary axis
 *		int	maximum iteration point for each point
 *		double	real coordinate of upper left
 *		double	imaginary coordinate of upper left
 *		double	real coordinate of lower right
 *		double	imaginary coordinate lower rightft
 *		long	(loop + 1) counters for histogram values
 */




#include <stdio.h>
#include <doscalls.h>

int mandinter ();

#define FALSE	0
#define TRUE	~FALSE

#define MAXREAL 2000		/* maximum number of points in line */
#define MAXLOOP 1000		/* maximum number of iterations */

#define CGA_DEV 0		/* Color Graphics Adapter */
#define CGA_COL 350		/* number of display columns for CGA */
#define CGA_ROW 200		/* number of display rows for CGA */
#define CGA_ASP 12/5		/* aspect ratio (width/height) for CGA */

#define EGA_DEV 1		/* Extended Graphics Adapter */
#define EGA_COL 640		/* number of display columns for EGA */
#define EGA_ROW 350		/* number of display rows for EGA */
#define EGA_ASP 1.33		/* aspect ratio (width/height) for EGA */

#define PGA_DEV 2		/* Professional Graphics Adapter */
#define PGA_COL 640		/* number of display columns for PGA */
#define PGA_ROW 480		/* number of display rows for PGA */
#define PGA_ASP 1.0		/* aspect ratio (width/height) for PGA */


struct cmplx {
	double	 realp; 	/* real part of number */
	double	 imagp; 	/* imaginary part of number */
};



char	fp287;			/* 287 processor status from DOSDEVCONFIG */
char	 pmand[60] = "mandel.cnt"; /* file name for loop counts */
FILE	*fmand;

double	rinc;			/* increment in real coordinate */
double	iinc;			/* increment in imaginary coordinate */

int	device = EGA_DEV;	/* default device type */
int	arg_device = -1;	/* device type from arguments */
int	mcount[MAXREAL];	/* array holding interation counters */
int	rlcount[MAXREAL + 2];	/* run length encoded interation counters */
int	nreal = EGA_COL;	/* number of points on real coordinate */
int	arg_nreal = 0;		/* number real coordinate from arguments */
int	nimag = EGA_ROW;	/* number of points on imaginary coordinate */
int	arg_nimag = 0;		/* number imaginary coordinate from arguments */
int	loop = 256;		/* maximum loop count for each point */
int	verbose = FALSE;	/* display scan line information if true */
double	aspect = EGA_ASP;	/* default screen aspect ratio */
double	arg_aspect = 0.0;	/* aspect from arguments */

long	hist[MAXLOOP + 1] = {0}; /* histogram counters */

struct	cmplx c;		/* value of mandelbrot seed */
struct	cmplx ul;		/* coordinates of upper left corner */
struct	cmplx lr;		/* coordinates of lower right corner */


main (argc, argv)
int	argc;
char	**argv;
{
	int	nr;
	int	ni;
	int	i;
	int	temp;
	int    *rl;
	long	position;

	if (DOSDEVCONFIG ((char far *)&fp287, 3, 0) != 0) {
		printf ("Unable to get fp processor presence flag\n");
		exit (1);
	}
	if (fp287 != 1) {
		printf ("There is not a 287 fp processor present\n");
		exit (1);
	}
	nextarg (argc, argv);
	if ((fmand = fopen (pmand, "wb")) == NULL) {
		printf ("Unable to open count file %s\n", pmand);
		exit (3);
	}
	if (ul.realp == lr.realp && ul.imagp == lr.imagp) {
		printf ("%15.13lf+%15.13lfi, %15.13lf+%15.13lfi\n",ul.realp,ul.imagp,lr.realp,lr.imagp);
		printf ("Upper left real = lower right real\n");
		exit (3);
	}

	/* reset nreal, nimag, aspect from command line device type */

	switch (arg_device) {
		case CGA_DEV:
			nreal = CGA_COL;
			nimag = CGA_ROW;
			aspect = CGA_ASP;
			break;

		case EGA_DEV:
			nreal = EGA_COL;
			nimag = EGA_ROW;
			aspect = EGA_ASP;
			break;

		case PGA_DEV:
			nreal = PGA_COL;
			nimag = PGA_ROW;
			aspect = PGA_ASP;
			break;
	}

	/* reset individual parameters */

	if (arg_nreal != 0)
		nreal = arg_nreal;
	if (arg_nimag != 0)
		nimag = arg_nimag;
	if (arg_aspect != 0)
		aspect = arg_aspect;

	/* compute the step increments for the specifed aspect */

	rinc = (lr.realp - ul.realp) / (double)nreal;
	iinc = rinc * aspect;

	lr.imagp = ul.imagp - iinc * nimag;

	fwrite ((char *)&nreal, sizeof (int), 1, fmand);
	fwrite ((char *)&nimag, sizeof (int), 1, fmand);
	fwrite ((char *)&loop, sizeof (int), 1, fmand);
	fwrite ((char *)&ul, sizeof (ul), 1, fmand);
	fwrite ((char *)&lr, sizeof (lr), 1, fmand);
	fwrite ((char *)&rinc, sizeof (rinc), 1, fmand);
	fwrite ((char *)&iinc, sizeof (iinc), 1, fmand);
	fwrite ((char *)&aspect, sizeof (aspect), 1, fmand);
	position = ftell (fmand);
	fwrite ((char *)hist, sizeof (long), loop + 1, fmand);

	c.imagp = ul.imagp;
	for (ni = 0; ni < nimag; ni++) {
		c.realp = ul.realp;
		if (verbose)
			printf ("%4d, %15.13lf + %15.13lfi", ni, c.realp, c.imagp);
		manditer (c.realp, c.imagp, loop, nreal, mcount, rinc);
		for (nr = 0; nr < nreal; nr++) {
			i = mcount[nr];
			hist[i]++;
		}

		/* run length encode counts for this scan line */

		temp = *mcount;
		nr = 1;
		rl = rlcount + 1;
		i = -1;
		for (; nr < nreal; nr++) {
			i++;
			if (mcount[nr] != temp) {
				/* process change in value */
				if (i != 0)
					/* output count of duplicate values */
					*rl++ = -(i + 1);
				/* output value */
				*rl++ = temp;
				/* reset compression values */
				temp = mcount[nr];
				i = -1;
			}
		}
		if (i != -1) {
			if (i != 0)
				/* output count of duplicate values */
				*rl++ = -(i + 2);
			/* output value */
			*rl++ = temp;
		}
		*rlcount = rl - rlcount -1;
		if (verbose)
			printf ("%4d\n", *rlcount);
		if (fwrite ((char *)rlcount, sizeof (int), *rlcount + 1, fmand) !=
		    *rlcount + 1)  {
			printf ("error writing count file %s\n", pmand);
			exit (4);
		}
		c.imagp -= iinc;
	}
	fseek (fmand, position, 0);
	if (fwrite ((char *)hist, sizeof (long), loop + 1, fmand) != loop + 1) {
		printf ("error writing histogram to file %s\n", pmand);
		exit (4);
	}
}



/**	nextarg - process arguments
 *
 */


nextarg (argc, argv)
int	argc;
char	**argv;
{

	argv++;
	while ((argc-- > 0) && (**argv == '-')) {
		switch (*(*argv+1)) {
			case 'a':
				/* specify aspect ratio */
				argv++;
				if ((argc < 1) ||
				    (sscanf (*argv++, "%lf", &arg_aspect) != 1)) {
					printf ("Error specifing aspect\n");
					exit (1);
				}
				argc--;
				break;

			case 'c':
				/* specify number of columns and limit to maximum value */
				argv++;
				if ((argc < 1) ||
				    (sscanf (*argv++, " %d", &arg_nreal) != 1)) {
					printf ("Error specifying number of real points\n");
					exit (1);
				}
				argc--;
				if (arg_nreal > MAXREAL) {
					arg_nreal = MAXREAL;
					printf ("Number of columns limited to %d\n", MAXREAL);
				}
				break;

			case 'd':
				/* specify default device paramters */
				argv++;
				if (strcmp (*argv, "CGA") == 0)
					arg_device = CGA_DEV;
				else if (strcmp (*argv, "EGA") == 0)
					arg_device = EGA_DEV;
				else if (strcmp (*argv, "PGA") == 0)
					arg_device = PGA_DEV;
				else {
					printf ("Error specifing device\n");
					exit (1);
				}
				argv++;
				argc--;
				break;

			case 'f':
				/* specify output file name */
				argv++;
				/* set file name */
				strcpy (pmand, *argv);
				strcat (pmand, ".cnt");
				break;

			case 'i':
				/* specify iteration count and limit to maximum */
				argv++;
				if ((argc < 1) ||
				    (sscanf (*argv++, " %d", &loop) != 1)) {
					printf ("Error specifying iteration limit\n");
					exit (1);
				}
				argc--;
				if (loop > MAXLOOP) {
					loop = MAXREAL;
					printf ("Iteration count limited to %d\n", MAXLOOP);
				}
				break;

			case 'l':
				/* specify lower right corner as c+di */
				argv++;
				if ((argc < 1) || (sscanf (*argv++, "%lf%lfi",
				    &lr.realp, &lr.imagp) != 2)) {
					printf ("Error specifying lower right\n");
					exit (1);
				}
				argc--;
				break;


			case 'r':
				/* specify number of rows */
				argv++;
				if ((argc < 1) ||
				    (sscanf (*argv++, " %d", &arg_nimag) != 1)) {
					printf ("Error specifying number of imaginary points\n");
					exit (1);
				}
				argc--;
				break;

			case 'u':
				/* specify upper right corner as a+bi */
				argv++;
				if ((argc < 1) || (sscanf (*argv++, "%lf%lfi",
				    &ul.realp, &ul.imagp) != 2)) {
					printf ("Error specifying upper left\n");
					exit (1);
				}
				argc--;
				break;

			case 'v':
				/* specify verbose output */
				argv++;
				verbose = TRUE;
				break;

			default:
				printf ("Unknown argument %s\n", *argv);
				exit (1);

		}
	}
}
