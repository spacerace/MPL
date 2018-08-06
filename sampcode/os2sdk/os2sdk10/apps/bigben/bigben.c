/*
 * This example uses a few of the many VIO calls.
 * 
 * This example puts the time on the screen in large numbers.
 */

#include <stdio.h>
#include <doscalls.h>
#include <subcalls.h>

#define	CHAR_WIDTH	8
#define	CHAR_HEIGHT	7

#define	CLOCK_ROW	10	/* row to start the clock */
#define	TOTAL_COLMS	80	/* screen size in colms */
#define	TOTAL_ROWS	24	/* screen size in rows */

	
char	BigChars[10][CHAR_HEIGHT][CHAR_WIDTH] = {

{
	"   00  ",
	"  0  0 ",
	" 0    0",
	" 0    0",
	" 0    0",
	"  0  0 ",
	"   00  "
},
{
	"   1   ",
	"   1   ",
	"   1   ",
	"   1   ",
	"   1   ",
	"   1   ",
	"   1   "
}, 
{
	"  2222 ",
	" 2    2",
	"      2",
	"     2 ",
	"   2   ",
	"  2    ",
	" 222222" 
},
{
	" 33333 ",
	"      3",
	"      3",
	"   333 ",
	"      3",
	"      3",
	" 33333 " 
},
{
	"    44 ",
	"   4 4 ",
	"  4  4 ",
	" 4   4 ",
	" 444444",
	"     4 ",
	"     4 " 
},
{
	" 555555",
	" 5     ",
	" 55555 ",
	"      5",
	"      5",
	" 5    5",
	"  5555 " 
},
{
	"    6  ",
	"   6   ",
	"  6    ",
	"  6666 ",
	" 6    6",
	" 6    6",
	"  6666 " 
},
{
	" 777777",
	"      7",
	"     7 ",
	"    7  ",
	"   7   ",
	"  7    ",
	" 7     "
},
{
	"  8888 ",
	" 8    8",
	" 8    8",
	"  8888 ",
	" 8    8",
	" 8    8",
	"  8888 "
},
{
	"  9999 ",
	" 9    9",
	" 9    9",
	"  9999 ",
	"    9  ",
	"   9   ",
	"  9    "
}
};


main(argc, argv)
	int	argc;
	char	*argv[];
{
	unsigned	rc;	/* return code */
	struct DateTime	Now;	/* time struct for DOSGETSETTIME */

	/* clear the screen */

	VIOWRTNCELL( (char far *) " \07", TOTAL_ROWS * TOTAL_COLMS, 0, 0, 0 );

	/* paint separaters between hours and minutes, and minutes and seconds*/

	VIOWRTNCELL( (char far *) "|\07", 1, (CLOCK_ROW + 2), 27, 0 );
	VIOWRTNCELL( (char far *) "|\07", 1, (CLOCK_ROW + 5), 27, 0 );
	VIOWRTNCELL( (char far *) "|\07", 1, (CLOCK_ROW + 2), 52, 0 );
	VIOWRTNCELL( (char far *) "|\07", 1, (CLOCK_ROW + 5), 52, 0 );
	 
	for (;;) {

	    /* get the system time */

	    if (rc = DOSGETDATETIME( (struct DateTime far *) &Now))  {

		printf("DOSGETDATETIME failed, error: %d\n", rc);
		DOSEXIT(1, 0);
	    }

	    /* write the digits out to the screen */

	    LoadNumber(Now.hour / 10, 5, CLOCK_ROW);
	    LoadNumber(Now.hour % 10, 15, CLOCK_ROW);
	    LoadNumber(Now.minutes / 10, 30, CLOCK_ROW);
	    LoadNumber(Now.minutes % 10, 40, CLOCK_ROW);
	    LoadNumber(Now.seconds / 10, 55, CLOCK_ROW);
	    LoadNumber(Now.seconds % 10, 65, CLOCK_ROW);

	    DOSSLEEP((long) 900);
	}
}


/* display the digit at the given coordinates */

LoadNumber( dig, x, y )
	unsigned	dig;
	unsigned	x;
	unsigned	y;
{
	int	i;

	/* write a list of char strings to make up a display number */

	for (i=0; (i < CHAR_HEIGHT); i++) 

	    /* write a character string starting from the coordinates */

	    VIOWRTCHARSTR( BigChars[dig][i], CHAR_WIDTH, y++, x, 0);
} 
