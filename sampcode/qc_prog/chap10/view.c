/* view.c  --  demonstrates lseek() by displaying  */
/*             a file and moving around in it      */

#include <fcntl.h>           /* for open()         */
#include <stdio.h>           /* for SEEK_CUR, etc. */

#define HUNK 512
#define MOVE 512L

main(argc, argv)
int argc;
char *argv[];
{
    char ch, buf[HUNK];
    long position = 0L;
    int  bytes, eofflag = 0, fd_in;

    if (argc != 2)
        {
        fprintf(stderr, "Usage: view file\n");
        exit(0);
        }

    if ((fd_in = open(argv[1], O_RDONLY)) < 0)
        {
        fprintf(stderr, "\"%s\": Can't open.\n", argv[1]);
        exit(1);
        }

    for (;;)
        {
        bytes = read(fd_in, buf, HUNK);
        if (bytes == 0)
            {
            if (! eofflag)
                {
                fprintf(stderr, "\n<<at end of file>>\n");
                ++eofflag;
                }
            else
                exit(0);
            }
        else if (bytes < 0)
            {
            fprintf(stderr, "\"%s\": Error Reading.\n", argv[1]);
            exit(1);
            }
        else
            {
            eofflag = 0;
            position = lseek(fd_in, 0L, SEEK_CUR);
            if (position == -1L)
                {
                fprintf(stderr, "\"%s\": Error Seeking.\n", argv[1]);
                exit(1);
                }
            Print(buf, bytes);
            do
                {
                ch = getch();
                if (ch == 'q' || ch == 'Q')
                    exit(0);
                } while (ch != '+' && ch != '-');

            if (ch == '-')
                {
                position = lseek(fd_in, -2 * MOVE, SEEK_CUR);
                if (position == -1L)
                    {
                    fprintf(stderr, "\"%s\": Error Seeking.\n", argv[1]);
                    exit(1);
                    }
                }
            }
        }
}

Print(char *buf, int cnt)
{
    int i;

    for (i = 0; i < cnt; ++i, ++buf)
        {
        if (*buf < ' ' && *buf != '\n' && *buf != '\t')
            printf("^%c", *buf + '@');
        else
            putchar(*buf);
        }
}
