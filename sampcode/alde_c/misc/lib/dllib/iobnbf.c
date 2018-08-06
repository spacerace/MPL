/*_ iob.c   Thu Oct 30 1986   Modified by: Walter Bright */

#include	<stdio.h>

FILE _iob[_NFILE] =
{
	{0,0,0,_IOTRAN | _IOREAD,0,1},		/* stdin	*/
	{0,0,0,_IOTRAN | _IOWRT | _IONBF ,1,1},	/* stdout	*/
	{0,0,0,_IOTRAN | _IOWRT | _IONBF ,2,1},	/* stderr	*/
	{0,0,0,_IOTRAN | _IORW  ,3,1},		/* stdaux	*/
	{0,0,0,_IOTRAN | _IOWRT ,4,1},		/* stdprn	*/

	/* the rest have all 0 entries	*/
};
