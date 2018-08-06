/* Listing 8-3 */

#define	BLOCKED		1
#define	UNBLOCKED	2
#define	TRUE		1
#define	FALSE		0

struct	BPstruct			/* table of border pixels */
{
	int	x,y;
	int	flag;
}
	BP[3000];			/* (increase if necessary) */

int	BPstart;			/* start of table */
int	BPend = 0;			/* first empty cell in table */
int	FillValue;			/* value of pixels in filled region */
int	BorderValue;			/* value of pixels in border */


BorderFill( x, y )
int	x,y;
{
	do				/* do until entire table has been scanned */
	{
	  TraceBorder( x, y );		/* trace border starting at x,y */
	  SortBP( BP );			/* sort the border pixel table */
	  ScanRegion( &x, &y );		/* look for holes in the interior */
	}
	while (BPstart < BPend);

	FillRegion();			/* use the table to fill the interior */
}


ScanRegion( x, y )
int	*x,*y;
{
	int	i = BPstart;
	int	xr;

	while (i<BPend)
	{
	  if (BP[i].flag == BLOCKED)		/* skip pixel if blocked */
	    ++i;

	  else
	  if (BP[i].y != BP[i+1].y)		/* skip pixel if last in line */
	    ++i;

	  else
	  {					/* if at least one pixel to fill .. */
	    if (BP[i].x < BP[i+1].x-1)		/* .. scan the line */
	    {
	      xr = ScanRight( BP[i].x+1, BP[i].y );

	      if (xr<BP[i+1].x)			/* if a border pixel is found .. */
	      {
		*x = xr;			/* .. return its x,y coordinates */
		*y = BP[i].y;
		break;
	      }
	    }

	    i += 2;				/* advance past this pair of pixels */
	  }
	}

	BPstart = i;
}


SortBP()			/* uses Microsoft C library quicksort routine */
{
	int	CompareBP();

	qsort( BP+BPstart, BPend-BPstart, sizeof(struct BPstruct), CompareBP );
}


CompareBP( arg1, arg2 )		/* returns -1 if arg1 < arg2 */
struct	BPstruct  *arg1,*arg2;
{
	int	i;

	i = arg1->y - arg2->y;		/* sort by y-coordinate */
	if (i!=0)
	  return( (i<0) ? -1 : 1 );	/* (return -1 if i<0, 1 if i>0) */

	i = arg1->x - arg2->x;		/* sort by x-coordinate */
	if (i!=0)
	  return( (i<0) ? -1 : 1 );

	i = arg1->flag - arg2->flag;	/* sort by flag */
	  return( (i<0) ? -1 : 1 );
}


FillRegion()
{
	int	i;


	for(i=0; i<BPend;)
	{
	  if (BP[i].flag == BLOCKED)		/* skip pixel if blocked */
	    ++i;

	  else
	  if (BP[i].y != BP[i+1].y)		/* skip pixel if last in line */
	    ++i;

	  else
	  {					/* if at least one pixel to fill .. */
	    if (BP[i].x < BP[i+1].x-1)		/* .. draw a line */
	      Line( BP[i].x+1, BP[i].y, BP[i+1].x-1, BP[i+1].y, FillValue );

	    i += 2;
	  }
	}
}


/* border tracing routine */

struct	BPstruct CurrentPixel;
int	D;				/* current search direction */
int	PrevD;				/* previous search direction */
int	PrevV;				/* previous vertical direction */


TraceBorder( StartX, StartY )
int	StartX,StartY;
{
	int	NextFound;		/* flags */
	int	Done;

/* initialize */

	CurrentPixel.x = StartX;
	CurrentPixel.y = StartY;

	D = 6;				/* current search direction */
	PrevD = 8;			/* previous search direction */
	PrevV = 2;			/* most recent vertical direction */

/* loop around the border until returned to the starting pixel */

	do
	{
	  NextFound = FindNextPixel();
	  Done =
	   (CurrentPixel.x == StartX) && (CurrentPixel.y == StartY);
	}
	while (NextFound && !Done);

/* if only one pixel in border, add it twice to the table */

	if (!NextFound)				/* pixel has no neighbors */
	{
	  AppendBPList( StartX, StartY, UNBLOCKED );
	  AppendBPList( StartX, StartY, UNBLOCKED );
	}

/* if last search direction was upward, add the starting pixel to the table */

	else
	if ( (PrevD <= 3) && (PrevD >= 1) )
	  AppendBPList( StartX, StartY, UNBLOCKED );
}	


FindNextPixel( )
{
	int	i;
	int	flag;


	for (i=-1; i<=5; i++)
	{
	  flag = FindBP( (D+i) & 7 );		/* search for next border pixel */
	  if (flag)				/* flag is TRUE if found */
	  {
	    D = (D+i) & 6;			/* (D+i) MOD 2 */
	    break;				/* exit from loop */
	  }
	}

	return(flag);
}


FindBP( d )
int	d;			/* direction to search for next border pixel */
{
	int	x,y;

	x = CurrentPixel.x;
	y = CurrentPixel.y;


	NextXY( &x, &y, d );		/* get x,y of pixel in direction d */

	if ( BorderValue == ReadPixel( x, y ) )
	{
	  AddBPList( d );		/* add pixel at x,y to table */
	  CurrentPixel.x = x;		/* pixel at x,y becomes current pixel */
	  CurrentPixel.y = y;
	  return( TRUE );
	}

	else
	  return( FALSE );
}


NextXY( x, y, Direction )
int	*x,*y;
int	Direction;
{
	switch( Direction )		/*   3 2 1   */
	{				/*   4   0   */
	  case 1:			/*   5 6 7   */
	  case 2:
	  case 3:

		*y -= 1;		/* up */
		break;
	  case 5:
	  case 6:
	  case 7:
		*y += 1;		/* down */
		break;
	}

	switch(Direction)
	{
	  case 3:
	  case 4:
	  case 5:
		*x -= 1;		/* left */
		break;
	  case 1:
	  case 0:
	  case 7:
		*x += 1;		/* right */
		break;
	}
}


AddBPList( d )
int	d;
{
	if (d == PrevD)
	  SameDirection();

	else
	{
	  DifferentDirection( d );
	  PrevV = PrevD;			/* new previous vertical direction */ 
	}
	
	PrevD = d;				/* new previous search direction */
}

SameDirection()
{
	if (PrevD == 0)				/* moving right ... */
	  BP[BPend-1].flag = BLOCKED;		/* .. block previous pixel */

	else
	if (PrevD != 4)				/* if not moving horizontally */
	  AppendBPList( CurrentPixel.x, CurrentPixel.y, UNBLOCKED );
}


DifferentDirection( d )
int	d;
{

/* previously moving left */

	if (PrevD == 4)
	{
	  if (PrevV == 5)			/* if from above .. */
	    BP[BPend-1].flag = BLOCKED;		/* .. block rightmost in line */

	  AppendBPList( CurrentPixel.x, CurrentPixel.y, BLOCKED );
	}

/* previously moving right */

	else
	if (PrevD == 0)				/* previously moving right .. */
	{
	  BP[BPend-1].flag = BLOCKED;		/* .. block rightmost in line */

	  if (d == 7)				/* if line started from above */
	    AppendBPList( CurrentPixel.x, CurrentPixel.y, BLOCKED );
	  else
	    AppendBPList( CurrentPixel.x, CurrentPixel.y, UNBLOCKED );
	}

/* previously moving in some vertical direction */

	else
	{
	  AppendBPList( CurrentPixel.x, CurrentPixel.y, UNBLOCKED );

/* add pixel twice if local vertical maximum or minimum */

	  if ( ( (d>=1) && (d<=3) ) && ( (PrevD >= 5) && (PrevD <= 7) ) ||
	       ( (d>=5) && (d<=7) ) && ( (PrevD >= 1) && (PrevD <= 3) ) )
	    AppendBPList( CurrentPixel.x, CurrentPixel.y, UNBLOCKED );
	}
}


AppendBPList( p, q, f )
int	p,q;			/* pixel x,y coordinates */
int	f;			/* flag */
{
	BP[BPend].x = p;
	BP[BPend].y = q;
	BP[BPend].flag = f;

	++BPend;		/* increment past last entry in table */
}


/* routine to scan a line for a border pixel */

int	Xmax;			/* largest valid pixel x-coordinate */

ScanRight( x, y )
int	x,y;
{
	while ( ReadPixel( x, y ) != BorderValue )
	{
	  ++x;				/* increment x */
	  if (x==Xmax)			/* if end of line in buffer .. */
	    break;			/* .. exit from the loop */
	}

	return( x );
}
