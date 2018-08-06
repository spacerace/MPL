/*
**       name         printf -- formatted write to console.
**
**       Same as Lattice C printf() function, but much faster.
*/

printf(cs, args)
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
                                chrout(c);
                        }
                else
                {
                        p = _pfmt(cs, work, &na, &n);
                        if (p)
                        {
                                cs = p;
                                for (i=0; i<n; i++)
                                        chrout(work[i]);
                        }
                }
                else
                        chrout(c);
        }
        return;
}

chrout(c)
char c;
{
        if (c == '\n')
                bdos(2,'\r');
        bdos(2,c);
}

