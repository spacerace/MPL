
/* GRAPHBOX.C -- defined to use PC-specific */
/*               graphics characters        */

#define NL 10
#define CR 13
#define BLANK 32
#define UPLEFT 201
#define UPRIGHT 187
#define LOWLEFT 200
#define LOWRIGHT 188
#define LINE 205
#define SIDE 186

main()
{
    int i, j, height, width;

    /* get height and width from user */
    printf("How high a box do you want? ");
    scanf("%d", &height);
    printf("How wide do you want it to be? ");
    scanf("%d", &width);

    /* draw top of box */
    putch(UPLEFT);
    for (i = 0; i < (width - 2); i++)
        putch(LINE);
    putch(UPRIGHT);
    putch(NL);
    putch(CR); /* go to next line */

    /* draw sides of box */
    for (i = 0; i < height - 2; i++)  /* outer loop */
        {
        putch(SIDE); /* left side */
        for (j = 0; j < (width - 2); j++) /* inner loop */
            {
            putch(BLANK);
            }
        putch(SIDE); /* right side */
        putch(NL);
        putch(CR); /* move to next line */
        }

    /* draw bottom of box */
    putch(LOWLEFT);
    for (i = 0; i < (width - 2); i++)
        putch(LINE);
    putch(LOWRIGHT);
    putch(NL);
    putch(CR); /* box is done, move cursor to new line */
}

