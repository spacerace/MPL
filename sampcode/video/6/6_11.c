/* Listing 6-11 */

struct	EndpointStruct		/* endpoints for clipped line */
{
  int	x1,y1;
  int	x2,y2;
};

struct	RegionStruct		/* rectangular clipping region */
{
  int	Xul;
  int	Yul;
  int	Xlr;
  int	Ylr;
};


union OutcodeUnion	/* outcodes are represented as bit fields */
{
  struct
  {
    unsigned code0 : 1;		/* x < Xul */
    unsigned code1 : 1; 	/* y < Yul */
    unsigned code2 : 1; 	/* x > Xlr */
    unsigned code3 : 1; 	/* y > Ylr */
  }
	ocs;

  int	outcodes;
};


#define	X1	ep->x1
#define	Y1	ep->y1
#define	X2	ep->x2
#define	Y2	ep->y2
#define	XUL	r->Xul
#define	YUL	r->Yul
#define	XLR	r->Xlr
#define	YLR	r->Ylr


Clip(ep,r)
struct	EndpointStruct	*ep;
struct	RegionStruct	*r;
{
	union OutcodeUnion	ocu1,ocu2;
	int 	Inside;
	int	Outside;


	/* initialize 4-bit codes */

	SetOutcodes( &ocu1, r, X1, Y1 );	/* initial 4-bit codes */
	SetOutcodes( &ocu2, r, X2, Y2 );

	Inside  = ((ocu1.outcodes | ocu2.outcodes) == 0);
	Outside = ((ocu1.outcodes & ocu2.outcodes) != 0);

	while (!Outside && !Inside)
	{
	  if (ocu1.outcodes==0)		/* swap endpoints if necessary so */
	  {				/*  that (x1,y1) needs to be clipped */
	    Swap( &X1, &X2 );
	    Swap( &Y1, &Y2 );
	    Swap( &ocu1, &ocu2 );
	  }


	  if (ocu1.ocs.code0)			/* clip left */
	  {
	    Y1 += (long)(Y2-Y1)*(XUL-X1)/(X2-X1);
	    X1 = XUL;
	  }

	  else if (ocu1.ocs.code1)		/* clip above */
	  {
	    X1 += (long)(X2-X1)*(YUL-Y1)/(Y2-Y1);
	    Y1 = YUL;
	  }

	  else if (ocu1.ocs.code2)		/* clip right */
	  {
	    Y1 += (long)(Y2-Y1)*(XLR-X1)/(X2-X1);
	    X1 = XLR;
	  }

	  else if (ocu1.ocs.code3)		/* clip below */
	  {
	    X1 += (long)(X2-X1)*(YLR-Y1)/(Y2-Y1);
	    Y1 = YLR;
	  }

          SetOutcodes( &ocu1, r, X1, Y1 );	      /* update for (x1,y1) */

	  Inside  = ((ocu1.outcodes | ocu2.outcodes) == 0); /* update       */
	  Outside = ((ocu1.outcodes & ocu2.outcodes) != 0); /*  4-bit codes */
	}

	return( Inside );
}

	
SetOutcodes( u, r, x, y )
union OutcodeUnion	*u;
struct RegionStruct	*r;
int	x,y;
{
	u->outcodes = 0;
	u->ocs.code0 = (x < XUL);
	u->ocs.code1 = (y < YUL);
	u->ocs.code2 = (x > XLR);
	u->ocs.code3 = (y > YLR);
}

	
Swap( pa, pb )
int	*pa,*pb;
{
	int	t;

	t = *pa;
	*pa = *pb;
	*pb = t;
}
