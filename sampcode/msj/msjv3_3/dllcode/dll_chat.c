/********************************************************************
*	DLL_CHAT.C  - A demonstration program using a demo DLL
*
*	(C) 1988, By Ross M. Greenberg for Microsoft Systems Journal
*
*	This is the main body of the CHAT program, interfacing with
*	and calling the DLL as if it were a bunch of routines 
*	available with a far call:  which it is!
*
*	Compile with:
*	cl /c chat.c
*	link chat,chat,,slibce+doscalls,chat
*
*	Remember: move the DLL itself into your DLL library directory
*
********************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	"chat.h"

#define	TRUE	1
#define	FALSE	0
#define	OK	TRUE
#define	NOT_OK	FALSE

#define	MAX_MSG		100
#define	MAX_MSG_LEN	80
#define	NULLP		(void *)NULL

/* The following OS/2 system calls are made in this module: */

extern far pascal dosexitlist();
extern far pascal dossleep();


/* The following DLL system calls are made in this module: */

extern far _loadds pascal login();
extern far _loadds pascal logout();
extern far _loadds pascal get_msg_cnt();
extern far _loadds pascal get_msg();
extern far _loadds pascal send_msg();


/********************************************************************
*	This is where the messages are stored, once received and 
*	formatted. This could probably be replaced easily with a call 
*	to calloc(), but then we wouldn't have to block on being a 
*	background process
********************************************************************/

char	msg_array[MAX_MSG + 1][MAX_MSG_LEN];

/* Must be global so that before_death() can access it to logout */

int	my_id = 0;

#define	MAX_SLEEP	2

/********************************************************************
*	before_death()
*
*	Called the coins are lowered onto the eyes of this invokation 
*	of CHAT. Any exit will cause this routine to be called. After 
*	this routine calls the DLL logout procedure, it removes calls 
*	the next exitroutine in the exit list.
********************************************************************/

void	far	before_death()
{
	logout(my_id);
	dosexitlist(3, before_death);
}

/********************************************************************
*	main()
*
*	After logging in (which returns a unique login id), the 
*	before_death() routine is added onto the exitlist.  Then the 
*	main loop:
*
*	If there are any messages, read them into out memory buffer 
*	(provided there is room) and kick up the count.  After 
*	retrieving all the msgs which will fit, call the display 
*	routine for each msg. Zero the count of messages when done.
*
*	Every couple of seconds, sleep for a little while (as if a 
*	human typing on a keyboard), then send the message to all 
*	other members of CHAT.
*
*	CHAT can only be exited (in its current form) with a 
*	control-C or error condition.
********************************************************************/

main()
{
int	msg_cnt = 0;
int	msg_id = 0;
int	lp_cnt;
char	tmp_buf[MAX_MSG_LEN];
int	m_cnt;

	printf("Logged into CHAT as user:%d\n", my_id = login());

	dosexitlist(1, before_death);

	while	(TRUE)
	{
		for (m_cnt = get_msg_cnt(my_id); m_cnt ; m_cnt--)
		{
			get_msg(my_id, (char far *)tmp_buf);
 			if	(msg_cnt <= MAX_MSG)
				sprintf(msg_array[msg_cnt++],"(%d)%s",my_id,tmp_buf);
	}

		if	(ok_to_disp())
		{
			for (lp_cnt = 0 ; lp_cnt < msg_cnt; lp_cnt++)
				disp_msg(msg_array[lp_cnt]);
			if	(msg_cnt > MAX_MSG)
				disp_msg("Looks like you might have lost some") 
				disp_msg(" messages while you were away\n");
			msg_cnt = NULL;
		}

		if	(rand() % (my_id + 1))
		{
			dossleep((long)(rand() % MAX_SLEEP) * 1000L);
			sprintf(tmp_buf, "Test message #%d from Session #%d\n", 
					msg_id++, my_id);
			if	(send_msg(my_id, (char far *)tmp_buf) == NOT_OK)
				printf("?Can't send a message....\n");
		}
	}
}

disp_msg(ptr)
char	*ptr;
{
	printf("%s", ptr);
	fflush(stdout);
}

extern far pascal dosgetinfoseg();

ok_to_disp()
{
struct gdtinfoarea far *gdt;
struct ldtinfoarea far *ldt;
unsigned	gseg;
unsigned	lseg;

	dosgetinfoseg((char far *)&gseg, (char far *)&lseg);
	gdt = (struct gdtinfoarea far *)((long)gseg << 16);
	ldt = (struct ldtinfoarea far *)((long)lseg << 16);

	return( gdt->foreground_process_id == ldt->parent_pid);
}
