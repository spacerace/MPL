/* Listing 12-2 */

int	VRcount = 0;			/* vertical interrupt counter */

main()
{
	if ( EnableISR0A() )
	{
	  printf( "\nCan't enable vertical interrupt handler\n" );
	  exit( 1 );
	}

	while (VRcount < 600)
	  printf( "\015Number of vertical interrupts:  %d", VRcount );

	DisableISR0A();
}
