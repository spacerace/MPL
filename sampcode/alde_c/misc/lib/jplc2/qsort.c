/* 1.0  11-27-84 */
/************************************************************************
 *			Robert C. Tausworthe				*
 *			Jet Propulsion Laboratory			*
 *			Pasadena, CA 91009		1984		*
 ************************************************************************/

#include "defs.h"
#include "stdtyp.h"

LOCAL char *_base;
LOCAL unsigned _width;
LOCAL int (*_compare)();

/************************************************************************/
	VOID
qsort(b, n, w, comp)	/* Quicksort n elements beginning at base b, of
			   width w characters each, using comparison
			   function comp(p, q), which is greater than 0
			   if, and only if, elements *p and *q are
			   out of sort.					*/
/*----------------------------------------------------------------------*/
char *b;
unsigned n, w;
int (*comp)();
{
	VOID _swap();
	int _comp();

	_base = b;
	_width = w;
	_compare = comp;
	sortQ(n, _comp, _swap);
}

/************************************************************************/
	LOCAL VOID
_swap(i, j)		/* exchange elements i and j of base array.	*/

/*----------------------------------------------------------------------*/
unsigned i, j;
{
	char *I, *J, c;
	unsigned k;

	I = _base + _width * i;
	J = _base + _width * j;
	for (k = 0; k < _width; k++)
	{	c = *I;
		*I++ = *J;
		*J++ = c;
	}
}
/*\p**********************************************************************/
	LOCAL
_comp(i, j)	/* convert comparison access from index to pointer.	*/

/*----------------------------------------------------------------------*/
unsigned i, j;
{
	char *I, *J;

	I = _base + _width * i;
	J = _base + _width * j;
	return ((*_compare)(I, J));
}
