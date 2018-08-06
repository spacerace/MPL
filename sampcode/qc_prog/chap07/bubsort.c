/* bubsort.c  --  passing an array to a function  */
/*                affects the original array      */

#define NUMINTS 6

extern void Bub_sort();
extern void Triple();

main()
{
    int num = 2, i;
    static int list[NUMINTS] = { 6, 5, 4, 3, 2, 1 };

    printf("num before Triple = %d\n", num);
    Triple(num);
    printf("num after Triple  = %d\n", num);
    printf("list[0] before Triple = %d\n", list[0]);
    Triple(list[0]);
    printf("list[0] after Triple  = %d\n", list[0]);

    printf("Before sorting -> ");
    for (i = 0; i < NUMINTS; ++i)
        {
        printf("%d ", list[i]);
        }
    printf("\n");

    Bub_sort(list);
    printf("After sorting ->  ");
    for (i = 0; i < NUMINTS; ++i)
        {
        printf("%d ", list[i]);
        }
    printf("\n");

}

void Triple(int x)  /* function doesn't affect original */
{
    x *= 3;
}

void Bub_sort(int vals[NUMINTS]) /* function changes original */
{
    int i, j, temp;

    for (i = (NUMINTS - 1); i > 0; --i)
        {
        for (j = 0; j < i; ++j)
            {
            if (vals[j] > vals[j+1])
                {
                temp      = vals[j];
                vals[j]   = vals[j+1];
                vals[j+1] = temp;
                }
            }
        }
}
