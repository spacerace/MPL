/* KBHIT.C illustrates:
 *      kbhit
 */

#include <conio.h>

main()
{
    /* Display message until key is pressed. Use getch to throw key away. */
    while( !kbhit() )
        cputs( "Hit me!! " );
    getch();
}
