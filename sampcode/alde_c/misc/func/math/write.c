
/***********************************************************
 *     IBM PC "C" Bulletin Board                           *
 *     Tulsa, OK  918-664-8737                             *
 *     SYSOP Lynn Long   300/1200 XMODEM                   *
 ***********************************************************/


#include "io.h"
#include "errno.h"

int filewr(), badfd(), conwr();

static int (*wrt_tab[])() = {
        badfd, filewr, conwr,
};

write(fd, buff, len)
char *buff;
{
        register struct channel *chp;

        if (fd < 0 || fd > MAXCHAN) {
                errno = EBADF;
                return -1;
        }
        chp = &chantab[fd];
        return (*wrt_tab[chp->c_write])(chp->c_arg, buff, len);
}

conwr(kind, buff, len)
register char *buff;
{
        register int count;

        for (count = 0 ; count < len ; ++count)
                bdos(kind, *buff++);
        return count;
}


