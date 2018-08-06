int blkicmp(blk1, blk2, len)
register char *blk1;
register char *blk2;
register int len;
/*
 *	Same as blkcmp() except the case of alphabetic characters is
 *	ignored during comparison.
 */
{
	register char c1, c2;

	while(((c1 = tolower(*blk1++)) == (c2 = tolower(*blk2++))) && (--len))
		;
	return(c1 - c2);
}
