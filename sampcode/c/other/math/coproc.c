/* COPROC.C illustrates use of the status and control words of a floating
 * point coprocessor (or emulator). Functions illustrated include:
 *      _clear87            _status87           _control87
 */

#include <stdio.h>
#include <conio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

double dx = 1e-40, dy;
float fx, fy;
unsigned status, control;
char tmpstr[20];
char *binstr( int num, char *buffer );

main()
{
    printf( "Status Word Key:\n" );
    printf( "B\tBusy flag\n0-3\tCondition codes\nS\tStack top pointer\n" );
    printf( "E\tError summary\nF\tStack flag\nP\tPrecision exception\n" );
    printf( "U\tUnderflow exception\nO\tOverflow exception\n" );
    printf( "Z\tZero divide exception\nD\tDenormalized exception\n" );
    printf( "I\tInvalid operation exception\n\n" );

    binstr(  _clear87(), tmpstr );
    printf( "B3SSS210EFPUOZDI  Function\tCondition\n\n" );
    printf( "%16s  _clear87\tAfter clearing\n", tmpstr );

    /* Storing double to float that hasn't enough precision for it
     * causes underflow and precision exceptions.
     */
    fx = dx;
    binstr(  _status87(), tmpstr );
    printf( "%16s  _status87\tAfter moving double to float\n", tmpstr );

    /* Storing float with lost precision back to double adds denormalized
     * exception (previous exceptions remain).
     */
    dy = fx;
    binstr(  _clear87(), tmpstr );
    printf( "%16s  _clear87\tAfter moving float to double\n", tmpstr );

    /* Using clear87() erases previous exceptions. */
    fy = dy;
    binstr(  _status87(), tmpstr );
    printf( "%16s  _status87\tAfter moving double to float\n\n", tmpstr );

    getch();
    printf( "Control Word Key:\n" );
    printf( "i\tInfinity control\nr\tRounding control\n" );
    printf( "p\tPrecision control\ne\tInterrupt enable mask\n" );
    printf( "U\tUnderflow mask\nO\tOverflow mask\n" );
    printf( "Z\tZero divide mask\nD\tDenormalized mask\n" );
    printf( "I\tInvalid operation mask\n\n" );
    printf( "???irrppe?PUOZDI  Result\n" );
    fy = .1;

    /* Show current control word. */
    binstr( _control87( 0, 0 ), tmpstr );
    printf( "%16s  %.1f * %.1f = %.15e with initial precision\n",
            tmpstr, fy, fy, fy * fy );

    /* Set precision to 24 bits. */
    binstr( _control87( PC_24, MCW_PC ), tmpstr );
    printf( "%16s  %.1f * %.1f = %.15e with 24 bit precision\n",
            tmpstr, fy, fy, fy * fy );

    /* Restore default. */
    binstr( _control87( CW_DEFAULT, 0xffff ), tmpstr );
    printf( "%16s  %.1f * %.1f = %.15e with default precision\n",
            tmpstr, fy, fy, fy * fy );
}

/* Convert integer to string of 16 binary characters. */
char *binstr( int num, char *buffer )
{
    char tmp[17];
    int  len;

    memset( buffer, '0', 16 );
    len = strlen( itoa( num, tmp, 2 ) );
    strcpy( buffer + 16 - len, tmp );
    return buffer;
}
