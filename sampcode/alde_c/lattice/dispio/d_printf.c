/*
**       name         printf -- formatted write to console.
**
**       Same as Lattice C printf() function, but much faster.
******************************
**       Renamed and modified to use the DISPIO function d_puts()
**       L. Paper
*/

d_printf(cs, args)
char *cs, *args;
{
        int i, c, n;
        char *p, **na, *_pfmt();
        char work[256];

        na =  &args;           /* point to first arg */
        while (*cs)
        {
                c = *cs++;
                if (c == '%')
                        if (*cs == '%')
                        {
                                c = *cs++;
                                d_putc(c);
                        }
                else
                {
                        p = _pfmt(cs, work, &na, &n);
                        if (p)
                        {
                                cs = p;
                                for (i=0; i<n; i++)
                                        d_putc(work[i]);
                        }
                }
                else
                        d_putc(c);
        }
        return;
}

