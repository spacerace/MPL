/* ARGS.C illustrates the following variables used for accessing
 * command-line arguments and environment variables:
 *      argc            argv            envp
 *
 * Also illustrates getting a process ID with:
 *      getpid
 */

#include <stdio.h>
#include <process.h>

main( int argc,             /* Number of strings in array argv          */
      char *argv[],         /* Array of command-line argument strings   */
      char **envp )         /* Array of environment variable strings    */
{
    int count;

    /* Display each command-line argument. */
    printf( "\nCommand-line arguments:\n" );
    for( count = 0; count < argc; count++ )
        printf( "  argv[%d]   %s\n", count, argv[count] );

    /* Display each environment variable. */
    printf( "\nEnvironment variables:\n" );
    while( *envp != NULL )
        printf( "  %s\n", *(envp++) );

    /* If run from DOS, shows different ID for DOS than for DOS shell.
     * If execed or spawned, shows ID of parent.
     */
    printf( "\nProcess id of parent: %d", getpid() );
    exit( 0 );
}
