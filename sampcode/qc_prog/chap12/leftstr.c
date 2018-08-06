/* leftstr.c -- a C version of BASIC's LEFT$ */

#include <stdio.h>

char *Leftstr(char *str, int cnt)
{
    static char *cp = NULL;
    char *malloc();

    if (cnt > strlen(str))
        cnt = strlen(str);
    if (cp != NULL)
        free(cp);
    if ((cp = malloc(cnt + 1)) == NULL)
        return (NULL);
    strncpy(cp, str, cnt);
    return (cp);
}
