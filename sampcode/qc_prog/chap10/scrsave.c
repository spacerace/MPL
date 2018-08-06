/* scrsave.c  --  demonstrates write() by saving the */
/*                text screen to a file.             */

#include <stdio.h>        /* for stderr              */
#include <fcntl.h>        /* for O_CREAT | O_BINARY  */
#include <sys\types.h>    /* for stat.h              */
#include <sys\stat.h>     /* for S_IREAD | S_IWRITE  */

#define SCRCHARS  (25 * 80)
int Buf[SCRCHARS];

main(argc, argv)
int argc;
char *argv[];
{
    int *cp, *ep, fname[16];
    int far *sp;
    int fd_out, bytes;

    if (argc != 2)
        {
        fprintf(stderr, "usage: scrsave file\n");
        exit(0);
        }
    if (strlen(argv[1]) > 8)
        {
        fprintf(stderr, "\"%s\": File name too long.\n", argv[1]);
        exit(1);
        }
    strcpy(fname, argv[1]);
    strcat(fname, ".SCR");
    if (access(fname, 0) == 0)
        {
        fprintf(stderr, "\"%s\": Won't overwrite.\n", fname);
        exit(1);
        }
    if ((fd_out = open(fname, O_WRONLY|O_CREAT|O_BINARY,
                       S_IREAD|S_IWRITE)) < 0)
        {
        fprintf(stderr, "\"%s\": Can't create.\n", fname);
        exit(1);
        }
    /* Copy the screen into a near buffer. */
    ep = &Buf[SCRCHARS - 1];
    cp = Buf;
    /* use 0xB800000 for EGA or VGA */
    sp = (int far *)(0xB0000000);
    for (; cp < ep; ++cp, ++sp)
        *cp = *sp;
    /* Write it. */
    bytes = write(fd_out, Buf, SCRCHARS * 2);
    if (bytes != SCRCHARS * 2)
        {
        fprintf(stderr, "\"%s\": Error writing.\n", fname);
        exit(1);
        }
}
