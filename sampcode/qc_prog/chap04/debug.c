
/* DEBUG.C -- for practice with debugger */

main()
{
    char response;
    int number, max_numbers = 5, count = 0, total = 0;
    float average;

    printf("Continue (y/n)? ");
    response = getche();
    while ((response != 'n') && (count < max_numbers))
        printf("\nEnter a number: ");
        scanf("%d", &number);
        total += number;
        printf("Continue (y/n)? ");
        response = getche();
    average = total / count;
    printf("\nTotal is %d\n", total);
    printf("Average is %f\n", average);
}

