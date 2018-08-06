/* pack.c  --  demonstrates structure packing with */
/*             the #pragma pack() directive        */

#pragma pack(4)        /* 1, 2 or 4 */

main()
{
    struct {
        char ch1;
        int  int1;
        char ch2;
        long int2;
    } s;

    printf("ch1  -> %lu\n", (unsigned long)(&s.ch1));
    printf("int1 -> %lu\n", (unsigned long)(&s.int1));
    printf("ch2  -> %lu\n", (unsigned long)(&s.ch2));
    printf("int2 -> %lu\n", (unsigned long)(&s.int2));
}
