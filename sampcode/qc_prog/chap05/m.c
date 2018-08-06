/* m.c -- draw a letter M        */
/*        using IF and CONTINUE  */

/* define characters */

#define CH 'M'    /* character to "draw" with */
#define BLANK ' '
#define NL 10
#define CR 13
#define LEFT 20   /* left side of M   */
#define RIGHT 46  /* right side of M  */
#define BOTTOM 22 /* last line to use */
main()
{
    int pos, line;
    /* space to left side */
    for (line = 1; line <= BOTTOM; line++)
        {
        for (pos = 1; pos < LEFT; pos++)
            {
            putch(BLANK);
            }
        putch(CH); /* draw left side */

        /* are we past midpoint? */
        if (line > ((RIGHT - LEFT) / 2))
            {
            /* yes, so just draw right side */
            for (pos = LEFT; pos < RIGHT; pos++)
                {
                putch(BLANK);
                }
            putch(CH);
            putch(NL);
            putch(CR);
            continue; /* start loop over, do next line */
            }
            /* not past midpoint, check for interior */
        for (pos = LEFT; pos < RIGHT; pos++)
            {
            if ((pos == (LEFT + line )) ||
                 (pos == (RIGHT - line )))
                putch(CH);
            else
                putch(BLANK);
            }
       putch(CH);
       putch(NL);
       putch(CR); /* could also use printf("\n"); */
       }
}
