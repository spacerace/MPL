int strpos(string, symbol)
register char *string;
register char symbol;
/*
 *	Return the index of the first occurance of <symbol> in <string>.
 *	-1 is returned if <symbol> is not found.
 */
{
	register int i = 0;

	do {
		if(*string == symbol)
			return(i);
		++i;
	} while(*string++);
	return(-1);
}
