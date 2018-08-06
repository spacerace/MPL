/* portinfo.c -- reads port values                    */
/* program list -- portinfo.c (inp() not in core lib) */

#include <conio.h>
#include <stdio.h>
main()
{
    unsigned int portnum;
    int regvalue;
    
    printf("Enter number (in hex) of the port ");
    printf("you wish to read: ");
    while (scanf("%x", &portnum) == 1)
        {
    regvalue = inp(portnum);
    printf("\nValue returned for port %x is %d (decimal)"
           "  %x (hex)\n", portnum, regvalue, regvalue);
    printf("Next port? (q to quit): ");
        }
}
