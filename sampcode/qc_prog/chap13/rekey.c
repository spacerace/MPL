/* rekey.c -- transliterates typed input             */
/*    This program illustrates getch() and putch().  */

#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#define ESC '\033'   /* the escape key */
char Newchars[] = "qwertyuiopasdfghjklzxcvbnm";
 /* values to be assigned to the a,b,c keys, etc. */
main()
{
    char ch;

    printf("Type characters and see them transformed;\n");
    printf("Press the Esc key to terminate.\n");
    while ((ch = getch()) != ESC)
        if (islower(ch))
            putch(Newchars[ch - 'a']);
        else if (isupper(ch))
            {
            ch = tolower(ch);
            putch(toupper(Newchars[ch - 'a']));
            }
        else if (ch == '\r')
            {
            putch('\n');
            putch('\r');
            }
        else
            putch(ch);
}
