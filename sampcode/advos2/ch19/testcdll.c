/*
	TESTCDLL.C

	A simple test program for the CDLL.DLL dynlink library.

	Compile with:  C> cl testcdll.c /link cdll.lib

	Usage is:  C> testcdll

	Copyright (C) 1988 Ray Duncan
*/

#include <stdio.h>

int extern far pascal MYFUNC(int, int);

main()
{
    int x;          /* temporary storage */

    printf("\nCalling CDLL.MYFUNC\n");

    x = MYFUNC(1, 2);

    printf("\nMYFUNC(1, 2) = %d\n", x);
}

