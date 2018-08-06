/* chars.c -- shows some variables of type char */
/*            as both characters and integers   */

main()
{
    char ch1 = 'A', ch2 = 'a';

    printf("The character %c has ASCII code %d\n", ch1, ch1);
    printf("If you add ten, you get %c\n", ch1 + 10);
    printf("The character %c has ASCII code %d\n", ch2, ch2);
}
