/* PRINTF.C illustrates output formatting with:
 *      printf
 *
 * The rules for formatting also apply to cprintf, sprintf, vfprintf,
 * vprintf, and vsprintf. For other examples of printf formatting,
 * see EXTDIR.C (sprintf), VPRINTF (vprintf), TABLE.C (fprintf),
 * ROTATE.C (printf), and IS.C (cprintf).
 */

#include <stdio.h>

main()
{
    char ch = 'h', *string = "computer";
    int count = 234, *ptr, hex = 0x10, oct = 010, dec = 10;
    double fp = 251.7366;

    /* Display integers. */
    printf("%d    %+d    %06d    %X    %x    %o\n\n",
            count, count, count, count, count, count );

    /* Count characters printed. */
    printf( "            V\n" );
    printf( "1234567890123%n45678901234567890\n", &count );
    printf( "Number of characters printed: %d\n\n", count );

    /* Display characters. */
    printf( "%10c%5c\n\n", ch, ch );

    /* Display strings. */
    printf( "%25s\n%25.4s\n\n", string, string );

    /* Display real numbers. */
    printf( "%f    %.2f    %e    %E\n\n", fp, fp, fp, fp );

    /* Display in different radixes. */
    printf( "%i    %i    %i\n\n", hex, oct, dec );

    /* Display pointers. */
    ptr = &count;
    printf( "%Np    %p    %Fp\n", ptr, (int far *)ptr, (int far *)ptr );
}
