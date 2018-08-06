/* PAGE.C illustrates video page functions including:
 *      _getactivepage  _getvisualpage  _setactivepage  _setvisualpage
 */

#include <conio.h>
#include <graph.h>
#include <time.h>

void delay( clock_t wait );         /* Prototype */
char *jumper[] = { { "\\o/\n O \n/ \\" },
                   { "_o_\n O \n( )"   },
                   { " o \n/O\\\n/ \\" },
                   { " o \n O \n( )"   }
                 };
main()
{
    int i, oldvpage, oldapage, oldcursor;

    _clearscreen( _GCLEARSCREEN );
    oldcursor = _displaycursor( _GCURSOROFF );
    oldapage  = _getactivepage();
    oldvpage  = _getvisualpage();

    /* Draw image on each page */
    for( i = 0; i < 4; i++ )
    {
        _setactivepage( i );
        _settextposition( 1, 1 );
        _outtext( jumper[i] );
    }

    while( !kbhit() )
        /* Cycle through pages 0 to 3 */
        for( i = 0; i < 4; i++ )
        {
            _setvisualpage( i );
            delay( 100L );
        }
    getch();

    /* Restore original page and cursor. */
    _displaycursor( oldcursor );
    _setactivepage( oldapage );
    _setvisualpage( oldvpage );
}

/* delay - Pauses for a specified number of microseconds. */
void delay( clock_t wait )
{
    clock_t goal;

    goal = wait + clock();
    while( goal > clock() )
        ;
}
