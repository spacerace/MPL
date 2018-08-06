static int sgetc(s)
unsigned char **s;
/*
 *	Local function to get a character from a string
 */
{
	return(*(*s)++);
}

static int sungetc(c, s)
unsigned char c, **s;
/*
 *	Local function to unget a character from a string
 */
{
	return(*--(*s) = c);
}

sscanf(buf, fmt, arg)
unsigned char *buf;
unsigned char *fmt;
int arg;
{
	return(_scanf(&buf, sgetc, sungetc, fmt, &arg));
}
