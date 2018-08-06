/*  
 *	swap_int -- exchange the values of the two integers
 */

void
swap_int(p1, p2)
register int *p1;
register int *p2;
{
	int tmp;

	/* exchange the values */
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
