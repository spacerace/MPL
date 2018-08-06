
/* Copyright (C) 1981,1982 by Manx Software Systems and Thomas Fenwick */
#include "stdio.h"

char *gets(line)
char *line;
{
        register char *cp;
        register int i;

        cp = line;
        while ((i = getchar()) != EOF && i != '\n')
                *cp++ = i;
        *cp = 0;
        if (i == EOF && cp == line)
                return NULL;
        return line;
}

char *fgets(s, n, fp)
char *s; FILE *fp;
{
        register c;
        register char *cp;

        cp = s;
        while (--n > 0 && (c = agetc(fp)) != EOF) {
                *cp++ = c;
                if (c == '\n')
                        break;
        }
        *cp = 0;
        if (c == EOF && cp == s)
                return NULL;
        return(s);
}

agetc(ptr)
register FILE *ptr;
{
        register int c;

top:
        if ((c = getc(ptr)) != EOF) {
                switch (c &= 127) {
                case 0x1a:
                        --ptr->_bp;
                        return EOF;
                case '\r':
                case 0:
                        goto top;
                }
        }
        return c;
}


