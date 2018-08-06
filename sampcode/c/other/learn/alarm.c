/* ALARM.C illustrates inline assembly and functions or keywords
 * related to Terminate-and-Stay-Resident programs. Functions include:
 *      _dos_setvect    _dos_getvect    _dos_keep
 *      _chain_intr     _enable         _disable
 *
 * Keywords:
 *      interrupt       _asm
 * Directive:
 *      #pragma
 * Pragma:
 *      check_stack     check_pointer
 * Global variables:
 *      _psp            _amblksiz
 *
 * WARNING: You must run ALARM from the DOS command line. The QC
 * environment does not permit TSRs to be installed from inside it, since
 * this would cause subsequent memory problems.
 *
 * See HARDERR.C for another example of inline assembler. See MOVEMEM.C
 * for another pragma example.
 */

#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Stack and pointer checking off */
#pragma check_stack( off )
#pragma check_pointer( off )

/* Inline assembler macro to sound a bell. Note that comments in macros must
 * be in the C format, not the assembler format.
 */
#define BEEP() _asm { \
                        _asm     push bx        /* Save register   */ \
                        _asm     sub  bx, bx    /* Page 0          */ \
                        _asm     mov  ax, 0E07h /* TTY bell        */ \
                        _asm     int  10h       /* BIOS 10         */ \
                        _asm     pop  bx        /* Restore         */ \
                    }

#define TICKPERMIN  1092L
#define MINPERHOUR  60L
enum BOOLEAN { FALSE, TRUE };

/* Prototypes for interrupt functions */
void (interrupt far *oldtimer)( void );
void (interrupt far *oldvideo)( void );
void interrupt far newtimer( void );
void interrupt far newvideo( unsigned _es, unsigned _ds, unsigned _di,
                             unsigned _si, unsigned _bp, unsigned _sp,
                             unsigned _bx, unsigned _dx, unsigned _cx,
                             unsigned _ax, unsigned _ip, unsigned _cs,
                             unsigned _flags );

/* Variables that will be accessed inside TSR must be global. */
int  ftimesup = FALSE, fintimer = FALSE, finvideo = FALSE;
long goaltick;
long far *pcurtick = (long far *)0x0000046cL;

/* Huge pointers force compiler to do segment arithmetic for us. */
char huge *tsrstack;
char huge *appstack;
char huge *tsrbottom;

main( int argc, char **argv )
{
    long minute, hour;
    unsigned tsrsize;

    /* Initialize stack and bottom of program. */
    _asm mov  WORD PTR tsrstack[0], sp
    _asm mov  WORD PTR tsrstack[2], ss
    FP_SEG( tsrbottom ) = _psp;
    FP_OFF( tsrbottom ) = 0;

    /* Use 16 paragraph heap (controlled through global in malloc.h). */
    _amblksiz = 256;

    /* Program size is:
     *     top of stack
     *   - bottom of program (converted to paragraphs)
     *   + paragraphs in the heap plus
     *   + one extra paragraph just to be safe
     */
    tsrsize = ((tsrstack - tsrbottom) >> 4) + (_amblksiz >> 4) + 1;

    /* If command-line, convert time to ticks past midnight. Time must
     * include 0 in first place (0930, not 930). Time must be later than
     * current time.
     */
    if( argc < 2 )
    {
        puts( "  Syntax: ALARM <hhmm> " );
        puts( "     where <hhmm> is time (in military format) to ring alarm" );
        exit( 1 );
    }

    minute = atol( argv[1] + 2 );
    argv[1][2] = 0;
    hour = atol( argv[1] );
    goaltick = (hour * MINPERHOUR * TICKPERMIN) + (minute * TICKPERMIN);
    if( *pcurtick > goaltick )
    {
        puts( "It's past that time now" );
        exit( 1 );
    }

    /* Replace existing timer and video routines with ours. */
    oldtimer = _dos_getvect( 0x1c );
    _dos_setvect( 0x1c, newtimer );
    oldvideo = _dos_getvect( 0x10 );
    _dos_setvect( 0x10, newvideo );

    /* Free the PSP segment and terminate with program resident. */
    _dos_freemem( _psp );
    _dos_keep( 0, tsrsize );
}

/* newtimer - Our timer interrupt compares current time to goal. If earlier
 * it just continues. If later, it beeps and sets a flag to quit checking.
 */
void interrupt far newtimer()
{
    if( ftimesup )
        _chain_intr( oldtimer );
    else
    {
        /* First execute the original timer interrupt. */
        (*oldtimer)();
        if( *pcurtick > goaltick )
        {
            /* If time is up, set flag so we'll never return. */
            ftimesup = TRUE;

            /* Save current stack of application, and set old stack of TSR.
             * This is for safety since we don't know the state of the
             * application stack, but we do know the state of our own stack.
             * Turn off interrupts during the stack switch.
             */
            _disable();
            _asm \
            {
                mov  WORD PTR appstack[0], sp   ; Save current stack
                mov  WORD PTR appstack[2], ss
                mov  sp, WORD PTR tsrstack[0]   ; Load new stack
                mov  ss, WORD PTR tsrstack[2]
            }
            _enable();

            /* Make sure we're not in video interrupt, then BEEP. This check
             * prevents the rare but potentially dangerous case of
             * calling INT 10 to beep while INT 10 is already running.
             */
            while( finvideo )
                ;
            BEEP();
            BEEP();
            BEEP();

            /* Restore application stack. */
            _disable();
            _asm \
            {
                mov  sp, WORD PTR appstack[0]
                mov  ss, WORD PTR appstack[2]
            }
            _enable();
        }
    }
}

/* newvideo - This routine protects against reentering INT 10 while it is
 * already executing. This could be disastrous if the interrupt routine was
 * interrupted while it was accessing a hardware register.
 */
void interrupt far newvideo( unsigned _es, unsigned _ds, unsigned _di,
                             unsigned _si, unsigned _bp, unsigned _sp,
                             unsigned _bx, unsigned _dx, unsigned _cx,
                             unsigned _ax, unsigned _ip, unsigned _cs,
                             unsigned _flags )
{
    static unsigned save_bp;

    /* If not already in (most of the time), chain to original. */
    if( !finvideo )
        _chain_intr( oldvideo );
    else
    {

        /* Set the inside flag, then make sure all the real registers
         * that might be passed to an interrupt 10h match the parameter
         * registers. Some of the real registers may be modified by the
         * preceding code. Note that BP must be saved in a static (nonstack)
         * variable so that it can be retrieved without modifying the stack.
         */
        finvideo = TRUE;
        _asm \
        {
            mov ax, _ax
            mov bx, _bx
            mov cx, _cx
            mov dx, _dx
            mov es, _es
            mov di, _di
            mov save_bp, bp
            mov bp, _bp
        }

        /* Call the original interrupt. */
        (*oldvideo)();

        /* Make sure that any values returned in real registers by the
         * interrupt are updated in the parameter registers. Reset the flag.
         */
        _asm \
        {
            mov bp, save_bp
            mov _bp, bp
            mov _di, di
            mov _es, es
            mov _dx, dx
            mov _cx, cx
            mov _bx, bx
            mov _ax, ax
        }
        finvideo = FALSE;
    }
}
