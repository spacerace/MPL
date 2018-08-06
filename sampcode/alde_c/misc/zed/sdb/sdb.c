/* SDB - main routine */

#include "stdio.h"
#include "sdbio.h"

extern int dbv_errcode;

main()
{
    printf("SDB - version 2.0\n");
    db_sinit();
    db_ifile("sdb.ini");

    while (TRUE) {
        db_prompt("SDB> ","\t> ");
        if (!db_parse(NULL)) {
            printf("** error: %s ***\n",db_ertxt(dbv_errcode));
            db_kill();
        }
    }
}

