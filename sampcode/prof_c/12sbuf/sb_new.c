/*
 *	sb_new -- prepare a new window (rectangular region)
 *	and return a pointer to it
 */

#include <stdio.h>
#include <malloc.h>
#include <local\sbuf.h>

struct REGION *
sb_new(top, left, height, width)
int top;	/* top row */
int left;	/* left column */
int height;	/* total rows */
int width;	/* total columns */
{
	struct REGION *new;

	/* allocate the control data structure */
	new = (struct REGION *)malloc(sizeof (struct REGION));
	if (new != NULL) {
		new->r0 = new->sr0 = top;
		new->r1 = new->sr1 = top + height - 1;
		new->c0 = new->sc0 = left;
		new->c1 = new->sc1 = left + width - 1;
		new->row = new->col = 0;
		new->wflags = 0;
	}
	return (new);
}
