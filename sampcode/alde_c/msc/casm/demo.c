extern int cadd(int);
extern int cons;
main(argc, argv, envp)
	int argc;
	char **argv;
	char **envp;

	{
	register char **p;
	char ii;
	int ccadd, ccons, ctemp;

	/* print out the argument list for this program */

	for (p = argv; argc > 0; argc--,p++) {
		printf("%s\n", *p);
		}

	/* test the cadd.asm linkup */
 
	ctemp = 100;
	ccadd = cadd(ctemp);
	ccons = cons;
	printf("ccadd =%d\nccons =%d\n", ccadd, ccons);

	/* print out the current environment settings.  Note that
	 * the environment table is terminated by a NULL entry
	 */

	for (p = envp; *p; p++) {
		printf("%s\n", *p);
		}

	exit(0);
	}
