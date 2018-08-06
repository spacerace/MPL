/* continue.c -- shows CONTINUE in loop */

main()
{
    int sw = 0;
    char ch;
    while (1) /* endless loop */
    {
        /* print current status */
        if (sw)
            printf("\nSwitch is ON\n");
        else
            printf("\nSwitch is OFF\n");

        printf("Do you want to quit? ");
        if (ch = getche() == 'y')
            break;    /* exit loop on yes */

        printf("\nDo you want to toggle the switch? ");
        if (ch = getche() != 'y')
            continue; /* restart loop on yes */

        sw = !sw;     /* toggle switch */
        }
}
