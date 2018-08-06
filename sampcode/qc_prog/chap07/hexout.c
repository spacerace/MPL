/* hexout.c --  print a floating point variable in */
/*              hexadecimal format                 */

extern void Hexout();

main()
{
    float fary[1];

    printf("Enter a floating point number\n");
    printf("(Any nonnumeric entry quits)\n\n");
    while (scanf("%f", &fary[0]) == 1)
        {
        Hexout(fary);
        }
    return (0);
}

void Hexout(unsigned char chary[])
{
    int i;

    for (i = 0; i < sizeof(float); ++i)
        {
        printf("%02X ", chary[i]);
        }
    printf("\n\n");
}
