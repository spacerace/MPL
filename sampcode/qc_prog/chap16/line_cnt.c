/*  line_cnt.c -- An overly active line counter       */
#include <stdio.h>
main()
{
    int ch;
    int lines = 0;

    while ( (ch = getchar() ) != EOF )
        if ( ch = '\n')
            lines++;
    printf("There were %d lines\n", lines);
}

