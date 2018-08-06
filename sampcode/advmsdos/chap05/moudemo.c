 /*
    Simple Demo of Int 33H Mouse Driver
    (C) 1988 Ray Duncan

    Compile with: CL MOUDEMO.C

    Usage:   MOUDEMO  (press both mouse buttons to exit)
*/

#include <stdio.h>
#include <dos.h>

union REGS regs;

void cls(void);                     /* function prototypes */
void gotoxy(int, int);

main(int argc, char *argv[])
{
    int x,y,buttons;                /* some scratch variables */
                                    /* for the mouse state */

    regs.x.ax = 0;                  /* reset mouse driver */
    int86(0x33, &regs, &regs);      /* and check status */
   
    if(regs.x.ax == 0)              /* exit if no mouse */
    {   printf("\nMouse not available\n");
        exit(1);
    }

    cls();                          /* clear the screen */  
    gotoxy(45,0);                   /* and show help info */
    puts("Press Both Mouse Buttons To Exit");

    regs.x.ax = 1;                  /* display mouse cursor */
    int86(0x33, &regs, &regs);
   
    do {
        regs.x.ax = 3;              /* get mouse position */    
        int86(0x33, &regs, &regs);  /* and button status */
        buttons = regs.x.bx & 3;
        x = regs.x.cx;
        y = regs.x.dx;
    
        gotoxy(0,0);                /* display mouse position */
        printf("X = %3d  Y = %3d", x, y);
        
    } while(buttons != 3);          /* exit if both buttons down */

    regs.x.ax = 2;                  /* hide mouse cursor */
    int86(0x33, &regs, &regs);
   
    cls();                          /* display message and exit */
    gotoxy(0,0);
    puts("Have a Mice Day!");
}

/*
    Clear the screen 
*/
void cls(void)
{
    regs.x.ax = 0x0600;             /* ROM BIOS video driver */
    regs.h.bh = 7;                  /* Int 10H Function 6 */
    regs.x.cx = 0;                  /* initializes a window */
    regs.h.dh = 24;
    regs.h.dl = 79;
    int86(0x10, &regs, &regs);
}

/*
    Position cursor to (x,y)
*/
void gotoxy(int x, int y)
{
    regs.h.dl = x;                  /* ROM BIOS video driver */
    regs.h.dh = y;                  /* Int 10H Function 2 */
    regs.h.bh = 0;                  /* positions the cursor */
    regs.h.ah = 2;
    int86(0x10, &regs, &regs);
}

