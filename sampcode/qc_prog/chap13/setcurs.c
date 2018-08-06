/* setcurs.c -- moves cursor, checks out Setcurs() */
#include <dos.h>
#include <stdio.h>
#define VIDEO 0x10
#define SETCURSOR 2
void Setcurs(unsigned char, unsigned char,
             unsigned char);
main()
{
    int row, col;

    printf("Enter row and column: (q to quit)\n");
    while (scanf("%d %d", &row, &col) == 2)
    {
        Setcurs(row, col, 0);
        printf("Enter row and column: (q to quit)");
    }
}

/* Setcurs() -- sets cursor to row, column, and page */
void Setcurs(row, col, page)
unsigned char row, col, page;
{
    union REGS reg;

    reg.h.ah = SETCURSOR;
    reg.h.dh = row;
    reg.h.dl = col;
    reg.h.bh = page;
    int86(VIDEO, &reg, &reg);
}
