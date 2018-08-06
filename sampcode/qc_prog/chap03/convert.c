/* convert.c -- converts Farenheit temprature       */
/*              to Centigrade; gets value from user */

main()
{
    float ftemp, ctemp;

    printf("What is the temprature in Farenheit? ");
    scanf("%f", &ftemp);
    ctemp = (ftemp - 32.0) * 5 / 9.0;

    printf("The temprature in Centigrade is %5.2f", ctemp);
}
