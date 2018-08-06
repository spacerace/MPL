/* COPY2.C illustrates DOS file I/O and DOS memory allocation functions
 * including:
 *      _dos_open       _dos_close      _dos_creatnew   _dos_creat
 *      _dos_read       _dos_write      _dos_allocmem   _dos_free
 *
 * See COPY1.C for another version of the copyfile function.
 */

#include <dos.h>
#include <fcntl.h>
#include <conio.h>
#include <stdio.h>

int copyfile( char *source, char *destin );     /* Prototype */

main( int argc, char *argv[] )
{
    if( argc == 3 )
        if( copyfile( argv[1], argv[2] ) )
            printf( "Copy failed\n" );
        else
            printf( "Copy successful\n" );
    else
        printf( "  SYNTAX: COPY2 <source> <target>\n" );
}

/* Function to copy one file to another (both specified by path).
 * Dynamically allocates memory for the file buffer. Prompts before
 * overwriting existing file. Returns 0 if successful, or an error
 * number if unsuccessful. This function uses dos functions only; no
 * standard C library functions are used.
 */
#define EXIST 80
enum ATTRIB { NORMAL, RDONLY, HIDDEN, SYSTEM = 4 };
int copyfile( char *source, char *target )
{
    char far *buf = NULL;
    static char prompt[] = "Target exists. Overwrite? ", newline[] = "\n\r";
    int hsource, htarget, ch;
    unsigned ret, segbuf, count;

    /* Attempt to dynamically allocate all of memory (0xffff paragraphs).
     * This will fail, but will return the amount actually available
     * in segbuf. Then allocate this amount.
     */
    _dos_allocmem( 0xffff, &segbuf );
    count = segbuf;
    if( ret = _dos_allocmem( count, &segbuf ) )
        return ret;
    FP_SEG( buf ) = segbuf;

    /* Open source file and create target, overwriting if necessary. */
    if( ret = _dos_open( source, O_RDONLY, &hsource ) )
        return ret;
    ret = _dos_creatnew( target, NORMAL, &htarget );
    if( ret == EXIST )
    {
        /* Use _dos_write to display prompts. Use bdos to call
         * function 1 to get and echo keystroke.
         */
        _dos_write( 1, prompt, sizeof( prompt ) - 1, &ch );
        ch = bdos( 1, 0, 0 ) & 0x00ff;
        if( (ch == 'y') || (ch == 'Y') )
            ret = _dos_creat( target, NORMAL, &htarget );
        _dos_write( 1, newline, sizeof( newline ) - 1, &ch );
    }
    if( ret )
        return ret;

    /* Read and write until there is nothing left. */
    while( count )
    {
        /* Read and write input. */
        if( (ret = _dos_read( hsource, buf, count, &count )) )
            return ret;
        if( (ret = _dos_write( htarget, buf, count, &count )) )
            return ret;
    }

    /* Close files and free memory. */
    _dos_close( hsource );
    _dos_close( htarget );
    _dos_freemem( segbuf );
    return 0;
}
