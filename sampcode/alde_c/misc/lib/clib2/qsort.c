/* qsort - quick sort
  qsort(n,comp,swap)
 unsigned n;
 int (*comp)();
 int (*swap)();
                  ***** see bsort for parameters
*/
#define void int

static int(*_comp)(), (*_swap)();

void qsort(n,comp,swap)
unsigned n;

int (*comp)();
int (*swap)();

{
	_comp =comp;
    	_swap=swap;
	_quick(0,n-1);
}

static void _quick(lb,ub)

unsigned lb,ub;
{
	unsigned j;
	unsigned _rearr();
	if(lb<ub) {
		if(j=_rearr(lb,ub))
		_quick(lb,j-1);
		_quick(j+1,ub);
	}
}

static unsigned _rearr(lb,ub)
unsigned lb,ub;

{
	do {
		while(ub > lb && (*_comp)(ub,lb) >=0)
			ub--;
		if(ub != lb) {
			(*_swap)(ub,lb);
			while(lb<ub && (*_comp)(lb,ub)<=0)
			lb++;
		if(lb != ub)
			(*_swap)(lb,ub);
		}
	} while(lb != ub);
	return(lb);
}
turn(lb);
}
turn(lb);
}
