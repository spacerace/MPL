long msize(addr)
register long *addr;
/*
 *	Return the size, in bytes, of the memory block at <addr>.  Note
 *	that the size is a long value, since the block may have been
 *	allocated by lalloc().
 */
{
	return((addr[-1] & 0x00FFFFFF) - 4L);
}
