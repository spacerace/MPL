int strrpos(string, symbol)
register char *string;
register char symbol;
/*
 *	Return the index of the last occurance of <symbol> in <string>.
 *	-1 is returned if <symbol> is not found.
 */
{
	register int i = 0;
	register char *p = string;

	while(*string++)
		++i;
	do {
		if(*--string == symbol)
			return(i);
		--i;
	} while(string != p);
	return(-1);
}
