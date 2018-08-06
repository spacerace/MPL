/* MOVEMEM.C illustrate direct memory access using functions:
 *      movedata        FP_SEG          FP_OFF
 *
 * Also illustrated:
 *      pack pragma
 *
 * See COPY2.C for another example of FP_SEG.
 */

#include <memory.h>
#include <stdio.h>
#include <dos.h>

#pragma pack(1)     /* Use pragma to force packing on byte boundaries. */

struct LOWMEMVID
{
    char     vidmode;       /* 0x449 */
    unsigned scrwid;        /* 0x44A */
    unsigned scrlen;        /* 0x44C */
    unsigned scroff;        /* 0x44E */
    struct   LOCATE
    {
        unsigned char col;
        unsigned char row;
    } csrpos[8];            /* 0x450 */
    struct   CURSIZE
    {
        unsigned char end;
        unsigned char start;
    } csrsize;              /* 0x460 */
    char     page;          /* 0x462 */
} vid;
struct LOWMEMVID far *pvid = &vid;

main()
{
    int page;

    /* Move system information into uninitialized structure variable. */
    movedata( 0, 0x449, FP_SEG( pvid ), FP_OFF( pvid ), sizeof( vid ) );

    printf( "Move data from low memory 0000:0449 to structure at %Fp\n\n",
             (void far *)&vid );

    printf( "Mode:\t\t\t%u\n", vid.vidmode );
    printf( "Page:\t\t\t%u\n", vid.page );
    printf( "Screen width:\t\t%u\n", vid.scrwid );
    printf( "Screen length:\t\t%u\n", vid.scrlen );
    printf( "Cursor size:\t\tstart: %u\tend: %u\n",
             vid.csrsize.start, vid.csrsize.end );
    printf( "Cursor location:\t" );
    for( page = 0; page < 8; page++ )
        printf( "page:\t%u\tcolumn: %u\trow: %u\n\t\t\t",
                page, vid.csrpos[page].col, vid.csrpos[page].row );
}
