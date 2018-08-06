/*
 *	hex.c -- hex conversions routines
 */

#define NIBBLE	0x000F

char hextab[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/*
 *	byte2hex -- convert a byte to a string
 *	representation of its hexadecimal value
 */

char *
byte2hex(data, buf)
unsigned char data;
char *buf;
{
	register char *cp;

	cp = buf;
	*cp++ = hextab[(data >> 4) & NIBBLE];
	*cp++ = hextab[data & NIBBLE];
	*cp = '\0';

	return (buf);
} /* end byte2hex() */

/*
 *	word2hex -- convert a word to a string
 *	representation of its hexadecimal value
 */

char *
word2hex(data, buf)
unsigned int data;
char *buf;
{
	register char *cp;

	cp = buf;
	*cp++ = hextab[(data >> 12) & NIBBLE];
	*cp++ = hextab[(data >> 8) & NIBBLE];
	*cp++ = hextab[(data >> 4) & NIBBLE];
	*cp++ = hextab[data & NIBBLE];
	*cp = '\0';

	return (buf);
} /* end word2hex() */
