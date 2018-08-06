/********************************************************************
*	CHATLIB.C  - A demonstration Dynamic Link Library
*
*	(C) 1988, By Ross M. Greenberg for Microsoft Systems Journal
*
*	This DLL, when used with the CHAT program acts as a central
*	repository for all messages being passed
*
*	Compile with:
*
*	cl /AL /Au /Gs /c chatlib.c
*
*	Note -	Though broken here the following two lines are entered
*			as one line:
*
*	link startup+chatlib+chatmem,chatlib.dll,,llibcdll+doscalls,
*	chatlib/NOE
*
*	Remember to move the DLL itself into your DLL library directory
*
********************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	<malloc.h>
#include	<dos.h>

#define	TRUE	1
#define	FALSE	0
#define	OK	TRUE
#define	NOT_OK	FALSE

#define	NULLP	((char *)NULL)
#define	GET_SEM	(dossemrequest(&semaphore, -1L))
#define	RLS_SEM	(dossemclear(&semaphore))

/* The following OS/2 system calls are made in this module: */

extern far pascal dossemrequest();
extern far pascal dossemclear();


/* The following external calls are made in this module: */

char *my_calloc();


/* This semaphore used to coordinate access to "critical" areas */

long	semaphore = 0;

/*******************************************************************
*	This structure starts defines the members of the linked list
*	which starts at master_ptr.  Once a structure is allocated,
*	it is never released, although the character array member 
*	msg_ptr points to will be released when the message is no longer 
*	needed
********************************************************************/
#define	MSG	struct	_msg
MSG{
	MSG			*next_ptr;	/* Point to next MSG, or NULLM      */
	char		*msg_ptr;	/* Point to the actual message      */
	int			msg_len;	/* length of the message - optional */
	unsigned	f_word;		/* flag_word. When set to 0xfff     */
							/* all chat members have seen this  */
							/* message, so it can be freed      */
	};

int	flag_word = 0xffff;	/* This is the word that f_word is  */
							/* set to initially. It is modified */
							/* so that each bit is "off" if that*/
							/* "user" is logged in              */
			 
#define	NULLM	((MSG *)NULL)
MSG	*master_ptr = NULLM;	/* Where the linked list begins    */

/********************************************************************
*	new_msg_struct(pointer to last MSG)
*
*	Allocates a new MSG, initializes the contents of the structure, 
*	and sets the linked list if not the first time called 
*	(last_msg != NULLM)
*
*	Returns a pointer to the structure allocated, NULLM if an error
********************************************************************/

MSG	*new_msg_struct(MSG *last_msg)
{
MSG	*tmp_ptr;

	if	((tmp_ptr = (MSG *)my_calloc(sizeof(MSG), 1)) == NULLM)
		return(NULLM);

	tmp_ptr->next_ptr = NULLM;

	tmp_ptr->msg_ptr = NULLP;
	tmp_ptr->msg_len = NULL;

	tmp_ptr->f_word = flag_word;

	if	(last_msg != NULLM)
		last_msg->next_ptr = tmp_ptr;

	return(tmp_ptr);
}


/********************************************************************
*	initialize()
*
*	Called either by the initialization routine of the DLL, or by the
*	first login. It allocates the first MSG structure, then allocates
*	and sets up for the next member
********************************************************************/

void far _loadds pascal initialize()
{

	if	((master_ptr = new_msg_struct(NULLP)) == NULLM)
	{
		printf("Couldn't allocate MSG memory for header...\n");
		exit(1);
	}

	new_msg_struct(master_ptr);
}

/********************************************************************
*	login()
*
*	If the master MSG structure hasn't been allocated already by
*	and earlier call to initialize() (by the DLL initialize routine),
*	then make the call now.  Memory has already been allocated, 
*	therefore, so now give ourselves access to the segment we've 
*	allocated.
*
*	Get the next free bit slot in the flag word, set it to indicate
*	it's in use, then return our login id.
********************************************************************/

int far _loadds pascal login()
{
int	log_id;
int	tmp_msk;

	if	(master_ptr == NULLM)
	{
		printf("Init in login\n");
		initialize();
	}

	my_getseg();

	GET_SEM;
	for (log_id= 0 ; log_id < 16 ; log_id++)
	{
		tmp_msk = mask(log_id);
		if	(flag_word & tmp_msk)
		{
			flag_word &= ~tmp_msk;
			RLS_SEM;
			return(log_id);
		}

	}
	RLS_SEM;

	printf("Login slots all used up!\n");
	exit(1);
}

/********************************************************************
*	get_msg_cnt(login_id)
*
*	For every MSG structure in the linked list with an associated
*	message attached to it, increment a counter if the id in question
*	hasn't received it yet, then return that counter when we fall off
*	the end.  
********************************************************************/

int far _loadds pascal get_msg_cnt(int id)
{
MSG	*tmp_ptr;
int	tmp_cnt = 0;
int	tmp_msk = mask(id);


	GET_SEM;
	for(tmp_ptr = master_ptr; tmp_ptr; tmp_ptr = tmp_ptr->next_ptr)
	{
		if	(!(tmp_ptr->f_word & tmp_msk))
			if	(tmp_ptr->msg_len)
				tmp_cnt++;
	}

	RLS_SEM;
	return(tmp_cnt);
}

/********************************************************************
*	send_msg(login_id, pointer_to_message)
*
*	If there are no other "chatter's" logged in, simply return.
*	(Flag_word or'ed with our mask would be 0xfff)
*
*	Find a free MSG structure (guaranteed to have at least one, since
*	every write leaves a free one allocated if its the last one in 
*	the linked list.  
*
*	Allocate memory for the message, copy the message into it, then
*	assign the pointer in the structure and the length of the message.
*	Finally, allocate a new structure if required.
********************************************************************/

int far _loadds pascal send_msg(int id, char far *ptr)
{
MSG	*tmp_ptr = master_ptr;
int	tmp_len = strlen(ptr) + 1;


	if	((flag_word | mask(id)) == 0xffff)
		return(OK);

	GET_SEM;
	while	(tmp_ptr->msg_len)
		tmp_ptr = tmp_ptr->next_ptr;

	if	((tmp_ptr->msg_ptr = my_calloc(tmp_len, 1)) == NULLP)
	{
		printf("Can't allocate %d bytes for msg\n", tmp_len);
		RLS_SEM;
		return(NOT_OK);
	}

	strcpy(tmp_ptr->msg_ptr, ptr);
	tmp_ptr->msg_len = tmp_len;
	tmp_ptr->f_word = (flag_word | mask(id));

	if	(tmp_ptr->next_ptr == NULLM)
	{
		if	(new_msg_struct(tmp_ptr) == NULLM)
		{
			printf("Can't allocate new MSG_header\n");
			free_msg(tmp_ptr);
			RLS_SEM;
			return(NOT_OK);
		}
	}

	RLS_SEM;
	return(OK);
}


/********************************************************************
*	logout(login_id)
*
*	Mark every mesage as read (freeing them if now "totally" read),
*	reset the flag word, and then indicate that the logout worked.
********************************************************************/

int far _loadds pascal logout(int id)
{
MSG	*tmp_ptr;
int	tmp_msk = mask(id);

	GET_SEM;
	for(tmp_ptr = master_ptr; tmp_ptr; tmp_ptr = tmp_ptr->next_ptr)
		mark_msg(id, tmp_ptr);

	flag_word |= mask(id);

	RLS_SEM;

	printf("In logout ... Hit a Key:");fflush(stdout);
	getch();
	printf("\n\n\n\n");

	return(0);
}

/********************************************************************
*	get_msg(login_id, pointer to buffer)
*
*	Find the first message the login_id hasn;t read, then
*	strcpy it into the buffer supplied. Then mark the message as
*	read (freeing as required).
********************************************************************/

int far _loadds pascal get_msg(int id, char far *ptr)
{
MSG	*tmp_ptr = master_ptr;
int	tmp_msk = mask(id);



	GET_SEM;
	for(tmp_ptr = master_ptr; tmp_ptr; tmp_ptr = tmp_ptr->next_ptr)
	{
		if	(!(tmp_ptr->f_word & tmp_msk))
		{
			strcpy(ptr, tmp_ptr->msg_ptr);
			mark_msg(id, tmp_ptr);
			RLS_SEM;
			return(TRUE);
		}
	}
	RLS_SEM;
	return(FALSE);
}

/********************************************************************
*	mark_msg(login id, pointer to message structure)
*
*	Mark our bit in the MSG f_word as set.  If then set to 0xffff,
*	the message is "totally" read, so free it.
*
*	******************************************************************
*
*	free(pointer to message structure)
*      
*	If there is a string associated with this structure, free the
*	memory so used, then zero out the pointer and the msg_len
********************************************************************/

mark_msg(int id, MSG *ptr)
{
	ptr->f_word |= mask(id);
	if	(ptr->f_word == 0xffff)
		free_msg(ptr);
}

free_msg(MSG *ptr)
{
	if	(ptr->msg_ptr)
		my_free(ptr->msg_ptr);
 	ptr->msg_ptr = NULLP;
	ptr->msg_len = NULL;
}

						/* GENERAL ROUTINES */

/* This routine merely returns with the bit corresponding 
 * to our login set
 */

mask(int log_id)
{
	return(1 << (log_id - 1));
}
