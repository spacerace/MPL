/* SETROWS.C illustrates
 *      _settextrows
 */

#include <graph.h>
#include <stdlib.h>

main( int argc, char ** argv )
{
    struct videoconfig vc;
    short rows = atoi( argv[1] );

    _getvideoconfig( &vc );

    /* Make sure new rows is valid and the same as requested rows. */
    if( !rows || (_settextrows( rows ) != rows) )
        _outtext( "\nSyntax: SETROWS [ 25 | 43 | 50 ]\n" );

    /* Always return old rows for batch file testing. */
    exit( vc.numtextrows );
}
