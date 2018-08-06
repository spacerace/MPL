/*

Figure 5
========

*/

/******************************************************************/
/* This function implements MAP/UNMAP MULTIPLE PAGES emm function */
/******************************************************************/

int map_unmap_multiple_pages (log_phys_pages,handle,map_unmap)

struct log_phys *log_phys_pages ;  /* pointer to log_phys struct */
unsigned int handle;               /* handle to map or unmap */
unsigned int map_unmap;            /* 0 = map, 1 = unmap */
{
 int i ;
 struct log_phys *temp_ptr;
 
 /* Map mulitple logical pages 0 -3 to logical pages 0 - 3 */

 temp_ptr = log_phys_pages;
   
 for (i=0 ; i<=3; i++) 
 {
   log_phys_pages->phys_page_number = i;
   if (map_unmap == 1)
    log_phys_pages->log_page_number = i;
   else
    log_phys_pages->log_page_number = 0xFFFF ; 

   log_phys_pages++ ;
       
 }

 inregs.x.ax = MAP_UNMAP_MULTIPLE_PAGES ;
 inregs.x.dx = handle;
 inregs.x.cx = 4;
 inregs.x.si = FP_OFF(temp_ptr);
 segregs.ds  = FP_SEG(temp_ptr);
 int86x(EMM_INT,&inregs,&outregs,&segregs);
 if (outregs.h.ah != 0) return(1);
 return(0);
} 
