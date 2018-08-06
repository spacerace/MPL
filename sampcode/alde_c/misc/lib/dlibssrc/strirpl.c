int strirpl(string, ptrn, rpl, n)
char *string;
char *ptrn;
register char *rpl;
register int n;
/*
 *	Same as strrpl() except ignores case of alphabetic characters.
 */
{
	register char *p, *q = string;
	register int d, rlen, nn = 0;
	char *stristr();

	rlen = strlen(rpl);
	d = rlen - strlen(ptrn);
	while(n && (p = stristr(q, ptrn))) {
		++nn;
		stradj(p, d);
		strncpy(p, rpl, rlen);
		q = p + rlen;
		if(n > 0)
			--n;
	}
	return(nn);
}
