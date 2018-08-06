/* strio.c -- uses cgets() and cputs()                   */
/* program list -- strio.c (cgets() not in core library) */
#include <conio.h>
#define MAXSIZE 6
main()
{
     char store[MAXSIZE + 2];

     store[0] = MAXSIZE; /* put limit in first element */
     cputs("What's your name?\n\r");
     cgets(store);
     cputs("\n\rI'll remember you, ");
     cputs(store + 2);
     cputs("!\n\r");
}
