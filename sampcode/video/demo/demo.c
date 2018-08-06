/******************************************************************************
*                                                                             *
* Name:  demo.c                                                               *
*                                                                             *
* Description:  This program shows how assembly language video graphics       *
*                can be built into a high-level program.                      *
*                                                                             *
*                                                                             *
* Notes:        Use  MAKE DEMO  to build this program.  You need the          *
*                following tools:                                             *
*                                                                             *
*                    - Microsoft C                                            *
*                    - Microsoft Macro Assembler                              *
*                    - LINK                                                   *
*                                                                             *
*                                                                             *
*               Use an IBM CGA, EGA, or VGA, or compatible hardware.          *
*                                                                             *
******************************************************************************/

#define	TRUE		1
#define	FALSE		0

#define	CGA		2
#define	EGA		3
#define	MCGA		4
#define	VGA		5

#define	Xmax		319
#define	Ymax		199

#define FivePI          (double)(5*3.1415926535)

main()
{
	static struct			/* for video ID */
	{
	  char	Subsystem;
	  char	Display;
	}		VIDstruct[2];

	int	VIDok = FALSE;

	int	x,y,Fgd;
	int	i;
	double	sin();


/* verify presence of CGA-compatible hardware */

	VideoID( VIDstruct );

	for( i=0; (i<=1) && !VIDok; i++ )
	  VIDok = ( VIDstruct[i].Subsystem == CGA ) ||
	          ( VIDstruct[i].Subsystem == EGA ) ||
	          ( VIDstruct[i].Subsystem == MCGA ) ||
	          ( VIDstruct[i].Subsystem == VGA );
	  
	if( !VIDok )
	{
	  printf( "\nCan't find compatible video hardware\n");
	  exit( 1 );
	}

/* draw something in 320x200 4-color mode */

	SetVmode( 4 );			/* BIOS mode 4 is 320x200 4-color */

	Line04( 0, Ymax/2, Xmax, Ymax/2, 3 );

	for( x=0; x<=Xmax; x++ )
	{
	  y = sin( FivePI * (double)x/(double)Xmax ) * (double)(Ymax/2);
	  y += Ymax/2;

	  Fgd = ( x%10 ? 2 : 3 );	/* select foreground pixel value */

	  Line04( x, y, x, Ymax/2, Fgd );	/* draw a line or two */
	  Line04( x, Ymax-y, x, Ymax/2, 5-Fgd );
	}
}
