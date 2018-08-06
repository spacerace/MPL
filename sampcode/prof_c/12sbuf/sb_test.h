/*
 *	sb_test.h -- header for sb_test program
 */

/* dimensions of the display windows */
#define CMND_ROW	0
#define CMND_COL	0
#define CMND_HT		1
#define CMND_WID	SB_COLS
#define STAT_ROW	CMND_HT
#define STAT_COL	0
#define STAT_HT		1
#define STAT_WID	SB_COLS
#define TEXT_ROW	(CMND_HT + STAT_HT)
#define TEXT_COL	0
#define TEXT_HT		(SB_ROWS - TEXT_ROW)
#define TEXT_WID	SB_COLS
#define HELP_ROW	5
#define HELP_COL	5
#define HELP_HT		18
#define HELP_WID	70
