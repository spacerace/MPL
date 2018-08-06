/**	Mstat.c - display Mandelbrot set statistics
 *	R. A. Garmoe  87/01/07
 */



/**	Mstat displays the statistics of the Mandelbrot set contained
 *	in the specified file.	The Mandelbrot set is defined by
 *
 *		z = z**2 + c
 *
 *	where z and c are in the complex plane and z = 0 + 0i for the first
 *	iteration.
 */

/*
 *	The .cnt file has the format
 *		int	number of points along the real axis
 *		int	number of points along the imaginary axis
 *		int	maximum iteration point for each point
 *		double	real coordinate of upper left
 *		double	imaginary coordinate of upper left
 *		double	real coordinate of lower right
 *		double	imaginary coordinate lower rightft
 *		double	increment between points on real axis
 *		double	increment between points on imaginary axis
 *		long	(loop + 1) counters for histogram values
 *
 *	The remainder of the file is the run length encoded scan
 *	lines encoded as:
 *		int	number of words in scan line encoded as:
 *			+int	    actual count value for pixel
 *			-int int    The first value is the run length and
 *				    second value is the run value
 */




#include <stdio.h>

#define FALSE	0
#define TRUE	~FALSE

#define MAXREAL 2000		/* maximum number of real coordinates */
#define MAXLOOP 1000		/* maximum number of iterations */

struct cmplx {
	double	 realp; 	/* real part of number */
	double	 imagp; 	/* imaginary part of number */
};
char	pmand[60] = "mandel.cnt";
FILE	*fmand;

struct	cmplx ul;		/* coordinates of upper left corner */
struct	cmplx lr;		/* coordinates of lower right corner */

int	nimag;			/* number of imaginary coordinates */
int	mloop;			/* maximum loop count */
int	nreal;			/* number of real coordinates */
int	verbose = FALSE;	/* print verbose statistics */

double	rinc;			/* increment in real coordinate */
double	iinc;			/* increment in imaginary coordinate */
double	aspect; 		/* aspect ratio */

long	hist[MAXLOOP + 1] = {0}; /* histogram counters */

main (argc, argv)
int	argc;
char	**argv;
{
	int	lnr;
	int	lni;
	int	ni;
	int	i;
	int	j;
	long   *hp;

	nextarg (argc, argv);
	if ((fmand = fopen (pmand, "rb")) == NULL) {
		printf ("Unable to open count file %s\n", pmand);
		exit (3);
	}

	if (fread ((char *)&nreal, sizeof (int), 1, fmand) != 1) {
		printf ("Error reading maximum number of reals %d\n", nreal);
		exit (2);
	}
	lnr = (nreal > 640)? 640: nreal;
	printf ("nreal = %d %d\n", nreal, lnr);
	if (fread ((char *)&nimag, sizeof (int), 1, fmand) != 1) {
		printf ("Error reading maximum number of imaginaries %d\n", nimag);
		exit (2);
	}
	lni = (nimag > 350)? 350: nimag;
	printf ("nimag = %d %d\n", nimag, lni);
	if (fread ((char *)&mloop, sizeof (int), 1, fmand) != 1) {
		printf ("Error reading maximum loop count %d\n", mloop);
		exit (2);
	}
	printf ("mloop = %d\n", mloop);
	if (fread ((char *)&ul, sizeof (ul), 1, fmand) != 1) {
		printf ("Error reading upper left coordinates\n");
		exit (2);
	}
	printf ("ul = %e+%ei\n", ul.realp, ul.imagp);
	if (fread ((char *)&lr, sizeof (lr), 1, fmand) != 1) {
		printf ("Error reading lower right coordinates\n");
		exit (2);
	}
	printf ("lr = %e+%ei\n", lr.realp, lr.imagp);

	if (fread ((char *)&rinc, sizeof (rinc), 1, fmand) != 1) {
		printf ("Error reading real increment\n");
		exit (2);
	}
	printf ("rinc = %e\n", rinc);
	if (fread ((char *)&iinc, sizeof (iinc), 1, fmand) != 1) {
		printf ("Error reading imaginary increment\n");
		exit (2);
	}
	printf ("iinc = %e\n", iinc);
	if (fread ((char *)&aspect, sizeof (aspect), 1, fmand) != 1) {
		printf ("Error reading aspect ratio\n");
		exit (2);
	}
	printf ("aspect = %e\n", aspect);

	if (fread ((char *)hist, sizeof (long), mloop + 1, fmand) != mloop + 1) {
		printf ("Error reading histogram\n");
		exit (2);
	}
	if (verbose) {
		printf ("     ");
		for ( i = 0; i < 10; i++)
			printf ("%7d", i);
		printf ("\n");
		ni = (mloop + 1) / 10;
		hp = hist;
		for (i = 0; i < ni; i++) {
			printf ("%4d ", i * 10);
			for (j = 0; j < 10; j++)
				printf ("%7ld", *hp++);
			printf ("\n");
		}
		if ((j = (mloop + 1 - ni * 10)) > 0) {
			printf ("%4d ", ni * 10);
			for (; j > 0; j--)
				printf ("%7ld", *hp++);
			printf ("\n");
		}
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
			case 'f':
				argv++;
				/* set file name */
				strcpy (pmand, *argv);
				strcat (pmand, ".cnt");
				break;

			case 'v':
				argv++;
				verbose = TRUE;
				break;

			default:
				printf ("Unknown argument %s\n", *argv);
				exit (1);

		}
	}
}
