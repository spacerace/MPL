static unsigned int sputc(c, s)
char c, **s;
/*
 *	Local function to put a character to a string
 */
{
	return(*(*s)++ = c);
}

sprintf(buf, fmt, arg)
char *buf;
char *fmt;
int arg;
{
	register int n;
	register char *p = buf;

	n = _printf(&buf, sputc, fmt, &arg);
	p[n] = '\0';		/* always tie of the string */
	return(n);
}
