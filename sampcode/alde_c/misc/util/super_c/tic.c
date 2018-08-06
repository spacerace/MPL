/*                      TicTac

        This program plays a perfect game of tic-tac-toe.
*/

#define FALSE 0
#define TRUE 1

char board[9];   /* The playing board; for each space, 0 is not used,
                   1 is X, -1 is O. */

/*      main()

        Function: Ask the user which side(s) he wants to play. Then
        make or ask for moves one each side until one side wins.

        Algorithm: First, set humanX and humanO to indicate which
        side(s) the human is playing. Then init the board and print
        it out. Then iterate making or asking for a move, printing
        it out, and testing for a win. Each pass thru the main loop,
        the program switches sides.
*/

main()

{
        register int side;               /* Which side is playing (1 or -1). */
        int move;               /* Next move. */
        int val;                /* Value of move. */
        int humanX, humanO;     /* TRUE if human is playing X/O. */
        char str[20];           /* Input are for yes/no. */

        /* Ask the user if the computer should play the Xs. */
        printf("Should the computer play Xs? ");
        gets(str);
        if ((str[0] == 'y') || (str[0] == 'Y')) humanX = FALSE;
        else humanX = TRUE;
        /* Ask about playing the Os. */
        printf("Should the computer play Os? ");
        gets(str);
        if ((str[0] == 'y') || (str[0] == 'Y')) humanO = FALSE;
        else humanO = TRUE;

        /* Initialize the board. */
        initBd();

        /* Show the empty board. */
        printBd();

        /* X goes first. */
        side = 1;

        /* Loop until a win or the player quits. */
        while (TRUE) {
                /* If this side is played by the human... */
                if ((humanX && (side == 1)) || (humanO && (side == -1))) {
                        /* Query for move; repeat until legal. */
                        do {
                            if (side == 1)
                                printf("X's move (enter 1-9, or 0 to quit): ");
                            else printf("O's move (enter 1-9, or 0 to quit): ");
                            scanf("%d",&move);
                        } while ((move != 0) && (board[move-1] != 0));
                        /* Check for quit. */
                        if (move == 0) break;
                /* Otherwise, decide on the best move to make. */
                } else move = bestMove(side,&val)+1;
                /* Make the move. */
                board[move-1] = side;
                /* Show the new board to the user. */
                printBd();
                /* If that one won it, say so and exit. */
                if (hasWon(side)) {
                        printf("That move won!\n");
                        break;
                };
                /* If it was a tie, say so and exit. */
                if (isTie()) {
                        printf("Tie game!\n");
                        break;
                };
                /* Switch sides for the next move. */
                side = -side;
        };
}

/*      initBd()

        Function: Initialize the board to all empty.

        Algorithm: Set each board entry to zero.
*/

initBd()

{
        register int i;

        /* Zero (empty) each square in the board. */
        for (i = 0; i < 9; i++) board[i] = 0;
}

/*      isTie()

        Function: Return TRUE if the current board position is a tie.

        Algorithm: See if we can find an untaken board position. If not,
        it's a tie. Otherwise, there's still hope.
*/

isTie()

{
        register int i;

        /* Check each space on the board. */
        for (i = 0; i < 9; i++) if (board[i] == 0) return(FALSE);
        /* If none found, return tie. */
        return(TRUE);
}

/*      hasWon(p)

        Function: Return true if player p has won, where p is 1 for X
        and -1 for O.

        Algorithm: Check for every possible three in a row. If any are
        found, it's a win, so we return TRUE; otherwise, we return FALSE.
*/
#ifdef NEVER
hasWon(p)

int p;

{
        register int i;
        register int w;

        /* Calculate the sum of three in a row. */
        w = 3*p;

        /* Check for three in a row horizontally. */
        for (i = 0; i < 9; i += 3)
                if ((board[i]+board[i+1]+board[i+2]) == w) return(TRUE);

        /* Check for three in a row vertically. */
        for (i = 0; i < 3; i++)
                if ((board[i]+board[i+3]+board[i+6]) == w) return(TRUE);

        /* Check the diagonols. */
        if ((board[0]+board[4]+board[8]) == w) return(TRUE);
        if ((board[2]+board[4]+board[6]) == w) return(TRUE);

        /* If nothing, then return a non-win. */
        return(FALSE);
}
#endif NEVER
/*      bestMove(p,v)

        Function: Return the best possible move for player p. p is 1
        for X or -1 for O. Also return the value of that move (1 for
        a win, 0 for a tie, and -1 for a loss) in v.
*/

bestMove(p,v)

int p;
int *v;

{
        register int i;
        int lastTie;
        register int lastMove;
        int subV;

        /* First, check for a tie. */
        if (isTie()) {
                *v = 0;
                return(0);
        };

        /* If not a tie, try each potential move. */
        for (*v = -1, lastTie = lastMove = -1, i = 0; i < 9; i++) {
                /* If this isn't a possible move, skip it. */
                if (board[i] != 0) continue;
                /* Make the move. */
                lastMove = i;
                board[i] = p;
                /* Did it win? */
                if (hasWon(p)) *v = 1;
                else {
                        /* If not, find out how good the other side can do. */
                        bestMove(-p,&subV);
                        /* If they can only lose, this is still a win. */
                        if (subV == -1) *v = 1;
                        /* Or, if it's a tie, remember it. */
                        else if (subV == 0) {
                                *v = 0;
                                lastTie = i;
                        };
                };
                /* Take back the move. */
                board[i] = 0;
                /* If we found a win, return immediately (can't do any
                   better than that). */
                if (*v == 1) return(i);
        };
        /* If we didn't find any wins, return a tie move. */
        if (*v == 0) return(lastTie);
        /* If there weren't even any ties, return a loosing move. */
        else return(lastMove);
}

/*      printBd()

        Function: Display the board for the user.

        Algorithm: Iterate thru each square and print it out.
*/

printBd()

{
        register int i;

        /* For each square... */
        for (i = 0; i < 9; i++) {
                /* Print X, O, or space. */
                if (board[i] == 0) printf(" ");
                else if (board[i] == 1) printf("X");
                else printf("O");
                /* If this is the last square in the line... */
                if ((i % 3) == 2) {
                        /* Print the adjacent numbered board. */
                        printf("    %1d|%1d|%1d\n",i-1,i,i+1);
                        if (i != 8) printf("-+-+-    -+-+-\n");
                } else printf("|");
        };
        printf("\n");
}

