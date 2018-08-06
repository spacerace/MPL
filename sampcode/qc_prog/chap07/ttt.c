/* ttt.c    --  a tic-tac-toe game demonstrates  */
/*              passing two-dimensional arrays   */
/*              to functions                     */

main()
{
    static char board[3][3] = {
        { '-', '-', '-' },
        { '-', '-', '-' },
        { '-', '-', '-' },
    };
    int row, col, ch;
    extern char Check_winner();
    extern void Make_move(), Draw_field();

    printf("You are X and make the first move.\n");
    while (1)
        {
        printf("Specify coordinate for your X.\n");
        printf("(eg, as a2, or Q to quit)\n");

        /* Print the square. */
        Draw_field(board);

        /* Input the user's coordinates. */
        if ((row = getch()) == 'Q')
            exit(0);
        row -= 'a';
        col = getch() - '1';

        /* Check for a legal move. */
        if (row < 0 || row > 2 || col < 0 || col > 2)
            {
            printf("Bad Square Specification\n\n");
            continue;
            }
        if (board[row][col] != '-')
            {
            printf("Sorry, Square Occupied\n\n");
            continue;
            }

        /* Make the move. */
        board[row][col] = 'X';
        if ((ch = Check_winner(board)) != '-' || ch == 't')
            break;
        Make_move(board);
        if ((ch = Check_winner(board)) != '-' || ch == 't')
            break;
        }
    Draw_field(board);
    if (ch == 't')
        printf("It's a tie!\n");
    else if (ch == 'X')
        printf("You win!\n");
    else
        printf("I win!\n");
}

char Check_winner(char field[][3])
{
    int row, col;

    for (row = col = 0; row < 3; ++row, ++col)
        {
        if (field[row][0] != '-'             /* horizontal */
                && field[row][0] == field[row][1]
                && field[row][1] == field[row][2] )
            {
            return(field[row][0]);
            }
        if (field[0][col] != '-'             /* vertical */
                && field[0][col] == field[1][col]
                && field[1][col] == field[2][col] )
            {
            return(field[0][col]);
            }
        }
    if (field[0][0] != '-'         /* right diagonal */
            && field[0][0] == field[1][1]
            && field[1][1] == field[2][2] )
        {
        return(field[0][0]);
        }
    if (field[0][2] != '-'         /* left diagonal */
            && field[0][2] == field[1][1]
            && field[1][1] == field[2][0] )
        {
        return(field[0][2]);
        }

    for (row = 0; row < 3; ++row)        /* any moves left */
        {
        for (col = 0; col < 3; ++col)
            {
            if (field[row][col] == '-')
                {
                return('-');
                }
            }
        }
    return ('t');
}

void Make_move(char field[3][3])
{
    int row, col;

    for (row = 2; row >= 0; --row)
        {
        for (col = 2; col >= 0; --col)
            {
            if (field[row][col] == '-')
                {
                field[row][col] = 'O';
                return;
                }
            }
        }
}

void Draw_field(char field[][3])
{
    int row, col;

    printf("\n   1  2  3\n\n");
    for (row = 0; row < 3; ++row)
        {
        printf("%c ", 'a'+row);
        for (col = 0; col < 3; ++col)
            {
            printf(" %c ", field[row][col] );
            }
        printf("\n");
        }
    printf("\n");
}
