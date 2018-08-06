/*
 *	message -- routines used to display and clear
 *	messages in a reserved message area
 */

#include "message.h"

MESSAGE Ml;
extern int writec(char, int, int);

/*
 *	set up the message-line manager
 */

void
initmsg(r, c, w, a, pg)
int r;			/* message row */
int c;			/* message column */
int w;			/* width of message field */
unsigned char a;	/* message field video attribute */
int pg;			/* active page for messages */
{
	MESSAGE *mp;
	void clrmsg();

	mp = &Ml;
	mp->m_row = r;
	mp->m_col = c;
	mp->m_wid = w;
	mp->m_attr = a;
	mp->m_pg = pg;
	mp->m_flag = 1;
	clrmsg();
}


/*
 *	showmsg -- display a message and set the message flag
 */

void
showmsg(msg)
char *msg;
{
	MESSAGE *mp;

	mp = &Ml;
	putcur(mp->m_row, mp->m_col, mp->m_pg);
	writec(' ', mp->m_wid, mp->m_pg);
	putstr(msg, mp->m_pg);
	mp->m_flag = 1;
	return;
}


/* 
 *	clrmsg -- erase the message area and reset the message flag
 */

void
clrmsg()
{
	MESSAGE *mp;

	mp = &Ml;
	if (mp->m_flag != 0) {
		putcur(mp->m_row, mp->m_col, mp->m_pg);
		writec(' ', mp->m_wid, mp->m_pg);
		mp->m_flag = 0;
	}
	return;
}
