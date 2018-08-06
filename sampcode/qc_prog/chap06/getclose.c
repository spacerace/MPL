/* getclose.c -- a number game using */
/*               random numbers      */

#define TRUE 1
#define FALSE 0

    /* external variables */
    int number,     /* total number in current game   */
        moves,      /* number of moves in current game*/
        target,     /* target number to reach         */
        done,       /* true if game is over           */
        score,      /* score of current game          */
        wins = 0,   /* number of games won            */
        losses = 0, /* number of games lost           */
        total = 0;  /* total score                    */

    char move;

    /* function prototype declarations */
    void intro(void);       /* tell player about game   */
    char getyn(void);       /* get yes/no response      */
    int  random(int num);   /* random between 1 and num */
    void new_target(void);  /* target number for game   */
    char get_move(void);    /* get player's move        */
    void do_move(void);     /* generate num from move   */
    void check_move(void);  /* won, lost, or continue?  */
    void show_score(void);  /* show score for game      */
    void show_total(void);  /* show total score         */

main()
{
    intro();         /* print instructions */

    while (TRUE)     /* new games until user quits */
        {
        printf("\nDo you want to continue? ");
        if (getyn() != 'y')
            break;   /* exit program */

        done = FALSE;
        number = moves = score = 0;
        new_target();  /* target number for this game */
        while (!done)  /* play one game */
            {
            get_move();/* user selects random number  */
            do_move(); /* generate random number      */
                       /* and add                     */
            check_move();  /* win, lose, or continue? */
            }
        show_score();  /* score for this game */
        show_total();  /* total score         */
        }
}

void intro(void)
{
    printf("Welcome to Getclose\n\n");
    printf("The object of this game is to\n");
    printf("try to get as close to the target\n");
    printf("number as possible in as few\n");
    printf("moves as possible by choosing from\n");
    printf("various ranges of random numbers.\n");
    printf("You score if you get within 4 of the\n");
    printf("target; a 100 point bonus for hitting\n");
    printf("the target, but no score if you go over.\n\n");
    }

char getyn(void)
                    /* get yes or no answer      */
                    /* repeats until valid entry */
{
    char ch;  /* character to read and return */

    while (TRUE)
        {
        printf(" (y or n) ");
        ch = getche();
        printf("\n");
        if ((ch == 'y') || (ch == 'n'))
        /* valid response, break out of loop */
            break;
        /* give error message and loop again */
        printf("please enter ");
        }
    return(ch);
    }

int random(int num)
    /* generate random number between 1 and num     */
    /* doesn't use library function srand() because */
    /* we don't want the same seed each time        */
{
    long seconds, result;
    time(&seconds);   /* randomize with system time */
    return (abs ((int)seconds * rand() % num) + 1);
}

void new_target(void)
             /* generate a new target number */
             /* between 50 and 99            */
{
    target = 50 + random(49);
    printf("\nYour target for this game is %d\n",
        target);
}

char get_move(void)
    {
    while (TRUE)
        {
        printf("\nPick a random number from 1 to\n");
        printf("a) 5  b) 10  c) 25  d) 50  e) 100 ");
        move = getche();
        if ((move >= 'a') && (move <= 'e'))
            {
            ++moves; /* count the move */
            break;   /* valid response */
            }
        /* invalid response, try again */
        printf("\nPlease type a, b, c, d, or e\n");
        }
}

void do_move(void)
    {
    int num = 0;  /* random value to obtain */
    switch (move)
        {
        case 'a' :
            num = random(5);
            break;
        case 'b' :
            num = random(10);
            break;
        case 'c' :
            num = random(25);
            break;
        case 'd' :
            num = random(50);
            break;
        case 'e' :
            num = random(100);
            break;
        }
    number += num;  /* add new number to total */
    printf("\n\nYou got a %d. Number is now: %d ", num, number);
    printf("(Target is %d)\n", target);
}

void check_move(void)
{
    int temp;
    if (number > target)
        {
        printf("\nYou went over! ");
        printf("No score this game.\n");
        losses++;
        done = TRUE; /* to break out of loop */
        }
    if (number == target)
        {
        printf("\nYou hit the target ");
        printf("for 100 bonus points!\n");
        score = (100 / moves) + 100;
        total += score;
        wins++;
        done = TRUE;
        }
    if ((number >= (target - 4)) && (number < target))
        {
        temp = 100 / moves;
        /* does player want to go for broke? */
        printf("\nTake %d points (y) or continue (n)? ",
            temp);
        if (getyn() == 'y')
            {
            score = temp;
            total += score;
            wins++;
            done = TRUE;
            }
       }
}

void show_score(void)
{
    printf("\nYou scored %d points in %d moves.\n",
        score, moves);
}

void show_total(void)
{
    printf("You have won %d games ", wins);
    printf("and lost %d.\n", losses);
    printf("Your total score is %d\n", total);
}
