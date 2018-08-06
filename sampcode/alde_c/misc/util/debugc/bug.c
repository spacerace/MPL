#include bug.h
#include stdio.h

      /*  *****************************************************  */
      /*  Dr. Bob's Utilities    Copyright (c) 1985              */
      /*  DEBUG  preprocessor macro example                      */
      /*  compile with  cc -ddebug bug.c     to see debug stuff  */
      /*  *****************************************************  */


main()
{
    int n;

    DB_PRINT("Debug is active\n\n");
    NDB_PRINT("Debug is NOT active\n\n");

    HERE("main");

    readfile();

    DEBUG(
       printf("\n\nIf debug is defined only two calls to ");
       printf("sortfile() will be made here\n");
       sortfile();
       sortfile();
    )
    HERE("main2");

    N_DEBUG(
       printf("Statements in this section will execute only if debug is NOT defined.\n");
       for(n = 1; n <= 8; n++)
       sortfile();
    )

    writefile();
    printf("\n");
    HERE("main3");

    printf("\nThis should be printed no matter what\n");

}


readfile()
{
    HERE("readfile");
}


sortfile()
{
    static int i = 1;
    DB_PRINT("      sortfile - pass #%d \n" COMMA i++);
    NDB_PRINT ("sf() - pass #%d \n" COMMA i++);
}


writefile()
{
    static char st[] = "This message created by writefile";
    HERE("writefile");
    DB_PRINT("\nThe string is: %s\n" COMMA st);
}en through debugging, use:

            