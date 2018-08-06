
#include <stdio.h>

main(argc, argv)	/* set a display mode */
int		argc;
char	*argv[];
{
	int		m;

	if (argc != 2) /* only one arg allowed on command line */
		help();
	m = atoi(argv[1]);
	if ((m < 0) || (m > 6)) /* arg out of range */
		help();
	set_mode(m);
}


help()
{
	puts("\nSetScrn -- ver 1.0 by Ray McVay\n");
	puts("\nUsage: setscrn n\n");
	puts(  "where: n = 0 = 40x25 BW\n");
	puts(  "     : n = 1 = 40x25 COLOR\n");
	puts(  "     : n = 2 = 80x25 BW\n");
	puts(  "     : n = 3 = 80x25 COLOR\n");
	puts(  "     : n = 4 = 320x200 COLOR\n");
	puts(  "     : n = 5 = 320x200 BW\n");
	puts(  "     : n = 6 = 640x200 BW\n");
	exit(1);
}
