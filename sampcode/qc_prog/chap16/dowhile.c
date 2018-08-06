/* dowhile.c -- misuse of do while loop     */

main()
{
    int i = 0;

    do while (i < 10)
	{
        printf("Happy Fourth of July!\n");
        i++;
	}
    printf("VOOOM\n");
}

