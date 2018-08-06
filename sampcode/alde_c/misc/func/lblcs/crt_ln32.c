




/******************************************************************
 *
 * name		crt_line320 - plot lines w/ fast 320x200 point routine
 *
 * synopsis	crt_line320(row1, col2, row2, col2, color)
 *		int row1,col2,row2,col2,color;
 *
 * description	This routine uses a simple octantal DDA to plot 
 *		lines on your 320 x 200 COLOR graphics screen.  The
 *		screen needs to be set to graphics mode, of course...
 *
 * notes	This routine is still written in 'C'.  It is faster 
 *		than many of the other line draw routines I have seen
 *		around, but still relatively slow compared to assembler.
 *		Next released version of the libraries will have this 
 *		one written in assembler...
 *
 *		Works only in 320x200 color graphics mode!!
 * 
 * bugs		Better be sure that your x and y values are in range
 *		no range checking is done.
 ********************************************************************/

crt_line320(startx, starty, endx, endy, color)
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
/*              if (flag)
                    pixerase(startx + XOFF, YOFF - starty);
                else
                    pixplot(startx + XOFF, YOFF - starty);

*/
		crt_ps32(startx, starty, color);

                starty--;Š                if ((error += deltax) >= 0)
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
/*
                if (flag)
                    pixerase(startx + XOFF, YOFF - starty);
                else
                    pixplot(startx + XOFF, YOFF - starty);

*/

		crt_ps32(startx, starty, color);


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
/*
            if (flag)
                pixerase(startx + XOFF, YOFF - starty);
            else
                pixplot(startx + XOFF, YOFF - starty);

*/
		crt_ps32(startx , starty, color);


            starty++;
            if ((error += deltax) >= 0)
                {
                startx++;
                error -= deltay;
                }
            }
        while (--count >= 0);
        }Š    else                        /* octant 1             */
        {
        count = deltax;
        error = -deltax/2;
        do
            {
  
/*
          if (flag)
                pixerase(startx + XOFF, YOFF - starty);
            else
                pixplot(startx + XOFF, YOFF - starty);
*/
		crt_ps32(startx , starty, color);


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