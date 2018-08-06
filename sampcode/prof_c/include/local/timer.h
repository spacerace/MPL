/*
 *	timer.h -- header for timer control routines
 */

/* timer clock and interrupt rates */
#define TIMER_CLK	1193180L
#define TIMER_MAX	65536L
#define TICKRATE	TIMER_CLK / TIMER_MAX

/* timer port access fro frequency setting */
#define TIMER_CTRL	0x43
#define TIMER_COUNT	0x42
#define TIMER_PREP	0xB6
