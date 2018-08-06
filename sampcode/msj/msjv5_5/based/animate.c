// animate.c RHS uses .LST file to display objects for animation

#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include"list.h"

#define SCREENSIZE 4000
#define SCREEN_ADDRESS 0xb8000000

void fp_show_object(unsigned far *object);
void bp_show_object(unsigned far *object);
void main(int argc, char **argv);

unsigned far *screen = (void far *)SCREEN_ADDRESS;
void fp_show_object(unsigned far *object)
    {
    unsigned i;
    unsigned far *sptr = screen;

    for(i = 0; i < SCREENSIZE/2; i++, *sptr++ = *object++)
        ;
    }

unsigned _based(screen) *bptr;
void bp_show_object(unsigned far *object)
    {
    unsigned i;

    bptr = 0;

    for(i = 0; i < SCREENSIZE/2; i++, *bptr++ = *object++)
        ;
    }

void main(int argc, char **argv)
    {
    void far *L;
    void far *object;
    char *screen = "APPART";
    unsigned numobjects, i, based = 0;
    unsigned lih;

    if(argc < 2 || argc > 3)
        error_exit(0,
           "Usage: ANIMATE <filename.LST> [/Based pointer display]");

    if(argc == 3 &&
            ((*argv[2]=='-' || *argv[2]=='/') &&
            (argv[2][1]=='b' || argv[2][1]=='B')))
        based = 1;

    if(L = ListRestore(argv[1]))
        {
        numobjects = ListGetNumItems(L);

        while(1)
            {
            lih = ListGetItem(L);
            for(i = 0; i < numobjects; i++)
                {
                if(object = ListItemGetObject(L, lih))
                    if(based)
                        bp_show_object((unsigned far *)object);
                    else
                        fp_show_object((unsigned far *)object);
                lih = ListItemGetNextItem(L,lih);
                }
            if(kbhit())
                break;
            }
        }
    }

