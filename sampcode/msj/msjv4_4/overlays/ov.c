#include <stdio.h>
#include <string.h>

void p1(void);
void p2(void);

extern char executable_name;

void main(int argc, char **argv)
{
    int i;

    strcpy(&executable_name,argv[0]);
    argc = argc;

    printf("This is main\n");
    for (i = 0; i < 1000; i++) {
        p1();
        p2();
    }
}


