
/* MIXLOOPS.C -- reads characters,       */
/*               beeps for ASCII count   */
/*               uses a while and a for  */

#include <conio.h>

main()
{
    char ch;
    int  i;

    while ((ch = getche()) != ' ')  /* get a char. */
        {
        for (i = 'a'; i <= ch; ++i) /* count up to */
            {                       /* alphabet pos.*/
            printf("In FOR loop!\n");
            printf("\a");  /* sound beep each time */
            }
        }
}

