/***	mouapi.c - A library of real mode mouse calls compatable with OS/2
 *
 *	A subset of the OS/2 mouse api calls for real mode calls.
 *	The calls and features are designed specifically for LIFE.C.
 *	This is designed as a library to be linked with an application
 *	using BIND.EXE to allow full compatability between real and
 *	protect modes for programs using mouse calls.
 *	Most are merely dummy calls because DOS 3.x doesn't care about
 *	such things.
 *
 *	Contains subsets of:
 *		MouOpen, MouReadEventQue, MouSetDevStatus,
 *		MouGetNumQueEl, MouSetPtrPos
 */

#include <subcalls.h>

/* defines for int33h() calls.	These are the offsets for the various registers
 *     in the register array. */
#define AX 0
#define BX 1
#define CX 2
#define DX 3


/***	MOUOPEN - initializes mouse
 *
 *	Initializes mouse and returns -1 in handle if mouse is present, else
 *	if no mouse is present, returns 0 in handle and error code.
 */
unsigned far pascal
MOUOPEN (name, handle)
char far *name;
unsigned far *handle;
{
	extern void int33h();			/* mouse interupt caller */
	int registers[4];			/* register array for above */

	registers[AX]=0;			/* ax=0, mouse init */
	int33h (registers);			/* do mouse call */
	if (registers[AX] == -1) {		/* was mouse present? */
		*handle=-1;			/* if yes, give dummy handle */
		registers[AX]=15;		/* set mickey to pixel */
		registers[CX]=8;		/*	ratio to 1:1 */
		registers[DX]=8;
		int33h(registers);
		return (0);			/* and return no error */
	}
	else {
		*handle=0;
		return (385);			/* else, return no mouse err */
	}
}


/***	MOUREADEVENTQUE - get mouse event
 *
 *	Gets events like the OS/2 MouReadEventQue but this one ignores
 *	the event mask and EventType parameter, never waiting for events,
 *	just returning a 0 mask if nothing happened since last call.
 *	Returns position absolute pixel coordinates
 */
unsigned far pascal
MOUREADEVENTQUE (buffer, type, handle)
	struct EventInfo far *buffer;
	unsigned far *type;
	unsigned handle;
{
	extern void int33h();			/* mouse interupt caller */
	int registers[4];			/* register array for above */
	static lastrow; 			/* position at last call */
	static lastcol;

	buffer->Time=-1L;			/* put in dummy time figure */
	registers[AX]=3;			/* ax=3, get mouse status */
	int33h (registers);			/* do mouse call */
	buffer->Mask=0; 			/* first, blank button info */
	if (registers[DX]-lastrow || registers[CX]-lastcol) {
		buffer->Row=registers[DX];	/* return row */
		buffer->Col=registers[CX];	/* return column */
		lastrow=registers[DX];		/* save for next time */
		lastcol=registers[CX];
		if (registers[BX] & 1)		/* if the left button is down*/
			(buffer->Mask) |= 2;	/*   then set appropriate bit*/
		if (registers[BX] & 2)		/* if the right button down*/
			(buffer->Mask) |= 8;	/*   then set appropriate bit*/
		else if (!(registers[BX] & 1))	/* if no buttons, but motion */
			buffer->Mask |= 1;	/*   then set apporpriate bit*/
	}
	if (registers[BX] & 1)		/* if the left button is down*/
		(buffer->Mask) |= 4;	/*   then set appropriate bit*/
	if (registers[BX] & 2)		/* if the right button down*/
		(buffer->Mask) |= 16;	/*   then set appropriate bit*/
	return (0);				/* return no error */
}


/*** MOUSETDEVSTATUS - Dummy function, included for maximum compatibility
 */
unsigned far pascal
MOUSETDEVSTATUS (status, handle)
unsigned far *status;
unsigned handle;
{
}


/*** MOUGETNUMQUEEL - Always returns 1 in .Events
*/
unsigned far pascal
MOUGETNUMQUEEL (info, handle)
struct QueInfo far *info;
unsigned handle;
{
	info->Events = 1;
}


/*** MOUSETPTRPOS - Sets mouse pointer position
*/
unsigned far pascal
MOUSETPTRPOS (loc, handle)
struct PtrLoc far *loc;
unsigned handle;
{
	int registers[4];			/* for int33h() */

	registers[AX]=4;			/* int 33 set position funct */
	registers[CX]=loc->ColPos;		/* new coordinates */
	registers[DX]=loc->RowPos;
	int33h (registers);			/* do it */
}
