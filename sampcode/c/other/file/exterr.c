/* EXTERR.C illustrates function:
 *      dosexterr
 */

#include <dos.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void errorinfo( void );     /* Prototype */

main( int argc, char *argv[] )
{
    int hsource;

    /* Open to get file handle and test for errors. Try specifying
     * invalid files to show different errors.
     */
    if( _dos_open( argv[1], O_RDWR, &hsource ) )
        errorinfo();
    printf( "No error\n" );
    _dos_close( hsource );
    exit( 0 );
}

void errorinfo()
{
    struct DOSERROR err;
    static char *eclass[] =
    {
        "", "Out of Resource", "Temporary Situation", "Authorization",
        "Internal", "Hardware Failure", "System Failure", "Application Error",
        "Not Found", "Bad Format", "Locked", "Media", "Already Exists",
        "Unknown"
    };
    static char *eaction[] =
    {
        "", "Retry", "Delay Retry", "User", "Abort", "Immediate Exit",
        "Ignore", "Retry After User Intervention"
    };
    static char *elocus[] =
    {
        "", "Unknown", "Block Device", "Net", "Serial Device", "Memory"
    };


    /* Get error information and display class, action, and locus. */
    dosexterr( &err );
    printf( "Class:\t%s\nAction:\t%s\nLocus:\t%s\nAction\t",
            eclass[err.class], eaction[err.action], elocus[err.locus] );

    /* Errors that could be caused by sample _dos_open. You can expand
     * this list to handle others.
     */
    switch( err.exterror )
    {
        case 2:
            printf( "File not found\n" );
            break;
        case 3:
            printf( "Path not found\n" );
            break;
        case 5:
            printf( "Access denied\n" );
            break;
    }
    exit( err.exterror );
}
