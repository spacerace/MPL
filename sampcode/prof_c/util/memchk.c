/*
 *	memchk -- look for random-access memory at
 *	a specified location; return non-zero if found
 */

#include <dos.h>
#include <memory.h>

int
memchk(seg, os)
unsigned int seg;
unsigned int os;
{
	unsigned char tstval, oldval, newval;
	unsigned int ds;
	struct SREGS segregs;

	/* get value of current data segment */
	segread(&segregs);
	ds = segregs.ds;

	/* save current contents of test location */
	movedata(seg, os, ds, (unsigned int)&oldval, 1);

	/* copy a known value into test location */
	tstval = 0xFC;
	movedata(ds, (unsigned int)&tstval, seg, os, 1);

	/* read test value back and comapre to value written */
	movedata(seg, os, ds, (unsigned int)&newval, 1);
	if (newval != tstval)
		return (0);

	/* restore original contents of test location */
	movedata(ds, (unsigned int)&oldval, seg, os, 1);

	return (1);
}
