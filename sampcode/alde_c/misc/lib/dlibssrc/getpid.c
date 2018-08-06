extern char	*_base;

int getpid()
/*
 *	Return an integer value unique for this process.
 */
{
	register unsigned long n;

	n = _base;			/* load process base address */
	return(0x7FFF & (n>>8));	/* create unique pid from it */
}
