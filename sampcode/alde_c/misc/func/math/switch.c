
/******************************************************************************
*******************************************************************************


                     THIS UTILITY SELECTS THE ACTIVE DISPLAY
             USAGE:  switch [MONITOR] [MODE]

                     [MONITOR] = monochrome or color.
                        type in either mono or monochrome,
                                or col or color for color display.

                     [MODE] = (FOR COLOR ONLY)
                        0     40 * 25 BLACK AND WHITE
                        1     40 * 25 COLOR
                        2     80 * 25 BLACK AND WHITE
                        3     80 * 25 COLOR
                        4     320 * 200 COLOR  (MED. RES.)
                        5     320 * 200 BLACK AND WHITE  (MED. RES.)
                        6     640 * 200 BLACK AND WHITE  (HIGH RES.)

*******************************************************************************
******************************************************************************/


#include "stdio.h"
#include "conio.h"
#include "utility.h"
#include "screen.h"
#include "string.h"
#include "ctype.h"
main(argc, argv)
int argc;
char **argv;
{
        ADS equip_flag, new_equip_flag;
        int comlinptr, ax, bx, cx, dx, mode, columns, page, x;
        unsigned flag;
        char c, *comlin1, *calloc();
        comlin1 = calloc(40, 1);/*allocate 40 bytes for comlin1*/
        x = 0;

        if(argc < 2)/*check for command line argument*/
                utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");

        equip_flag.s = 0x0040;/*rom bios data segment*/
        equip_flag.r = 0x0010;/*offset*/
        new_equip_flag.s = _defds;/*default data segment*/
        new_equip_flag.r = &flag;

        strcpy(comlin1, argv[1]);/*move argv1 to comlin1 for convert to LC*/
        while((c = comlin1[x]) != '\0'){/*convert if upper case to lower*/
                comlin1[x++] = tolower(c);
        }

        strcpy(argv[1], comlin1);/*return changed string to  argv[1]*/
        free(comlin1);/*free allocated space of comlin1*/

        if((stsindex("mono", argv[1])) == 0){/*switch to monochrome*/
                if(!scmode(&mode, &columns, &page))/*check not on monochrome*/
                        utabort("ALREADY ON MONOCHROME ADAPTER");
                outp(0x3d8, 00);/*deselect video enable on 6845*/
                bios(17, &ax, &bx, &cx, &dx);/*get current flag into ax*/
                flag = ax | 0x0030;/*set equipment flag to monochrome*/
                utslmove(&new_equip_flag, &equip_flag, 4);/*return flag*/
                screset(0);/*bios set mode call*/
                exit();
        }

        if((stsindex("col", argv[1])) == 0){/*switch to color display*/
             if(argc != 3 || argc > 3)/*correct number of arguments*/
                utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");
             if(scmode(&mode, &columns, &page))/*check not on color already*/
                      utabort("ALREADY ON COLOR ADAPTER");
             clear_screen();/*clear mono screen*/
             utslmove(&equip_flag, &new_equip_flag, 4);
             flag = flag ^ 0x0030;/*erase old display info*/
             flag = flag | 0x0020;/*set as 80 * 25 color card*/
             utslmove(&new_equip_flag, &equip_flag, 4);/*return flag*/
             stcd_i(argv[2], &comlinptr);/*set com line arg to integer*/
             screset(comlinptr);/*set mode from command line*/
             exit();
        }
        utabort("NO COMMAND LINE ARGUMENT  switch [MONITOR] [MODE]");
}


clear_screen()/*clears monochrome screen*/
{

        int ax, bx, cx, dx;
        ax = 0x0600;/*set ah = 6  scroll screen*/
        bx = 0x0700;/*attribute 7*/
        cx = 0x0000;
        dx = 0x184f;/*set 24, 79 for scroll*/

        bios(16, &ax, &bx, &cx, &dx);

}


