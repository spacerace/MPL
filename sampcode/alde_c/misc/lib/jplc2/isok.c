/* 1.0  01-22-85 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1985		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

/************************************************************************/
	BOOL
isok(prompt)		/* Output prompt to stdout and accept y/n answer.
			   Return TRUE for y, FALSE for n.		*/
/*----------------------------------------------------------------------*/
STRING prompt;
{
	STRING getns();
	char c, ans[2];

	FOREVER
	{	getns(prompt, ans, 1);
		c = tolower(*ans);
		if ((c IS 'y') OR (c IS 'n'))
			break;
		puts("\nPlease respond with 'y' or 'no'.");
	}
	return ((c IS 'y') ? TRUE : FALSE);
}
