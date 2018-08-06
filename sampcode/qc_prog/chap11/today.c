/* today.c  -- demonstrates using enum  */

main()
{
    enum week_days {
        monday = 1,    /* start with 1 */
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday
    } pay_day;

    static char *day_names[] = {
        "",
        "monday",
        "tuesday",
        "wednesday",
        "thursday",
        "friday",
        "saturday",
        "sunday"
    };

    printf("What day do you want to be paid on?\n");

    for (pay_day = monday; pay_day <= sunday; ++pay_day)
        {
        printf("%d. %s\n", pay_day, day_names[pay_day]);
        }
    
    printf("Which (%d-%d): ", monday, sunday);

    do
        {
        pay_day = getch();
        pay_day -= '0';
        } while (pay_day < monday || pay_day > sunday);

    printf("%d\n\n", pay_day);

    printf("You selected %s\n", day_names[pay_day]);

}
