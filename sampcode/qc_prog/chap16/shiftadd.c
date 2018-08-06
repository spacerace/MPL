/* shiftadd.c -- shift and add numbers               */

main()
{
    int x = 0x12;
    int y;

    y = x << 8 + 2;
    printf("y is 0x%x\n", y);
}

