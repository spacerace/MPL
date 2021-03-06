/* memory.c
*/

/*	determine system & available memory sizes
*/

#include	<_default.c>		/* diddle to make smaller */
#include	<stdio.h>	      /* this is CI-C86 header file */

struct	regval {
	    unsigned ax,bx,cx,dx,si,di,ds,es;
	    };

extern	unsigned int *_PSPSEG;		/* pointer to DOS program segment prefix
					   everything below this addres is verboten */

extern	long	     ptrtoabs();	/* convert pointer to absolute address */
extern	unsigned int sysint();		/* invokes any INT */

main()
{

    struct   regval sysr;		    /* register arguments for INT */
    unsigned int    intstat;		    /* flags returned from INT */
    unsigned long   hdw_memsize,	    /* total system memory, bytes */
		    ret_memsize;	    /* total available to program */

    intstat = sysint( 0x12, &sysr, &sysr ); /* invoke BIOS INT 12h */
    hdw_memsize = sysr.ax * 1024L;	    /* total system memory */

    ret_memsize = hdw_memsize - ptrtoabs(_PSPSEG);/* total available memory */

    printf( "%12lu bytes - system\n", hdw_memsize );
    printf( "%12lu bytes - available\n", ret_memsize );

    exit( 0 );				/* exit to whomever, successfully */

    }	/* end main() */

					/* end memory.c */
                                                      