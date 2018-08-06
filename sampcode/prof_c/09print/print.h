/*
 *	print.h	-- header information for print programs
 */

/* default printing format information */
#define BOTTOM	3	/* blank lines at bottom of page */
#define MARGIN	5	/* default margin width in columns */
#define MAXPCOL	80	/* maximum number of printed columns per line */
#define MAX_PS	32	/* maximum length of printer control strings */
#define PAGELEN	66	/* default page length (at 6 lines per inch) */
#define LPI	6	/* default lines per inch */
#define TABSPEC 8	/* default tab separation */
#define TOP1	2	/* blank lines above header line */
#define TOP2	2	/* blank lines below header line */
#define TABSPEC	8	/* tab interval */

/* primary data structure for printer programs */
typedef struct pr_st {
	/* numeric variables */
	int p_top1;	/* lines above header */
	int p_top2;	/* lines below header */
	int p_btm;	/* lines in footer */
	int p_wid;	/* width in columns */
	int p_lmarg;	/* left margin */
	int p_rmarg;	/* right margin */
	int p_len;	/* lines per page */
	int p_lpi;	/* lines per inch */
	int p_lnum;	/* non-zero turns line numbering on */
	int p_mode;	/* zero for generic printer */
	int p_font;	/* font number when in non-generic mode */
	int p_ff;	/* non-zero uses formfeed to eject page */
	int p_tabint;	/* tab interval */

	/* string variables */
	char p_hdr[MAX_PS];
	char p_dest[MAX_PS];
} PRINT;
