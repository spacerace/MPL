/*
 *	printer.h -- header for printer control functions
 */


/* ASCII codes used for Epson MX/FX-series printer control */
#define DC2	18	/* cancel condensed type mode */
#define DC4	20	/* cancel expanded type mode */
#define ESC	27	/* signal start of printer control sequences */
#define FF	12	/* top of page next page */
#define SO	14	/* start expanded type mode */
#define SI	15	/* start condensed type mode */

/* font types */
#define NORMAL		0x00
#define CONDENSED	0x01
#define DOUBLE		0x02
#define	EMPHASIZED	0x04
#define EXPANDED	0x08
#define ITALICS		0x10
#define UNDERLINE	0x20

/* miscellaneous constants */
#define MAXPSTR	32	/* maximum printer control string length */

/* primary printer control data structure */
typedef struct printer_st {
	/* hardware initialize/reset */
	char p_init[MAXPSTR];	

	/* set option strings and codes */
	char p_bold[MAXPSTR];	/* bold (emphasized) on */
	char p_cmp[MAXPSTR];	/* compressed on */
	char p_ds[MAXPSTR];	/* double strike on */
	char p_exp[MAXPSTR];	/* expanded (double width) on */
	char p_ul[MAXPSTR];	/* underline on */
	char p_ital[MAXPSTR];	/* italic on */

	/* reset option strings and codes */
	char p_norm[MAXPSTR];	/* restore normal font */
	char p_xbold[MAXPSTR];	/* bold (emphasized) off */
	char p_xcmp[MAXPSTR];	/* compressed off */
	char p_xds[MAXPSTR];	/* double strike off */
	char p_xexp[MAXPSTR];	/* expanded (double width) off */
	char p_xul[MAXPSTR];	/* underline off */
	char p_xital[MAXPSTR];	/* italic off */
} PRINTER;
