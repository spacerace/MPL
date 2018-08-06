
/************************************************************
 *         IBM PC "C" Bulletin Board                        *
 *         Tulsa, OK 918-664-8737                           *
 *         SYSOP Lynn Long 300/1200 XMODEM                  *
 ***********************************************************/

#include "io.h"
#include "errno.h"
#include "fcntl.h"

int badfd(), filerd(), conin();

static int (*readtab[])() = {
        badfd, filerd, conin,
};
extern int errno;

read(fd, buff, len)
char *buff;
{
        register struct channel *chp;

        if (fd < 0 || fd > MAXCHAN) {
                errno = EBADF;
                return -1;
        }
        chp = &chantab[fd];
        return (*readtab[chp->c_read])(chp->c_arg, buff, len);
}

conin(x,buff,len)
char *buff;
{
        static char buffer[258] = { 0,0,0,0,0 };
        static int used = 0;
        register int l;

        if (buffer[1] == 0) {
                buffer[0] = 255;
                buffer[1] = buffer[2] = 0;
                bdos(10,buffer);
                bdos(2,'\n');
                if (buffer[2] == 0x1a) {
                        buffer[1] = 0;
                        return 0;
                }
                buffer[++buffer[1] + 1] = '\r';
                buffer[++buffer[1] + 1] = '\n';
                used = 2;
        }
        if ((l = buffer[1]) > len)
                l = len;
        blockmv(buff, buffer+used, l);
        used += l;
        buffer[1] -= l;
        return l;
}

