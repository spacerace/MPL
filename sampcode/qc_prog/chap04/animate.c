
/* ANIMATE.C -- animate a graphics character */
/*              until a key is pressed       */

/* Special characters */
#define RTARROW 175
#define LFTARROW 174
#define BLANK 32
#define BACKSPACE 8

main()
{
    int pos, i, j = 1;
    while ( !kbhit() )
        {
        pos = 1;
        while (pos < 79)
            {
            putch(RTARROW);
            i = 1;
            while (i < 1000)
                {
                j = i + 10;
                i++;
                }
            putch(BACKSPACE);
            putch(BLANK);
            pos++;
            }
        while (pos > 1)
            {
            putch(LFTARROW);
            i = 1;
            while (i < 1000)
                {
                j = i + 10;
                i++;
                }
            putch(BACKSPACE);
            putch(BLANK);
            putch(BACKSPACE);
            putch(BACKSPACE);
            pos--;
            }
        }
}

