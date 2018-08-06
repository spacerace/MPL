
#include "io.h"

unlink(name)
char *name;
{
        struct fcb delfcb;

        fcbinit(name,&delfcb);
        return bdos(DELFIL,&delfcb);
}

