/* BEEP.C illustrates timing and port input and output functions
 * including:
 *      inp             outp            clock
 * Also keyword:
 *      enum
 *
 * In addition to the delay use shown here, clock can be used as a
 * timer as shown in SIEVE.C.
 */

#include <time.h>
#include <conio.h>

void beep( unsigned duration, unsigned frequency );
void delay( clock_t wait );


enum notes
{   /* Enumeration of notes and frequencies     */
    C0 = 262, D0 = 296, E0 = 330, F0 = 349, G0 = 392, A0 = 440, B0 = 494,
    C1 = 523, D1 = 587, E1 = 659, F1 = 698, G1 = 784, A1 = 880, B1 = 988,
    EIGHTH = 125, QUARTER = 250, HALF = 500, WHOLE = 1000, END = 0
} song[] =
{   /* Array initialized to notes of song       */
    C1, HALF, G0, HALF, A0, HALF, E0, HALF, F0, HALF, E0, QUARTER,
    D0, QUARTER, C0, WHOLE, END
};

int main ()
{
    int note = 0;

    while( song[note] )
        beep( song[note++], song[note++] );
}

/* beep - sounds the speaker for a time specified in microseconds by
 * duration at a pitch specified in hertz by frequency.
 */
void beep( unsigned duration, unsigned frequency )
{
    int control;

    /* If frequency is 0, beep doesn't try to make a sound. It
     * just sleeps for the duration.
     */
    if (frequency)
    {
        /* 75 is the shortest reliable duration of a sound. */
        if( duration < 75 )
            duration = 75;

        /* Prepare timer by sending 10111100 to port 43. */
        outp( 0x43, 0xb6 );

        /* Divide input frequency by timer ticks per second and
         * write (byte by byte) to timer.
         */
        frequency = (unsigned)(1193180L / frequency);
        outp( 0x42, (char)frequency );
        outp( 0x42, (char)(frequency >> 8) );

        /* Save speaker control byte. */
        control = inp( 0x61 );

        /* Turn on the speaker (with bits 0 and 1). */
        outp( 0x61, control | 0x3 );
    }

    delay( (clock_t)duration );

    /* Turn speaker back on if necessary. */
    if( frequency )
        outp( 0x61, control );
}

/* delay - Pauses for a specified number of microseconds. */
void delay( clock_t wait )
{
    clock_t goal;

    goal = wait + clock();
    while( goal > clock() )
        ;
}
