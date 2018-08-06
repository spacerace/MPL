/* hello.c  --  legal ways to initialize strings as */
/*              arrays of char values               */

char Gphrase[] = { 
    'H','e','l','l','o','\n','\0' };    /* global initialization */

main()
{
    static char gphrase[] = {
        'h','e','l','l','o','\n','\0' };    /* local initialization */

    printf("Global: %s\n", Gphrase);
    printf("Local:  %s\n", gphrase);

}
