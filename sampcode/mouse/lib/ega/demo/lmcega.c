/*----------------------------------------------------------------------*/
/* REFERENCE -- Test of cegal()				IN lmcega.c	*/
/*									*/
/* This program tests the function cegal(), which is an interface	*/
/* between large model MSC 3.0 (or higher) 'C' programs, and the EGA	*/
/* Register Interface.							*/
/*									*/
/* To create lmcega.exe:						*/
/*				MSC  /AL /Ze /w0 LMCEGA;		*/
/*				LINK LMCEGA,,,..\EGA;			*/
/*	               or:						*/
/*				MAKE LMCEGA.MAK				*/
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <dos.h>

#define ega(a,b,c,d,e)	cegal(a,b,c,d,e)
#define FALSE	0
#define NOT	!
#define TRUE	NOT FALSE


main()
{
    int ah, bx, cx, dx, es;
    char buf[2];
    char far *ver_fptr;
    
    ah = 0xfa;
    bx = 0;
    
    ega(&ah, &bx, &cx, &dx, &es);
	
    if (!bx)
    {
	printf("\nEGA Driver not found");
	return;
    }
    printf("\nEGA Driver found, version ");
    ver_fptr = (((long) es) << 16) + bx;
    printf("%1d.%2d", *ver_fptr, ver_fptr[1]);
    
    printf("\nRead/Write Range functions (F2, F3) ");
    if (test_range())	printf("PASSED");
    else		printf("FAILED");
    printf("\n\n\n");
}


test_range()
{
    int ah, bx, cx, dx, es;
    int *bxp;
    static char org_buf[2] = {0xAA, 0xAA};
    static char wr_buf[2] = {0xBB, 0xBB};
    static char rd_buf[2] = {0xCC, 0xCC};
    int old_map_mask;
    int old_misc_reg;

    /* read the cursor location high and low registers */
    
    ah  = 0xF2;		/* read range	*/
    cx  = 0x0e00;	/* start at reg 14	*/
    cx |= 0x0002;	/* 2 reg to read	*/
    dx  = 0x0000;	/* crt controller	*/
    es  = getds();	/* current data segment */
    
    ega(&ah, org_buf, &cx, &dx, &es);
    
    wr_buf[0] = 0xee;
    wr_buf[1] = 0xff;

    ah = 0xF3;		/* write range		*/
    cx = 0x0e02;	/* start at e, write 2	*/
    dx = 0;		/* crt controller	*/
    es = getds();
    
    ega(&ah, wr_buf, &cx, &dx, &es);
    
    ah = 0xF2;		/* read range	*/
    cx = 0x0e02;	/* start at e , read 2	*/
    dx = 0;
    es = getds();
    
    ega(&ah, rd_buf, &cx, &dx, &es);

    /* restore original values */
        
    ah = 0xF3;		/* write range		*/
    cx = 0x0e02;	/* start at e, write 2	*/
    dx = 0;		/* crt controller	*/
    es = getds();
    ega(&ah, org_buf, &cx, &dx, &es);
    
    if (rd_buf[0] != wr_buf[0])
	return FALSE;
    if (rd_buf[1] != wr_buf[1])
	return FALSE;
    
    return TRUE;
}


getds()
/*----------------------------------------------------------------------*/
/* METHOD  -- Returns the current data segment.				*/
/*----------------------------------------------------------------------*/
/* RETURNS -- Current code segment.					*/
/*----------------------------------------------------------------------*/
/* GLOBAL EFFECTS -- none.						*/
/*----------------------------------------------------------------------*/
/* REVISION  1.00 is:  Original version.				*/
/*----------------------------------------------------------------------*/
{
    struct SREGS segregs;
    
    segread(&segregs);
    return segregs.ds;
}
