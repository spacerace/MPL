#include <stdio.h>

extern	int	fputc();

fprintf(fp, fmt, arg)
FILE *fp;
char *fmt;
int arg;
{
	return(_printf(fp, fputc, fmt, &arg));
}

printf(fmt, arg)
char *fmt;
int arg;
{
	return(_printf(stdout, fputc, fmt, &arg));
}
