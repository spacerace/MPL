/*

Figure 7
========


/* THIS IS THE MAIN PROGRAM FOR SWPSCR */

#include <c:\msc\include\dos.h>
#include <c:\msc\include\memory.h>
#include <c:\msc\include\stdio.h>
#include <c:\emm\demo\emm.h>

/* set up size and base of video ram */

#define VIDEO_RAM_SIZE 4000
#define VIDEO_RAM_BASE 0XB8000000
union REGS    inregs,    outregs;
struct SREGS     segregs ;

char far *video_ram_ptr =  {VIDEO_RAM_BASE}; /* video  start address
                                                (CGA) */
unsigned long int video_ram_size ={4000};    /* bytes in video ram */
unsigned int emm_handle ;                    /* emm handle */
char emm_device_name[] = "EMMXXXX0";         /* Device Nmae of EMM */
char emm_handle_name[8] ="shared" ;          /* name for handle to be 
                                                shared */
char far *emm_ptr;                           /* pointer to page 
                                                frame */
char far *(*page_ptr) ;                      /* pointer to page in 
                                                the frame */
int pages_needed = 4;
struct log_phys {
  int log_page_number;
  int phys_page_number;
       } current_pages [4] ;
struct log_phys far *map_unmap_ptr ;
int  result ;  /* result passed back from function calls */

main ()
{
   /***************************************************************/
   /* Check for a expanded memory manager of 4.0 or greater.      */
   /***************************************************************/

   check_emm_version_number();
   
   /***************************************************************/
   /* Cet four pages of expanded memory.                          */
   /***************************************************************/

   result = get_expanded_memory(&emm_ptr, pages_needed, &emm_handle, 
                                emm_handle_name);
   if (result != 0) exit(1);
   
   /***************************************************************/
   /* Map in needed pages                                         */
   /***************************************************************/

   result = map_unmap_multiple_pages (current_pages, emm_handle, 1);

   /***************************************************************/
   /* Copy video screen to logical page 0.                        */
   /***************************************************************/

   move_exchg_to_expanded(MOVE_MEMORY_REGION, /* Move to convent- */
                          video_ram_ptr,      /* ional ram area   */
                          emm_handle,         /* handle number    */
                          0,                  /* to physical page */
                          video_ram_size);    /* the number bytes */
   
   

   /* make a null video screen at logical page 1 */

   page_ptr = (emm_ptr + 0x4000);
   memset (page_ptr, 0, VIDEO_RAM_SIZE);

   /***************************************************************/
   /* Unmap all of the pages so that they are proteced.           */
   /***************************************************************/

   result = map_unmap_multiple_pages (current_pages, emm_handle, 0);

   /* exit to Dos */
   
}