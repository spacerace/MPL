// testlist.c RHS

#include<stdio.h>
#include<malloc.h>
#include"list.h"

#define OBJECTSIZE  10
void main(void)
    {
    void far *L;

    if(L = ListInit(0,"TEST"))
        {
        ListAdd(L,_fmalloc(OBJECTSIZE), OBJECTSIZE, "JUNK01");
        ListAdd(L,_fmalloc(OBJECTSIZE), OBJECTSIZE, "JUNK02");
        ListAdd(L,_fmalloc(OBJECTSIZE), OBJECTSIZE, "JUNK03");
        ListAdd(L,_fmalloc(OBJECTSIZE), OBJECTSIZE, "JUNK04");
        ListAdd(L,_fmalloc(OBJECTSIZE), OBJECTSIZE, "JUNK05");
        }
    ListDump(L);
    }

