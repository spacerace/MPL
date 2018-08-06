/* scrinv.c  --  using a far pointer to access text   */
/*               screen memory                        */

#define ROWS 25
#define COLS 80

main()
{
    int far *screenp;
    int temp, i;

    do
        {
        /* use 0xB800000 for EGA or VGA */
        screenp = (int far *)0xB0000000;

        for (i = 0; i < ((ROWS*COLS)/2); ++i)
            {
            temp = screenp[i];
            screenp[i] = screenp[(ROWS*COLS)-i-1];
            screenp[(ROWS*COLS)-i-1] = temp;
            }
        } while (getch() != 'Q');

}
