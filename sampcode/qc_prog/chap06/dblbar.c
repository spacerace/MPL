/* dblbar.c -- prints header using */
/*             line() function     */

#define DOUBLE_BAR 205

main()
{
    void line();       /* declare line() function */

    line(10);          /* call line() function    */
    printf("dblbar.c -- prints header using\n");
    printf("line() function\n");
    line(50);          /* call line() again       */
}

void line()            /* function definition     */
{
    int pos;
    for (pos = 1; pos <= 40; pos++)
        putch(DOUBLE_BAR);
    printf("\n");
}
