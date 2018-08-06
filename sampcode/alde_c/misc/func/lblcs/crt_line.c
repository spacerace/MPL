



/******************************************************************
 *
 * name		crt_line - plot lines w/ BIOS point set routine
 *
 * synopsis	crt_line(row1,col1,row2,col2,color);		
 *		int row1,col2,row2,color;
 *
 * description	This routine uses a simple octantal DDA to plot 
 *		lines on your graphics screen.  The screen 
 *		needs to be set to graphics mode, of course...
 *
 * notes	This routine is still written in 'C'.  It is faster 
 *		than many of the other line draw routines I have seen
 *		around, but still relatively slow compared to assembler.
 *
 *		Since this routine uses the BIOS point set routine,
 *		it will run on color or mono display adaptors, in
 *		any supported graphics mode.  The disadvantage of 
 *		using the BIOS point set routine is that it is
 *		extremely slow.....  The speed of this routine is
 *		entirely limited by the speed of the BIOS pset call. 
 * 
 * bugs		Better be sure that your x and y values are in range
 *		no range checking is done.
 ********************************************************************/

crt_line(startx, starty, endx, endy, color)
int  startx, starty, endx, endy, color;
    {
    int count, deltax, deltay, error, t;


    if (endx < startx)
        {
        t = startx;             /* fold quadrants 2, 3 to 1, 4  */
        startx = endx;
        endx = t;
        t = starty;
        starty = endy;
        endy = t;
        }
    deltax = endx - startx;
    deltay = endy - starty;
    if (deltay < 0)
        {
        if (-deltay > deltax)   /* octant 7             */
            {
            count = -deltay;
            error = deltay / 2;
            do
                {
		crt_pset(startx, starty, color);

                starty--;
                if ((error += deltax) >= 0)
                    {
                    startx++;
                    error += deltay;
                    }
                }
            while (--count >= 0);
            }
        else                    /* octant 8             */
            {
            count = deltax;
            error = -deltax / 2;
            do
                {

		crt_pset(startx, starty, color);

                startx++;
                if ((error -= deltay) >= 0)
                    {
                    starty--;
                    error -= deltax;
                    }
                }
            while (--count >= 0);
            }
        }
    else if (deltay > deltax)       /* octant 2         */
        {
        count = deltay;
        error = -deltay / 2;
        do
            {
		crt_pset(startx , starty, color);

            starty++;
            if ((error += deltax) >= 0)
                {
                startx++;
                error -= deltay;
                }
            }
        while (--count >= 0);
        }
    else                        /* octant 1             */
        {
        count = deltax;
        error = -deltax/2;
        do
            {
  
		crt_pset(startx , starty, color);

            startx++;
            if ((error += deltay) >= 0)
                {
                starty++;
                error -= deltax;
                }
            }
        while (--count >= 0);
        }
    }





