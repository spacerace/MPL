#define	CLKCMD	0X5A		/* Command port for Godbout SS 1 clock */
#define CDATA	CLKCMD+1	/* Data port for Godbout SS 1 clock */
#define	CLOCK	0 
#define SEC1	CLOCK		/* Unit seconds digit address */
#define SEC10	CLOCK+1		/* Tens of seconds digit address */
#define MIN1	CLOCK+2		/* Unit miniutes digit address */
#define MIN10	CLOCK+3		/* Tens of minutes digit address */
#define HOUR1	CLOCK+4		/* Unit hours digit address */
#define HOUR10	CLOCK+5		/* Tens of hours digit address */
#define WDAY	CLOCK+6		/* Day of week digit address */
#define DAY1	CLOCK+7		/* Unit days digit address */
#define DAY10	CLOCK+8		/* Tens of days digit address */
#define MONTH1	CLOCK+9		/* Unit months digit address */
#define	MONTH10 CLOCK+10	/* Tens of months digit address */
#define YEAR1	CLOCK+11	/* Unit years digit address*/
#define	YEAR10	CLOCK+12	/* Tens of years digit address*/
#define MASK	0X000F		/* Digit bit mask, lower 4 bits BDC digit */
#define	CREAD	0X10		/* Read bit */
#define CWRITE  0X20		/* Write bit */
#define CHOLD   0X40		/* Hold bit */