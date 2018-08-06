/*
 *	dspytype -- determine display adapter type
 */

#include <stdio.h>
#include <dos.h>
#include <local\bioslib.h>
#include <local\video.h>

#define MDA_SEG	0xB000
#define CGA_SEG 0xB800

main()
{
	extern int memchk(unsigned int, unsigned int);
	int mdaflag, egaflag, cgaflag;
	int ega_mem, ega_mode;
	unsigned int features, switches;
	static int memtab[] = {
		64, 128, 192, 256
	};

	mdaflag = egaflag = cgaflag = 0;
	
	/* look for display adapters */
	if (ega_info(&ega_mem, &ega_mode, &features, &switches))
		++egaflag;
	fputs("Enhanced graphics adapter ", stdout);
	if (egaflag) {
		fputs("installed\n", stdout);
		fprintf(stdout, "EGA memory size = %d-KB\n", memtab[ega_mem]);
		fprintf(stdout, "EGA is in %s mode\n",
			ega_mode ? "monochrome" : "color");
	}
	else
		fputs("not installed\n", stdout);

	if (egaflag && ega_mode == 0) {
		/* look for IBM monochrome memory */
		if (memchk(MDA_SEG, 0))
			++mdaflag;
	}
	else {
		/* look for IBM monochrome memory */
		if (memchk(CGA_SEG, 0))
			++cgaflag;
	}
	fputs("Monochrome adapter ", stdout);
	if (mdaflag)
		fputs("installed\n", stdout);
	else
		fputs("not installed\n", stdout);
	fputs("Color/graphics adapter ", stdout);
	if (cgaflag)
		fputs("installed\n", stdout);
	else
		fputs("not installed\n", stdout);

	/* report video settings */
	getstate();
	fprintf(stdout, "mode=%d width=%d page=%d\n", Vmode, Vwidth, Vpage);

	exit(0);
}
