/*
 *	epson.h -- header for Epson printer control functions
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
