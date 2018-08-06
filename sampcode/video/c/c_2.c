/* Listing C-2 */

main()
{
	char	*SubsystemName();
	char	*DisplayName();

	static struct
	{
	  char	Subsystem;
	  char	Display;
	}
			VIDstruct[2];


	/* detect video subsystems */

	VideoID( VIDstruct );


	/* show results */

	printf( "Video subsystems in this computer:" );

	printf( "\n  %s (%s)", SubsystemName(VIDstruct[0].Subsystem),
	  DisplayName(VIDstruct[0].Display) );

	if ( VIDstruct[1].Subsystem )
	  printf( "\n  %s (%s)", SubsystemName(VIDstruct[1].Subsystem),
	    DisplayName(VIDstruct[1].Display) );
}


char *SubsystemName( a )
char	a;
{
	static char *IBMname[] =
	{
	  "(none)",
	  "MDA",
	  "CGA",
	  "EGA",
	  "MCGA",
	  "VGA"
	};

	static char *Hercname[] =
	{
	  "HGC",
	  "HGC+",
	  "InColor"
	};

	if ( a & 0x80 )
	  return ( Hercname[a & 0x7F] );
	else
	  return( IBMname[a] );
}


char *DisplayName( d )
char	d;
{
	static char *name[] =
	{
	  "(none)",
	  "MDA-compatible monochrome display",
	  "CGA-compatible color display",
	  "EGA-compatible color display",
	  "PS/2-compatible monochrome display",
	  "PS/2-compatible color display"
	};

	return( name[d] );
}
