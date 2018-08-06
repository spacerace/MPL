/* box.c  --  demonstrate the result of initializing  */
/*            a three-dimensional array               */

main()
{
    static int cube[3][3][3] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 25, 26, 27 };
    int plane;
    extern void Draw_plane();

    for (plane = 0; plane < 3; ++plane)
        {
        Draw_plane(cube, plane);
        }
}

void Draw_plane(int box[3][3][3], int slice)
    {
    int row, col;

    printf("Plane[%d] =\n", slice);
    for (row = 0; row < 3; ++row)
        {
        for (col = 0; col < 3; ++col)
            {
            printf( "%2d ", box[slice][row][col]);
            }
        printf("\n");
        }
    printf("\n");
}
