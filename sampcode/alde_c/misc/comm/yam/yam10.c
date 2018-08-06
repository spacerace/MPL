/*
>> yam10.c 04-Mar-84
	File unsqueezer module taken from Richard Greenlaw's USQ
*/
#include "yam.h"

/* *** Stuff for first translation module *** */
#define DLE 0x90
/* *** Stuff for second translation module *** */
#define SPEOF 256	/* special endfile token */
#define LARGE 30000

unsqueeze()

{
	int i, c;
	char cc;

	char *p;
	int numnodes;		/* size of decoding tree */
	unsigned linect;	/* count of number of lines previewed */
	char origname[14];	/* Original file name without drive */

	/* Initialization */
	linect = 0;
	init_cr();
	init_huff();

	/* Process rest of header (RECOGNIZE already read) */
	getw(&fin);	/* ignore checksum ... */

	/* Get original file name */
	p = origname;	/* send it to array */
	do
	{
		*p = getc(&fin);
	} while(*p++ != '\0');

	printf("%s -> %s\n", Tname, origname);

	numnodes = getw(&fin);
	if(numnodes < 0 || numnodes >= NUMVALS)
	{
		printf("%s has invalid decode tree size\n", Tname);
		return 1;
	}

	/* Initialize for possible empty tree (SPEOF only) */
	dnode[0].children[0] = -(SPEOF + 1);
	dnode[0].children[1] = -(SPEOF + 1);

	/* Get decoding tree from file */
	for(i = 0; i < numnodes; ++i)
	{
		dnode[i].children[0] = getw(&fin);
		dnode[i].children[1] = getw(&fin);
	}

	while((c = getcr()) != EOF)
	{
#ifdef RESTRICTED
		if (c == '\n')  /* new line? */
		{
			if (++linect >= MAXLINES)
			{	printf("\n\n\t[== Squeezed file %d-line limit exceeded ==]",MAXLINES);
				printf("\n\t[Use S mode to transfer file to your system]\n\n");
				return CNTRLK;
			}
		}
#endif
		if( !(c=putcty(c)))
			continue;
		if(c == CNTRLC || c == CAN || c == CNTRLK)
		{
			return c;
		}
	}
	return CPMEOF;
}


/* initialize decoding functions */

init_cr()

{
	repct = 0;
}

init_huff()
{
	bpos = 99;	/* force initial read */
}

/* Get bytes with decoding - this decodes repetition,
 * calls getuhuff to decode file stream into byte
 * level code with only repetition encoding.
 *
 * The code is simple passing through of bytes except
 * that DLE is encoded as DLE-zero and other values
 * repeated more than twice are encoded as value-DLE-count.
 */

int
getcr()

{
	int c;

	if(repct > 0)
	{
		/* Expanding a repeated char */
		--repct;
		return value;
	}
	else
	{
		/* Nothing unusual */
		if((c = getuhuff()) != DLE)
		{
			/* It's not the special delimiter */
			value = c;
			if(value == EOF)
				repct = LARGE;
			return value;
		}
		else
		{
			/* Special token */
			if((repct = getuhuff()) == 0)
			{
				/* DLE, zero represents DLE */
				return DLE;
			}
			else
			{
				/* Begin expanding repetition */
				repct -= 2;	/* 2nd time */
				return value;
			}
		}
	}
}

/* Decode file stream into a byte level code with only
 * repetition encoding remaining.
 */

int
getuhuff()

{
	int i;
	int bitval;

	/* Follow bit stream in tree to a leaf*/
	i = 0;	/* Start at root of tree */
	do
	{
		if(++bpos > 7)
		{
			if((curin = getc(&fin)) == ERROR)
				return ERROR;
			bpos = 0;
			/* move a level deeper in tree */
			i = dnode[i].children[1 & curin];
		}
		else
		{
			i = dnode[i].children[1 & (curin >>= 1)];
		}
	} while(i >= 0);

	/* Decode fake node index to original data value */
	i = -(i + 1);
	/* Decode special endfile token to normal EOF */
	i = (i == SPEOF) ? EOF : i;
	return i;
}
