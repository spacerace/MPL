/*
 *	tstsel -- test driver for the "select" functions
 */

#include <stdio.h>
#include <local\std.h>

#define MAXTEST	20
#define NRANGES	10
#define NDIGITS	5

extern struct slist_st {
	long int s_min;
	long int s_max;
} Slist[NRANGES + 1];

main (argc, argv)
int argc;
char **argv;
{
	int i;
	extern int mkslist(char *);
	extern int selected(unsigned int);
	static void showlist();

	if (argc != 2) {
		fprintf(stderr, "Usage: tstsel list\n");
		exit(1);
	}
	printf("argv[1] = %s\n", argv[1]);
	mkslist(argv[1]);
	showlist();
	for (i = 0; i < MAXTEST; ++i)
		printf("%2d -> %s\n", i, selected(i) ? "YES" : "NO");
	exit(0);
}

/*
 *	showlist -- display the contents of the select list
 */

static void
showlist()
{
	int i;

	/* scan the selection list and display values */ 
	for (i = 0; i <= NRANGES; ++i)
		printf("%2d %5ld %5ld\n", i, Slist[i].s_min, Slist[i].s_max);
}
