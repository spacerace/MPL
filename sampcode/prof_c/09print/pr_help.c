/*
 *	pr_help -- display an abbreviated manual page
 */

#include <stdio.h>
#include <local\std.h>

void
pr_help(pname)
char *pname;
{
	static char *m_str[] = {
		"The following options may be used singly or in combination:",
		"-e\t set Epson-compatible mode",
		"-f\t use formfeed to eject a page (default is newlines)",
		"-g\t use generic printer mode",
		"-h hdr\t use specified header instead of file name",
		"-l len\t set page length in lines (default = 66)",
		"-n\t enable line numbering (default = off)",
		"-o cols\t offset from left edge in columns (default = 5)",
		"-p\t preview output on screen (may be redirected)",
		"-s list\t print only selected pages",
		"-w cols\t line width in columns (default = 80)"
	};
	int i, n = sizeof (m_str)/ sizeof (char *);

	fprintf(stderr, "Usage: %s [options] file...\n\n", pname);
	for (i = 0; i < n; ++i)
		fprintf(stderr, "%s\n", m_str[i]);

	return;
}
