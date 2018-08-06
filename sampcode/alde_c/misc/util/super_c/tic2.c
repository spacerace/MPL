#define FALSE 0
#define TRUE 1

char board[9];     /* The playing board; for each space, 0 is not used, 1 is X,
                     -1 is O. */

hasWon(p)

int p;

{
 register int i;
 register int w;

 w = 3*p;
 for (i = 0; i < 9; i += 3)
     if ((board[i]+board[i+1]+board[i+2]) == w) return(TRUE);
 for (i = 0; i < 3; i++)
     if ((board[i]+board[i+3]+board[i+6]) == w) return(TRUE);
 if ((board[0]+board[4]+board[8]) == w) return(TRUE);
 if ((board[2]+board[4]+board[6]) == w) return(TRUE);
 return(FALSE);
}

