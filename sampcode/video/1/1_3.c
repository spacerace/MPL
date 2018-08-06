/* Listing 1-3 */

main( argc, argv )
int	argc;
char	**argv;
{
	int	ModeNumber;
	void	SetVmode();


	if (argc != 2)			/* verify command line syntax */
	{
	  printf( "\nSyntax:  SETVMODE n\n" );
	  exit( 1 );
	}

	sscanf( argv[1], "%x", &ModeNumber );	/* get desired mode number */

	SetVmode( ModeNumber );			/* call ROM BIOS via INT 10h */
}
