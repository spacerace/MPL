/********************************************************************
*	CHATMEM.C  - Memory allocation routines for shared DLL memory
*
*	(C) 1988, By Ross M. Greenberg for Microsoft Systems Journal
*
*	This module conatins three functions. Allocation of memory,
*	de-allocation of memory and the getseg call.
*
*	The current ANSI calloc/alloc/malloc sequence does not allow for
*	an additional parameter to specify if memory requested through
*	these functions is to be sharable or private. Therefore the MSC
*	library calls all allocate private memory.
*
*	These routines allocate a 64K chunk of memory, requested from OS/2
*	as a sharable chunk, then dole it out using the DosSub allocation
*	calls.
*
*	Only one 64K chunk is allocated: if more memory is desired an
*	additional call to dosallocseg would have to be made and all 
*	sessions already logged in given access to the chunk.  Out of 
*	laziness, that was not done for these demonsttration routines.
*
*
*	Compile with:
*
*	cl /AL /Au /Gs /c chatmem.c
*
*	Note -	Though broken here, the following two lines are entered
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

/* The following OS/2 system calls are made in this module: */

extern far pascal dosallocseg();
extern far pascal dosfreeseg();
extern far pascal dossuballoc();
extern far pascal dossubset();
extern far pascal dossubfree();
extern far pascal dosgetseg();
extern far pascal dossemrequest();
extern far pascal dossemclear();


#define	NULLP	(char *)NULL


/*	This semaphore is so that we don't hurt ourselves as we allocate
 *	and deallocate memory
 */

long	memory_semaphore = NULL;

/*	This is the actual selector which the 64K dosallocseg() 
 * call returns
 */

unsigned major_selector = NULL;

/********************************************************************
*	my_calloc(number_of_items,size_of_item)
*
*	Emulates the more typical calloc call, but returns memory which
*	can later be allocated as sharable.
*
*	After the first call (which causes a 64K chunk to be allocated),
*	all subsequent calls cause a dossuballoc call to be made, and
*	a long pointer to the returned memory to be created and returned
*
*	The 64K chunk must be initialized by the dossubset call before it
*	can be used by other dossub functions.
*
*	Because the dossubfree call requires a size, the size requested
*	plus the sizeof an int is actually allocated and the size of the
*	total request is then stored in the first two bytes of the 
*	returned character array.  The ptr returned, however, is this 
*	memory location plus the initial sizeof and int -- therefore the 
*	bookkeeping is transparent to the application task.
********************************************************************/

char * 
my_calloc(size1, size2)
int	size1;
int	size2;
{
unsigned	long	selector;
int	stat;
char	*ptr;
int	sizeit = (size1 * size2) + sizeof(int);

	dossemrequest(&memory_semaphore, -1L);
	if	(!major_selector)
	{
		if	(stat = dosallocseg(0, &major_selector, 3))
		{
			printf("dosalloc error:%d\n", stat);
			dossemclear(&memory_semaphore);
			return(NULLP);
		}

		if	(stat = dossubset(major_selector, 1, 0))
		{
			printf("Error in dossubset:%d\n", stat);
			dossemclear(&memory_semaphore);
			return(NULLP);
		}
	}

	selector = 0;
	if	(stat = dossuballoc(major_selector, &selector, sizeit))
	{
		printf("dossuballoc error:%d\n", stat);
		dossemclear(&memory_semaphore);
		return(NULLP);
	}

	dossemclear(&memory_semaphore);
	ptr = (char *)(((long)major_selector << 16) + (long)selector);
	memset(ptr, (char)NULL, sizeit);
	*(int *)ptr = sizeit;
	return(ptr + sizeof(int));
}

/********************************************************************
*	my_free(pointer_to_a_character_array_previously_my_calloc'ed)
*
*	Subtract sizeof an int from the pointer, dereference as an
*	int, then free that number of bytes.
*
********************************************************************/

my_free(ptr)
char	*ptr;
{
int	stat;

	ptr -= sizeof(int);

	dossemrequest(&memory_semaphore, -1L);
	if (stat = dossubfree(major_selector, FP_OFF(ptr), *(int *)ptr))
	{
		printf("Error freeing: %lx\n", ptr);
		exit(1);
	}
	dossemclear(&memory_semaphore);
}

/********************************************************************
*	my_getseg()
*
*	Causes the memory affilaited with the major_selector to become
*	accessable to this process.
********************************************************************/

my_getseg()
{
int	stat;

	if	(stat=dosgetseg(major_selector))
		printf("Error on getseg:%d\n", stat);
	return(stat);
}
