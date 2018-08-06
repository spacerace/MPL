/* WRAP.C illustrates:
 *      _wrapon
 */

#include <conio.h>
#include <graph.h>

main()
{
    _wrapon( _GWRAPON );
    while( !kbhit() )
        _outtext( "Wrap on! " );

    getch();
    _clearscreen( _GCLEARSCREEN );
    _wrapon( _GWRAPOFF );
    while( !kbhit() )
        _outtext( "Wrap off!" );
    getch();
}
