/* scrrest.c  --  demonstrates read() by restoring */
/*                text screen from any file.       */

#include <stdio.h>        /* for stderr              */
#include <fcntl.h>        /* for O_RDONLY | O_BINARY */

#define SCRCHARS  (25 * 80)
int Buf[SCRCHARS];

main(argc, argv)
int argc;
char *argv[];
{
    int *cp, *ep;
    int far *sp;
    int fd_in, bytes;

    if (argc != 2)
        {
        fprintf(stderr, "usage: scrrest file.scr\n");
        exit(0);
        }
    if ((fd_in = open(argv[1], O_RDONLY | O_BINARY)) < 0)
        {
        fprintf(stderr, "\"%s\": Can't open to read.\n", argv[1]);
        exit(1);
        }
    /* Read it. */
    bytes = read(fd_in, Buf, SCRCHARS * 2);
    if (bytes < 0)
        {
        fprintf(stderr, "\"%s\": Error Reading.\n", argv[1]);
        exit(1);
        }
    if (bytes == 0)
        {
        fprintf(stderr, "\"%s\": Empty File.\n", argv[1]);
        exit(1);
        }
    /* Copy the buffer to screen memory. */
    ep = &Buf[bytes / 2];
    cp = Buf;

    /* use 0xB800000 for EGA or VGA */
    sp = (int far *)(0xB0000000);
    for (; cp < ep; ++cp, ++sp)
        *sp = *cp;
}
