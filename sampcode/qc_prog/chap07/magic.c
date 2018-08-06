/* magic.c  --  demonstrate use of a two-dimensional  */
/*              array of type int                     */

main()
{
    static int square[3][3] = { 5, 8, 3, 4, 2, 0, 7, 1, 6 };
    int zrow = 1, zcol = 2;  /* location of the zero */
    int num, row, col, i , j, rowdist, coldist;

    while (1)
        {
        printf("Swap what with zero?\n");
        printf("(Q to quit)\n");

        /* Print the square. */
        for (i = 0; i < 3; ++i)
            {
            for (j = 0; j < 3; ++j)
                {
                printf(" %d ", square[i][j] );
                }
            printf("\n");
            }

        /* Input the user number. */
        if ((num = getch()) == 'Q')
            exit(0);
        num -= '0';
        if (num < 1 || num > 9)
            {
            printf("Not a legal number.\n\n");
            continue;
            }

        /* Find that square. */
        for (row = 0; row < 3; ++row)
            {
            for(col = 0; col < 3; ++col)
                {
                if (num == square[row][col])
                    {
                    goto GOTIT;
                    }
                }
            }
GOTIT:
        /* Check for a legal move. */
        if (row > 2 || col > 2)
            {
            printf("Bad Box Specification\n\n");
            continue;
            }
        rowdist = zrow - row;
        if (rowdist < 0) 
            rowdist *= -1;
        coldist = zcol - col;
        if (coldist < 0) 
            coldist *= -1;
        if (rowdist > 1 || coldist > 1)
            {
            printf("Not A Neighbor\n\n");
            continue;
            }

        /* Make the move. */
        square[zrow][zcol] = square[row][col];
        square[row][col] = 0;
        zrow = row;
        zcol = col;

        /* See if done, and solved. */
        for (i = 0; i < 3; ++i)
            {
            for (j = 0; j < 3; ++j)
                {
                if (square[i][j] != ((i * 3) + j))
                    {
                    break;
                    }
                }
            }
        if ((i * j) == 9)
            break;
        }
    printf("\n\aYOU GOT IT !!!\n");
}
