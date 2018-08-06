/*
This is a collection of miscellaneous routines written in CI-C86.  They use
some of CI's library routines, although these are either standard or easily
written.  I *think* the only non-standard one is sysint(irpt, regs, regs),
where irpt is the interrupt desired, and regs are defined as:
 
	struct reg_set { int ax, bx, cx, dx, si, di, ds, es; } *regs;
 
The functionality should be obvious.  I make no claims; specifically, I did
not clean them up.  They aren't well documented, but are small and simple,
so you shouldn't have many problems.
 
The file format is CI's ARCHive format.  If you don't have ARCH, '-ARCHIVE-'
starts lines separating files.  Just split them there with an editor (and get
rid of the line).  The file name and size is also on the separator line.
 
Questions, comments, flames, vegetables, etc. can be sent to Alan Batie via:
 
 
         >>----->>-------------( Nathan C. Myers )-----------------\
               /                                                    |
              |  ...!decvax!tektronix!ogcvax!hp-pcd!orstcs!nathan   |
              |           nathan.oregon-state@RAND-RELAY            |
               \___________________________________________________/
 
 
---------------------------------Tear off here---------------------------------
*/
-ARCHIVE- beep.c 402
beep(freq, length)
	int freq;
	double length;
	{
	int port_stat;
	int divisor;
 
	divisor = (long) 1193180 / (long) freq;
 
	outportb(0x43, 0xB6);	/*  Speaker control port  */
	outportb(0x42, (divisor & 0xFF));	/*  Set frequency  */
	outportb(0x42, ((divisor >> 8) & 0xFF));
 
	port_stat = inportb(0x61);
	outportb(0x61, port_stat | 0x03);
	pause(length);
	outportb(0x61, port_stat);
	}
-ARCHIVE- itoa.c 830
/*
 *  This itoa is not perfect by any means, but at least it doesn't call
 *  in the floating point library (sprintf), and is certainly quicker.
 *
 *  Written by Alan Batie & Nathan Myers, 3-1-1984
 */
 
itoa(val, buf)
	int val;
	char *buf;
	{
	int buf_idx;
	int j;
	int div_idx;
	int first;
	static int divisors[] = { 10000, 1000, 100, 10, 1 };
 
	buf_idx = 0;
 
 
	if (val == 0)
		{
		buf[0] = '0';
		buf[1] = '\0';
		return;
		}
	else if (val == -32768)
		{
		strcpy(buf, "-32768");
		return;
		}
 
	if (val < 0)
		{
		buf[buf_idx++] = '-';
		val = -val;
		}
 
	first = 1;
	div_idx = 0;
 
	while (val)
		{
		j = val / divisors[div_idx];
		if (j != 0 || first != 1)
			{
			buf[buf_idx++] = j + '0';
			first = 0;
			}
		val %= divisors[div_idx++];
		}
 
	buf[buf_idx] = '\0';
	}
-ARCHIVE- pause.c 133
#define DELAY 25400
 
pause(seconds)
double seconds;
	{
	long timer;
 
	for (timer = seconds * DELAY; timer > 0; timer--);
	}
-ARCHIVE- timedate.c 874
/*
 *  File name:		timedate.c
 *
 *  Belongs to:		Extended C library
 *  Description:	Routines to return the current system date and time
 *
 *  Version:		1.1
 *  History:		x/x/84 - Original code
 *  Last change:	x/x/84 - Cleaned up for version 2 library
 *  Author:		Alan Batie
 */
 
#include "stdio.h"
#include "stdext.h"
#include "timedate.h"
 
get_date( date )
struct date_struct *date;
{
  struct reg_set regs;
 
  regs.ax = 0x2A00;
  sysint(0x21, &regs, &regs);
 
  date->month = (regs.dx >> 8) & 0xFF;
  date->day = regs.dx & 0xFF;
  date->year = regs.cx;
}
 
get_time( time )
struct time_struct *time;
{
  struct reg_set regs;
 
  regs.ax = 0x2C00;
  sysint(0x21, &regs, &regs);
 
  time->hours = (regs.cx >> 8) & 0xFF;
  time->minutes = regs.cx & 0xFF;
  time->seconds = (regs.dx >> 8) & 0xFF;
  time->centons = regs.dx & 0xFF;
}
-ARCHIVE- timedate.h 544
 
/*  timedate.h - Structure definitions for use with the library routines
		 get_time and get_date (centons are hundredths of a second)  */
 
struct date_struct { int month, day, year; };
struct time_struct { int hours, minutes, seconds, centons; };
 
#define clear_date(dt) dt.month = dt.day = dt.year = 0
#define clear_time(tm) tm.hours = tm.minutes = tm.seconds = tm.centons = 0
 
/*
 *  extern get_date(cur_date)
 *      struct date_struct *cur_date;
 *
 *  extern get_time(cur_time)
 *      struct time_struct *cur_time;
 */
-ARCHIVE- timediff.c 436
#include "timedate.h"
 
long time_diff(xtime, ytime)
	struct time_struct *xtime;
	struct time_struct *ytime;
	{
	register long diff;
 
	diff = xtime->hours * 3600000L;
	diff += xtime->minutes * 60000L;
	diff += xtime->seconds * 1000L;
	diff += xtime->centons * 10L;
 
	diff -= ytime->hours * 3600000L;
	diff -= ytime->minutes * 60000L;
	diff -= ytime->seconds * 1000L;
	diff -= ytime->centons * 10L;
 
	return(diff);
	}


