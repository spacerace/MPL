/* rightstr.c -- a C version of BASIC's RIGHT$ */

#include <stdio.h>

char *Rightstr(char *str, int cnt)
{
    static char *cp = NULL;
    char *malloc();

    if (cnt > strlen(str))
        cnt = strlen(str);
    if (cp != NULL)
        free(cp);
    if ((cp = malloc(cnt + 1)) == NULL)
        return (NULL);
    strcpy(cp, str + strlen(str) - cnt);
    return (cp);
}
