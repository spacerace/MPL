#include <d:\usr\stdlib\time.h>

long start_timer(t)
TIMER *t;
/*
 *	Start a 200Hz timer.  This timer value can later be checked with
 *	time_since() to determine elapsed time.  These functions provide
 *	a very low-overhead way of timing events.
 */
{
	asm("	clr.l	-(sp)		");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* enter supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	$4BA,-(sp)	");	/* save system clock value */
	asm("	move.l	d0,-(sp)	");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* exit supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	(sp)+,d0	");
	asm("	move.l	$8(a6),a0	");	/* grab pointer to timer */
	asm("	move.l	d0,(a0)		");	/* return clock value */
}

long time_since(t)
TIMER *t;
/*
 *	Returns the number of 200Hz ticks since start_timer() was called
 *	for timer <t>.
 */
{
	asm("	clr.l	-(sp)		");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* enter supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	$4BA,-(sp)	");	/* save system clock value */
	asm("	move.l	d0,-(sp)	");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* exit supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	(sp)+,d0	");
	asm("	move.l	$8(a6),a0	");	/* grab pointer to timer */
	asm("	sub.l	(a0),d0		");	/* return elapsed time */
}

sleep(dt)
int dt;
/*
 *	Suspend operation for <dt> seconds.  This is implemented as a
 *	start_timer()/time_since() tight loop waiting for the specified
 *	amount of time to pass.  In a multi-tasking environment, this
 *	function should be replaced by a call which will de-activate
 *	this task for a period of time, allowing other tasks to run.
 */
{
	long t, start_timer(), time_since();
	register long tt;

	tt = ((long) dt) * 200L;
	start_timer(&t);
	while(time_since(&t) < tt)
		;
}
