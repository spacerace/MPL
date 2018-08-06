/* dirx.c  --  directory examples  */

#include <direct.h>
#include <stdio.h>

#define SUBDIR "SUBDIR"
#define SUBSUBDIR "SUBSUB"

main()
{
    char *current_dir;
    void Err();

    if ((current_dir = getcwd(NULL, 0)) == NULL)
        Err("getcwd()", "Can't get current directory.");

    if (mkdir(SUBDIR) != 0)
        Err( SUBSUBDIR, "Can't make directory." );

    if (chdir(SUBDIR) != 0)
        Err( SUBDIR, "Can't cd into directory." );

    if (mkdir(SUBSUBDIR) != 0)
        Err( SUBSUBDIR, "Can't make directory." );

    if (chdir(current_dir) != 0)
        Err( SUBDIR, "Can't cd back to." );

    if (rmdir(SUBDIR) != 0)
        Err( SUBDIR, "Can't remove directory." );

}

void Err(char *what, char *msg)
{
    fprintf(stderr, "\"%s\": %s\n", what, msg );
    exit (1);
}
