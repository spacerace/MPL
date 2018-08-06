/* xmas.c -- fills an array with values, then passes */
/*           each of those values to a function.     */

main()
{
    int i, j, widths[20];
    void Center_out();

    for (i = 0, j = 1; i < 18; ++i, j += 2)
        {
        widths[i] = j;
        }
    widths[i++] = 3;
    widths[i] = 3;

    for (i = 0; i < 20; i++)
        {
        Center_out('X', widths[i]);
        }

}

void Center_out(char character, int width)
{
    int i;

    for (i = 0; i < ((80 - width) / 2); ++i)
        {
        putch(' ');
        }
    for (i = 0; i < width; ++i)
        {
        putch(character);
        }
    putch('\r');
    putch('\n');
}
