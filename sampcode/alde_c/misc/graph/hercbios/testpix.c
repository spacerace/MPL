
/*  MAIN tests PIXEL by drawing lines on the screen  */

#include	"stdio.h"

main ()
{
	int i,j;
	char	line [90];

	/* first clear screen */
	grafix (0);

	/* now draw a couple of lines */
	for (i=0; i<696; i++)
	{
		pixel (i, i/2, 1);
		pixel (i, 348 - i/2, 1);
	}

	waitkey();		/* wait for CR */

	/* a different pattern on page 1 */
	swpage (1);
	for (i=0; i<720; i++)
	for (j=0; j<348; j=j+50)
		pixel (i,j,1);
	for (i=0; i<720; i=i+90)
	for (j=0; j<348; j++)
		pixel (i,j,1);

	/* switch back and forth a couple of times */
	waitkey();
	swpage (0);
	waitkey();
	swpage (1);
	waitkey();
	swpage (0);
	waitkey();
	swpage (1);
	waitkey();

#ifndef NOTONE
	/* add some writing */
	dline (10, 4, "Hello, there!");
	dline (10, 5, "Second LINE of text.");
	dline (10, 6, "all on display page 1.");

	waitkey();
	for (i=0; i<8; i++)
		for (j=0; j<32; j++)
			dchar (j+5, i+13, (char) (32*i+j));

	waitkey();
#endif
	/* repeat for page 0 */
	swpage (0);

#ifndef NOTZERO
	/* add some writing */
	dline (10, 4, "Hello, there!");
	dline (10, 5, "Second LINE of text.");
	dline (10, 6, "this time on display page 0.");

	waitkey();
	for (i=0; i<8; i++)
		for (j=0; j<32; j++)
			dchar (j+5, i+13, (char) (32*i+j));
#endif

	waitkey();

	/* back to alpha mode */
	waitkey();
	alfa();
}

dline (x, y, chstr)	/* write character-string at x,y */
	int 	x,y;
	char	chstr [90];
{
	int	i;
	char	*p;

	i=0;
	for ( p=chstr; *p != '\0'; p++ )
	{
		dchar ( x+i, y, *p );
		i++;
	}
}
