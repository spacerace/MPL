#include <stdio.h>

extern int fgetc(), fungetc();

fscanf(fp, fmt, arg)
FILE *fp;
char *fmt;
char *arg;
{
	return(_scanf(fp, fgetc, fungetc, fmt, &arg));
}

scanf(fmt, arg)
char *fmt;
char *arg;
{
	return(_scanf(stdin, fgetc, fungetc, fmt, &arg));
}
