/* formats.c -- shows what happens when format */
/*              doesn't match data type        */

main()
{
    int i = 5;
    printf("As integer:      %d\n", i);
    printf("As long integer: %ld\n", i);
    printf("As exponential:  %e\n", i);
    printf("As float:        %f\n", i);
}
