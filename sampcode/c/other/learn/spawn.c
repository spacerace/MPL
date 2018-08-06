/* SPAWN.C illustrates the different versions of spawn including:
 *      spawnl          spawnle         spawnlp         spawnlpe
 *      spawnv          spawnve         spawnvp         spawnvpe
 *
 * Although SPAWN.C can spawn any program, you can verify how different
 * versions handle arguments and environment by compiling and
 * specifying the sample program ARGS.C. See EXEC.C for examples
 * of the similar exec functions.
 */

#include <stdio.h>
#include <conio.h>
#include <process.h>

char *my_env[] =                        /* Environment for spawn?e */
{
    "THIS=environment will be",
    "PASSED=to child by the",
    "SPAWN=functions",
    NULL
};

main()
{
    char *args[4], prog[80];
    int ch, r;

    printf( "Enter name of program to spawn: " );
    gets( prog );
    printf( " 1. spawnl   2. spawnle   3. spawnlp   4. spawnlpe\n" );
    printf( " 5. spawnv   6. spawnve   7. spawnvp   8. spawnvpe\n" );
    printf( "Type a number from 1 to 8 (or 0 to quit): " );
    ch = getche();
    if( (ch < '1') || (ch > '8') )
        exit( -1 );
    printf( "\n\n" );

    /* Arguments for spawnv? */
    args[0] = prog;             /* First argument ignored under most */
    args[1] = "spawn??";        /*   recent versions of DOS          */
    args[2] = "two";
    args[2] = "two";
    args[3] = NULL;

    switch( ch )
    {
        case '1':
            r = spawnl( P_WAIT, prog, prog, "spawnl", "two", NULL );
            break;
        case '2':
            r = spawnle( P_WAIT, prog, prog, "spawnle", "two", NULL, my_env );
            break;
        case '3':
            r = spawnlp( P_WAIT, prog, prog, "spawnlp", "two", NULL );
            break;
        case '4':
            r = spawnlpe( P_WAIT, prog, prog, "spawnlpe", "two", NULL, my_env );
            break;
        case '5':
            r = spawnv( P_WAIT, prog, args );
            break;
        case '6':
            r = spawnve( P_WAIT, prog, args, my_env );
            break;
        case '7':
            r = spawnvp( P_WAIT, prog, args );
            break;
        case '8':
            r = spawnvpe( P_WAIT, prog, args, my_env );
            break;
        default:
            break;
    }
    if( r == -1 )
        printf( "Couldn't spawn process" );
    else
        printf( "\nReturned from SPAWN!" );
    exit( r );
}
