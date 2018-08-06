char *memccpy(dst, src, c, cnt)
register char *dst;
register char *src;
register char c;
register int cnt;
/*
 *	Copy bytes from <src> to <dst> until either <cnt> bytes have been
 *	copied, or the character <c> has been copied.  Returns <dst>.
 */
{
	register char *p = dst;

	while(cnt--)
		if((*dst++ = *src++) == c)
			break;
	return(p);
}
