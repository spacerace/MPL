/*
 * This is a common include file used by examples share.c and shrchild.c.
 * It contains the declaration of the shared memory segment.
 */

/* Convert an unsigned selector and an unsigned offset into a far pointer */


/* size of shared memory segment */
#define	SHRSEGSIZE	sizeof(struct ShareRec)	

#define SHRSEGNAME	"\\SHAREMEM\\SHARESEG.DAT"	  /* shared seg name */
#define	CIRCBUFSIZE	20		/* size of circular buffer */
#define WAITFOREVER	-1L		/* no timeout if semaphore is owned */

struct ShareRec {
	ULONG		fullsem;	/* buffer access semaphore */
	ULONG		emptysem;	/* buffer access semaphore */
	ULONG		mutexsem;	/* buffer access semaphore */
	int		head;		/* buffer write pointer */
	int		tail;		/* buffer read pointer */
	char 		CircBuffer[CIRCBUFSIZE];	/* circular buffer */
};

/* definition of buffer full condition */
#define BUFFUL(p) ((p->head==p->tail-1)||((p->tail==0)&&(p->head==CIRCBUFSIZE-1)))
