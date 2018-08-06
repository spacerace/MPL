
/* INFLATE.C -- shows multiple initialization */
/*              and calculations in for loop  */

main()
{
    int year;
    float value, rate;
    printf("What do you think the inflation rate will be?");
    scanf("%f", &rate);
    printf("If the dollar is worth 100 cents in 1987\n");
    printf("and the inflation rate is %2.2f, then:\n", rate);

    for (year=1988, value = 1.0; year <=1999;
         value *= (1.0 - rate),
         printf("in %d the dollar will be worth", year),
         printf(" %2.0f cents\n", value * 100),
         ++year
         );
}

