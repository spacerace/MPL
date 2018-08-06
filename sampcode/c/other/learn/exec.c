/* EXEC.C illustrates the different versions of exec including:
 *      execl           execle          execlp          execlpe
 *      execv           execve          execvp          execvpe
 *
 * Although EXEC.C can exec any program, you can verify how different
 * versions handle arguments and environment by compiling and
 * specifying the sample program ARGS.C. See SPAWN.C for examples
 * of the similar spawn functions.
 */

#include <stdio.h>
#include <conio.h>
#include <process.h>

char *my_env[] =                /* Environment for exec?e */
{
    "THIS=environment will be",
    "PASSED=to child by the",
    "SPAWN=functions",
    NULL
};

main()
{
    char *args[4], prog[80];
    int ch;

    printf( "Enter name of program to exec: " );
    gets( prog );
    printf( " 1. execl   2. execle   3. execlp   4. execlpe\n" );
    printf( " 5. execv   6. execve   7. execvp   8. execvpe\n" );
    printf( "Type a number from 1 to 8 (or 0 to quit): " );
    ch = getche();
    if( (ch < '1') || (ch > '8') )
        exit( 1 );
    printf( "\n\n" );

    /* Arguments for execv? */
    args[0] = prog;             /* First argument ignored under most */
    args[1] = "exec??";         /*   recent versions of DOS          */
    args[2] = "two";
    args[3] = NULL;

    switch( ch )
    {
        case '1':
            execl( prog, prog, "execl", "two", NULL );
            break;
        case '2':
            execle( prog, prog, "execle", "two", NULL, my_env );
            break;
        case '3':
            execlp( prog, prog, "execlp", "two", NULL );
            break;
        case '4':
            execlpe( prog, prog, "execlpe", "two", NULL, my_env );
            break;
        case '5':
            execv( prog, args );
            break;
        case '6':
            execve( prog, args, my_env );
            break;
        case '7':
            execvp( prog, args );
            break;
        case '8':
            execvpe( prog, args, my_env );
            break;
        default:
            break;
    }

    /* This point is only reached if exec fails. */
    printf( "\nProcess was not execed." );
    exit( 0 );
}
