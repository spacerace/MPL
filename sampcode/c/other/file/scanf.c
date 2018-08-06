/* SCANF.C illustrates formatted input. Functions illustrated include:
 *          scanf           fflush          flushall
 *
 * For other examples of formatted input, see TABLE.C (fscanf) and
 * SETTIME.C (sscanf).
 */

#include <stdio.h>
#include <conio.h>

main()
{
    int result, integer;
    float fp;
    char string[81];

    /* Get numbers. */
    printf( "Enter an integer and a floating point number: " );
    scanf( "%d %f", &integer, &fp );
    printf( "%d + %f = %f\n\n", integer, fp, integer + fp );

    /* Read each word as a string. */
    printf( "Enter a sentence of four words with scanf: " );
    for( integer = 0; integer < 4; integer++ )
    {
        scanf( "%s", string );
        printf( "%s\n", string );
    }

    /* Clear the input buffer and get with gets. */
    fflush( stdin );
    printf( "Enter the same sentence with gets: " );
    gets( string );
    printf( "%s\n", string );

    /* Specify delimiters. The ^ inside the brackets says accept any
     * character except the following other characters in brackets (tab
     * and newline in the example).
     */
    printf( "Enter the same sentence with scanf and delimiters: " );
    scanf( "%[^\n\t]s", string );
    printf( "%s\n", string );

    /* Loop until input value is 0. */
    printf( "\n\nEnter numbers in C decimal (num), hex (0xnum), " );
    printf( "or octal (0num) format.\nEnter 0 to quit\n\n" );
    do
    {
        printf( "Enter number: " );
        result = scanf( "%i", &integer );
        if( result )
            /* Display valid integers. */
            printf( "Decimal: %i  Octal: 0%o  Hexadecimal: 0x%X\n\n",
                    integer, integer, integer );
        else
        {   /* Read invalid characters. Then flush and continue. */
            scanf( "%s", string );
            printf( "Invalid number: %s\n\n", string );
            flushall();
            integer = 1;
        }
    } while( integer );
}
