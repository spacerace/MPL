#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>

_segment psp;

typedef struct _psp
    {
    unsigned        int20;
    unsigned        allocblockseg;
    char            reserved0;
    unsigned char   dosfunctdispatch[5];
    unsigned long   int22;
    unsigned long   int23;
    unsigned long   int24;
    char            reserved1[22];
    _segment        envseg;
    } PSP;

PSP _based(psp) *pspptr = 0;

void main(void)
    {
    psp = _psp;

printf(
   "Int22 is set to %lp\nInt23 is set to %lp\nInt24 is set to %lp\n",
       (void far *)pspptr->int22,
       (void far *)pspptr->int23,
       (void far *)pspptr->int24);

        {
        _segment envseg = pspptr->envseg;
        char _based((_segment)envseg) *envptr = 0;


        printf("pspptr->envseg=%04x\n",pspptr->envseg);
        printf("Environment:\n");
        for( ; *((char far *)envptr);
            envptr += _fstrlen((char far *)envptr)+1)
            printf("%Fs\n",(char far *)envptr);

        }
    }
