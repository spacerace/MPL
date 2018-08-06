/*
 *	hex.c -- hex conversions routines
 */

#define NIBBLE	0x000F
#define BYTE	0x00FF
#define WORD	0xFFFF

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
	char *cp;
	unsigned int d;

	d = data & BYTE;
	cp = buf;
	*cp++ = hextab[(d >> 4) & NIBBLE];
	*cp++ = hextab[d & NIBBLE];
	*cp = '\0';

	return (buf);
}

/*
 *	word2hex -- convert a word to a string
 *	representation of its hexadecimal value
 */

char *
word2hex(data, buf)
unsigned int data;
char *buf;
{
	char *cp;
	unsigned int d;

	d = data & WORD;
	cp = buf;
	*cp++ = hextab[(d >> 12) & NIBBLE];
	*cp++ = hextab[(d >> 8) & NIBBLE];
	*cp++ = hextab[(d >> 4) & NIBBLE];
	*cp++ = hextab[d & NIBBLE];
	*cp = '\0';

	return (buf);
}
