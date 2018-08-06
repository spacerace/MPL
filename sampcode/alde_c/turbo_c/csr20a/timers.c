/* timers.c
 *
 *	Return elapsed time (in tenths of seconds)
 *	since kick-off of timer.
 *
 *	D. Perras	03 Jan 1987
 *  Datalight C
 *
 *      B. Pritchett	05 Jan 1987
 *  Microsoft C Modifications
 *
  *	Routines included:
 *
 *		void	init_tmr()		Initialize the timer for operation
 *		void	start_tmr()		Start the timer counting
 *	unsigned	stop_tmr()		Stop the timer, return results
 *	unsigned	read_tmr()		Read the elapsed time since start_tmr()
 *		void	reset_tmr()		Reset the timer does not return elapsed time
 *		void	zero_tmr()		Zero the timer (relative), keep it running
 *
 * 		long 	get_timer()		Return the PC's current timer (internal)
 */

#include <dos.h>  /* This is to include the REGS structure. */
    		      /* It can be changed as needed for the compiler */


#define	MSC	1		/* references union: regs.x.cx vs. regs.cx */

long	timers[10], t_dvsr;
int	timer_flag[10];

#define	 INIT_FLG	1
#define	 STRT_FLG	2
#define	 TMR_ON(j)		(timer_flag[(j)] & STRT_FLG)
#define	 TMR_INIT(j)	(timer_flag[(j)] & INIT_FLG)
#define  TIMER_INT	0x1A

#define	 ERRVAL		0

unsigned init_tmr()
{
/*	This routine does two timer calls and compares the return values
 *  just to be sure the timer is working.
 *	Return ERRVAL if error found, otherwise returns (unsigned) tm1
 */

	int i;
	unsigned tm1;
	long tmv, get_timer();
	
	for (i=0; i<10; i++) 
		timer_flag[i] = timers[i] = 0;
	tmv = get_timer();

	for (i=0; i<16000; i++)
		;					/* Null 'for' loop to use up some time */
	tm1 = get_timer() - tmv;

/* Currently, we test the result value for non-zero.  However, if the
 * system environment demands it, a value range could be tested and the
 * t_dvsr variable assigned accordingly.  The 182 value is for IBM PC's
 * and compatibles.
 */

	if (tm1 != 0)
		for (i=0; i<10; i++)	
			timer_flag[i] |= INIT_FLG;
	else tm1 = ERRVAL;
	t_dvsr = 182;
	return tm1;
}

void start_tmr(x)
int x;
{
/*	This routine picks up the current time stored by the timer
 *	and puts it in timers[x].
 *	It also set the 'timer started indicator'
 *	It returns nothing.
 */

	long get_timer();

	x = abs(x)%10;

	if (TMR_INIT(x) && !TMR_ON(x)) {
		timers[x] = get_timer();
		timer_flag[x] |= STRT_FLG;
	}
}

unsigned stop_tmr(x)
int x;
{
/*	This routine stops the timer and returns the difference
 *	between the count stored by the start_tmr and the timer
 *	count at the point of stopping.
 *  The difference is made significant to the tenths-of-seconds.
 */

	unsigned tmp;
	long	tmp1, get_timer();

	x = abs(x)%10;

	if (TMR_ON(x)) {
		tmp1 = get_timer()-timers[x];
		timer_flag[x] &= ~STRT_FLG;
		tmp1 *= 100;
		tmp1 /=  t_dvsr;
		timers[x] = 0;
		tmp = tmp1;
		return tmp;
	} else return ERRVAL;
}

unsigned read_tmr(x)
int x;
{
/*	This routine works line stop_tmr except that the timer is
 *	is not stopped.  The elapsed time is returned and the timer
 *	is continued.
 *  Successive calls to read_tmr will return successivelly greater values.
 */

	unsigned tmp;
	long	tmp3, get_timer();

	x = abs(x)%10;

	if (TMR_ON(x)) {
		tmp3 = get_timer()-timers[x];
		tmp3 *= 100;
		tmp3 /= t_dvsr;
		tmp = tmp3;
		return tmp;
	} else return ERRVAL;
}

void reset_tmr(x)
int x;
{
/* This function resets the specified timer to 0
 * and clears the TMR_ON flag.  It does not return
 * a value.
 */

	x = abs(x)%10;

	if (TMR_ON(x)) {
		timers[x] = 0;
		timer_flag[x] &= ~STRT_FLG;
	}
}

void zero_tmr(x)
int x;
{
/*  This function zeros the specified timer relative to the time it
 *  is called.  The timer operation is continued.
 *  It does not return a timer value.
 */

	long get_timer();


	x = abs(x)%10;

	if (TMR_ON(x)) {
		timers[x] = get_timer();
	}
}


long get_timer()
{
/*	This routine returns the value in the PC's timer
 *	It is placed here so that the int86 call can be
 *  changed, if needed, without effecting the code above.
 */


	static long	tmp1;
#ifdef MSC
	static union REGS regs;
#else
	static REGS regs;
#endif
	static int flags;

#ifdef MSC
		regs.x.ax = 0;
		flags = int86(TIMER_INT,&regs,&regs);
		tmp1 = regs.x.cx * 65536 + regs.x.dx;
#else
		regs.ax = 0;
		flags = int86(TIMER_INT,&regs,&regs);
		tmp1 = regs.cx  * 65536 + regs.dx;
#endif
		return(tmp1);
}

/* end of timers.c	*/
