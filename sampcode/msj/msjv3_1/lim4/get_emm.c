/*

Figure 4
========

*/

/******************************************************************/
/* This function gets the amount of exapnded memory needed and    */
/* returns a pointer to the page from in emm_ptr_ptr.  It also    */
/* assigns a name to the handle. It will return a result of 
/* non-zero if it fails.                                          */
/******************************************************************/

int get_expanded_memory(emm_ptr_ptr, pages, emm_handle_ptr, name)

char *(*emm_ptr_ptr);    /* pointer to expanded memory page frame */
int pages;               /* number of pages to allocate */
unsigned int *emm_handle_ptr;  /* pointer to emm handle */
char *name;

{
   /* check for number of expanded memory pages requested */

   inregs.h.ah = GET_UNALLOCATED_PAGE_COUNT ;
   int86(EMM_INT, &inregs, &outregs);
   if (outregs.h.ah != 0) return(1);
   if (outregs.x.bx < pages) return(2);

   /* allocate pages pages - Function */

   inregs.h.ah = ALLOCATE_PAGES ;   /* Get a handle and allocate */
   inregs.x.bx = pages;             /* 4 logical pages           */ 
   int86(EMM_INT, &inregs, &outregs);
   if (outregs.h.ah != 0) return(3);
   *emm_handle_ptr = outregs.x.dx ;

   /* get page frame segment address - Function 2 */

   inregs.h.ah = GET_FRAME_ADDRESS;
   int86(EMM_INT, &inregs, &outregs);
   if (outregs.h.ah != 0) return(4);
   *emm_ptr_ptr = (unsigned long int) (outregs.x.bx *65536);
   

   /* assign name to handle  - Function 20          */

   inregs.x.ax = SET_HANDLE_NAME ;
   inregs.x.dx = *emm_handle_ptr ;
   inregs.x.si = FP_OFF(name) ;
   segregs.ds  = FP_SEG(name);
   int86x(EMM_INT, &inregs, &outregs, &segregs);
   if (outregs.h.ah != 0) return(5);

   return(0);

}