int blkcmp(blk1, blk2, len)
register char *blk1;
register char *blk2;
register int len;
/*
 *	Lexicographically compare the two blocks.  Return a value
 *	indicating the relationship between the blocks.  Possible
 *	return values are:
 *		negative	blk1 < blk2
 *		0		blk1 == blk2
 *		positive	blk1 > blk2
 *	<len> bytes are always compared.
 */
{
	while((--len) && (*blk1 == *blk2)) {
		++blk1;
		++blk2;
	}
	return(*blk1 - *blk2);
}
