/* Program: tone.c */
/* #include <tone.c> */
/* Usage: tone(freq,time); */

#define TIMERMODE 182		/* Code to put timer in right mode */
#define FREQSCALE 1190000L	/* basic time frequency in Hz */
#define TIMESCALE 1230L		/* number of counts in .1 second */
#define T_MODEPORT 67		/* port controls timer mode */
#define FREQPORT 66			/* port controls tone frequency */
#define BEEPPORT 97			/* port controls speaker */
#define ON 79				/* signal to turn speaker on */

tone(freq,time)
int freq, time;
{
	int hibyt, lobyt, port;
	long i, count, divisor;

	divisor=FREQSCALE/freq;		/* scale frequency to timer units */
	lobyt=divisor % 256;		/* break integer into */
	hibyt=divisor / 256;		/* two bytes */
	count=TIMESCALE*time;		/* convert time to TIMER unites */
	outp(T_MODEPORT,TIMERMODE);	/* prepare timer for input */
	outp(FREQPORT,lobyt);		/* set low byte of timer register */
	outp(FREQPORT,hibyt);		/* set high byte of timer register */
	port=inp(BEEPPORT);		/* save port setting */
	outp(BEEPPORT,ON);		/* turn speaker on */
	for(i=0; i<count; i++)
	;
	outp(BEEPPORT,port);		/* turn speaker off, restore setting */
}
                                                                                                                         