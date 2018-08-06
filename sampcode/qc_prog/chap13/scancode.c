/*   scancode.c -- displays ASCII or scan code         */
/*   This program illustrates using getch() to detect  */
/*   special keys such as function keys.               */

#include <conio.h>
#define ESC '\033'     /* ESC key */
main()
{
     int ch;

     printf("Strike keys and see the codes!\n");
     printf("Press the Esc key to quit.\n");

     while ((ch = getch()) != ESC)
          {
          if (ch != 0)
               {
               if (ch <= 32)    /* control characters */
                    printf("^%c has ASCII code %d\n",
                            ch + 64, ch);
               else
                    printf("%c has ASCII code %d\n", ch, ch);
               }
          else              /* ch IS 0 */
               {
               ch = getch();  /* get scan code */
               printf("Scan code is %d\n", ch);
               }
          }
}
