/*  menu.c -- uses ansi.sys for cursor control and   */
/*            for video reverse in a sample menu     */
/*  Note: Requires that ANSI.SYS be installed.       */

#include <conio.h>
#define ITEMS 5           /* number of menu items */
#define UP 72             /* scan code for up arrow */
#define DOWN 80           /* scan code for down arrow */
#define VIDREV "\033[7m"  /* reverse video attribute */
#define ATTOFF "\033[0m"  /* turn attributes off   */
#define ED()   printf("\033[2J")  /* erase display */
#define HOME() printf("\033[H")  /* home the cursor */
#define CUU(Y) printf("\033[%dA",Y);  /* cursor up */
#define CUD(Y) printf("\033[%dB",Y);  /* cursor down */

char *Menu[ITEMS] = {"Add a number to the list",
                     "Delete a number from the list",
                     "Clear the list",
                     "Sum the list",
                     "Quit"};
char *Heading =
"Use arrow keys to highlight choice. "
"Use Enter key to select choice.";

void showmenu(int);
int getmesg(int);
main()
{
    int messno = 0; /* message to be highlighted */
    ED();
    showmenu(messno);
    while (messno != ITEMS - 1)
        {
        messno = getmesg(messno);
        ED();
        switch (messno)
           {
           case 0 :
           case 1 :
           case 2 :
           case 3 : printf("...pretending to work ...");
                    printf("Hit any key to continue\n");
                    getch();
                    ED();
                    showmenu(messno);
                    break;
           case 4 : printf("Quitting!\n");
                    break;
           default: printf("Programming error!\n");
                    break;
           }
        }
}

/* showmenu() displays the menu  */
void showmenu(highlite)
int highlite;   /* message number to be highlighted */
{
    int n;
    char *start;

    HOME();
    printf("%s", Heading);
    for (n = 0; n < ITEMS; n++)
    {
        if (n == highlite)
            start = VIDREV; /* turn on reverse video */
        else
            start = ATTOFF;
        printf("\n\n%s%s%s", start, Menu[n], ATTOFF);
    }
    HOME();
    CUD(2 + 2 * highlite);
}

/*  getmesg() selects a menu item */
int getmesg(mnum)
int mnum; /* current message number */
{
    char ch;

    while ((ch = getch()) != '\r')
        if (ch == 0)
            {
            ch = getch();
            switch (ch)
                {
                case UP   : if (mnum > 0)
                                {
                                CUU(2);
                                showmenu (--mnum);
                                }
                            else
                                {
                                CUD(2 * ITEMS - 2);
                                showmenu(mnum=ITEMS-1);
                                }
                            break;
                case DOWN : if (mnum < ITEMS - 1)
                                {
                                CUD(2);
                                showmenu(++mnum);
                                }
                            else
                                {
                                CUU(2 * ITEMS - 2);
                                showmenu(mnum = 0);
                                }
                            break;
                }
            }
        return mnum;
}
