/*                      Change Border Color

        This program changes the background color/attributes.
*/

/*      main(argc,argv)

        Function: Change the color of the border of the display to that
        given as a command line parameter.

        Algorithm: Check the number of parameters. If there is one, then
        convert it to a number using atoi, and pass that to the setPal
        function to actually set the border color.

        Comments: This program will do nothing with a non-color display
        adapter.
*/

main(argc,argv)

int argc;
char *argv[];

{
        /* Check for the right number of parameters. */
        if (argc != 2) {
                /* If wrong, tell the user and exit. */
                puts("Usage: border <border color>");
                exit(1);
        };

        /* Convert the parameter to a number and pass it to setPal. */
        setPal(0,atoi(argv[1]));
}

