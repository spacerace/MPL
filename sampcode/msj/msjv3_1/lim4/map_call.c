/*

Figure 8
========


/* THIS IS THE  MAIN PROGRAM FOR MAPCALL */

#include <c:\msc\include\dos.h>
#include <c:\msc\include\memory.h>
#include <c:\msc\include\stdio.h>
#include <c:\emm\demo\emm.h>

/* set up size and base of video ram */

union REGS    inregs,    outregs;
struct SREGS     segregs ;

unsigned int emm_handle ;
char emm_device_name[] = "EMMXXXX0";      /* Device Name of EMM */
char far emm_handle_name[8] ="mapcall";   /* name for handle */
char far mod1_name[] =
     "c:\\emm\\demo\\module1.exe\0";      /* name of modules to be */                                                      
char far mod2_name[] =                    /* to be loaded */
     "c:\\emm\\demo\\module2.exe\0";
char far *emm_ptr ;
char far *(*page_ptr) ;
int pages_needed = 16 ;

struct log_phys {
  int log_page_number;
  int phys_page_number;
       } ;

struct log_phys far current_pages[4];

struct log_phys far map_call_pages ;

int  result;      

main ()
{


   /***************************************************************/
   /* Check for a expanded memory manager of 4.0 or greater.      */
   /***************************************************************/

   check_emm_version_number();
   
   /***************************************************************/
   /* Cet four pages of expanded memory.                          */
   /***************************************************************/

   result = get_expanded_meory(&emm_ptr, pages_needed, &emm_handle,
                               emm_handle_name);
   if (result != 0) exit(1);

   /***************************************************************/
   /* Map in needed pages                                         */
   /***************************************************************/

   result = map_unmap_multiple_pages (current_pages, emm_handle, 1);
   
   /***************************************************************/
   /* Load map and calls executeables into expanded memory        */
   /***************************************************************/

   *page_ptr = emm_ptr;   /* Module 1 will be loaded into logical */
   load_overlay(mod1_name, page_ptr, 0);   /* and physical page 0 */

   /***************************************************************/
   /* Load module 2 into logical page 1 and physical page 1.      */
   /***************************************************************/

   load_overlay(mod2_name, page_ptr, 1);
   
   /***************************************************************/
   /* Unmap all pages. The map and call will map in the pages     */
   /* requested and restore the present mapping.                  */
   /***************************************************************/

   result = map_unmap_multiple_pages (current_pages, emm_handle, 0);
   
   /***************************************************************/
   /* Do map and call to module in logical page 1. Map logical    */
   /* page to physical page 0 and have current mapping context    */
   /* restored on the return from the map and call.               */
   /***************************************************************/

    map_call_pages.log_page_number  = 0;
    map_call_pages.phys_page_number = 0;
    
    map_and_call(&map_call_pages, 1, &current_pages, 0, emm_handle);
   
   /***************************************************************/
   /* release handle befor exiting.  This also ummaps any pages   */
   /* that are mapped.                                            */
   /***************************************************************/

   inregs.h.ah = DEALLOCATE_PAGES ;
   inregs.x.dx = emm_handle ;
   int86(EMM_INT, &inregs, &outregs);
   
   exit(0);
}
