

main()
{
	static char *names[6] = { "Yankees\0",
			"Tigers\0",
			"Blue Jays\0",
			"Brewers\0",
			"Red Sox\0",
			"Orioles\0" };
	
	static int wins[6] = {57, 55, 52, 48, 44, 29};
	static int losses[6] = {18, 20, 23, 27, 31, 46};
	int i;
	
	create_BIFF("ALEast.xls");
	dimension_BIFF(0, 5, 1, 3);
	for (i = 0; i < 6; i++){
		save_BIFF(8, i, 1, names[i]);
		save_BIFF(2, i, 2, (char *) &wins[i]);
		save_BIFF(2, i, 3, (char *) &losses[i]);
	}
	close_BIFF();
}

