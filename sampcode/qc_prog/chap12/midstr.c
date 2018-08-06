/* midstr.c -- a C version of BASIC's MID$  */

#include <stdio.h>

char *Midstr(char *str, int where, int cnt)
{
    static char *cp = NULL;
    char *malloc();

    if (cnt > strlen(str + where))
        cnt = strlen(str + where);
    if (cp != NULL)
        free(cp);
    if ((cp = malloc(cnt + 1)) == NULL)
        return (NULL);
    strncpy(cp, str+where, cnt);
    return (cp);
}

