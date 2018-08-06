/*
Microsoft Systems Journal
Volume 2; Issue 1; March, 1987
Code Listings For:

	Expanded Memory
	pp. 21-32

Author(s): Marion Hansen, Bill Krueger, Nick Stuecklen
Title:     Expanded Memeory: Writing Programs That Break the 640K Barrier




==============================================================================
==============================================================================


Figure 3: Main Program
==============================================================================
*/

#include <dos.h>
#include <stdio.h>

#define EMM_INT 0x67                /*  EMM interrupt number  */
#define GET_PAGE_FRAME_BASE 0x41    /*  EMM func = get page frame 
                                        base address  */
#define GET_FREE_COUNT 0x42         /*  EMM Func = get unallocated 
                                        pages count  */
#define ALLOCATE_PAGES 0x43         /*  EMM Func = allocates pages  */
#define MAP_PAGES 0x44              /*  EMM Func = map pages  */
#define DEALLOCATE_PAGES 0x45       /*  EMM Func = deallocate pages  */
#define GET_INT_VECTOR 0x35         /*  DOS func = get interrupt 
                                        vector  */
#define DEVICE_NAME_LEN 8           /*  Number of chars in device 
                                        driver name field  */
#define VIDEO_RAM_SIZE 4000         /*  Total bytes in video RAM 
                                        (char/attr)  */
#define VIDEO_RAM_BASE 0xB0000000   /*  Video RAM start address (MDA) */

union REGS input_regs, output_regs; /*  Regs used for calls to EMM 
                                        and DOS  */
struct SREGS segment_regs;
unsigned int emm_status;            /*  Status returned by EMM  */

main ()

{
unsigned int i;
long target_time, current_time;
char *video_ram_ptr = {VIDEO_RAM_BASE}; /*  Pointer to video RAM  */
unsigned int emm_handle;                /*  EMM handle  */
char *expanded_memory_ptr;              /*  Pointer to expanded 
                                            memory  */

/*  Ensure that the Expanded Memory Manager software is installed 
    on the user's system.  */

   detect_emm();

/*  Get a page of expanded memory.  */
 
   get_expanded_memory_page (&expanded_memory_ptr, &emm_handle);

/*  Copy the current video RAM contents to expanded memory.  */

   memcpy (expanded_memory_ptr, video_ram_ptr, VIDEO_RAM_SIZE);

/*  Clear the screen to nulls.  */

   memset (video_ram_ptr, '\0', VIDEO_RAM_SIZE);

/*  Delay for 1 second so the user can see the blanked screen.  */

   time (&current_time);
   target_time = current_time + 1;
   while (current_time < target_time)
    {
     time (&current_time);
    }

/*  Restore the video RAM contents from expanded memory.  */

   memcpy (video_ram_ptr, expanded_memory_ptr, VIDEO_RAM_SIZE);

/*  Deallocate the expanded memory page  */

   release_expanded_memory_page (emm_handle);

   exit(0);
}

/*
==============================================================================
==============================================================================

Figure 4: Detect_EMM Subroutine
==============================================================================
*/

detect_emm ()

{
static char EMM_device_name [DEVICE_NAME_LEN] = {"EMMXXXX0"};
char *int_67_device_name_ptr;


/*  Determine the address of the routine associated with INT 67 hex.  */

  input_regs.h.ah = GET_INT_VECTOR;  /*  DOS function  */
  input_regs.h.al = EMM_INT;         /*  EMM interrupt number  */
  intdosx (&input_regs, &output_regs, &segment_regs);
  int_67_device_name_ptr = 
  (segment_regs.es * 65536) + 10;    /*  Create ptr to device name 
                                         field  */

/*  Compare the device name with the known EMM device name.  */

  if(memcmp(EMM_device_name,int_67_device_name_ptr,DEVICE_NAME_LEN) !=0)
   {
     printf ("\x07Abort: EMM device driver not installed\n");
     exit(0);
   }  
} 

/*
==============================================================================
==============================================================================

Figure 5: Check_Status Subprocedure
==============================================================================
*/

check_status (emm_status)
unsigned int emm_status;
{
static char *emm_error_strings[] = {
  "no error",
  "EMM software malfunction",
  "EMM hardware malfunction",
  "RESERVED",
  "Invalid EMM handle",
  "Invalid EMM function code",
  "All EMM handles being used",
  "Save/restore page mapping context error",
  "Not enough expanded memory pages",
  "Not enough unallocated pages",
  "Can not allocate zero pages",
  "Logical page out of range",
  "Physical page out of range",
  "Page mapping hardware state save area full",
  "Page mapping hardware state save area already has handle",
  "No handle associated with the page mapping hardware state save area",
  "Invalid subfunction"
 };

/*  IF EMM error, THEN print error message and EXIT  */

   if (emm_status != 0)                    /*  IF EMM error...  */
    {
      emm_status -= 0x7F;                  /*  Make error code 
                                               zero-based  */
      printf ("\x07Abort: EMM error = ");  /*  Issue error prefix  */
      printf ("%s\n", emm_error_strings[emm_status]);
                                           /*  Issue actual error 
                                               message  */
      exit(0);                             /*  And then exit to 
                                               DOS  */
    }
}

/*
==============================================================================
==============================================================================
Figure 6: Get_Expanded_Memory_Page Subprocedure

==============================================================================
*/

get_expanded_memory_page (expanded_memory_ptr_ptr, emm_handle_ptr)

unsigned int *emm_handle_ptr;     /*  16 bit handle returned by EMM  */
char *(*expanded_memory_ptr_ptr); /*  Pointer to expanded memory 
                                      page  */

{
unsigned int page_frame_base;     /*  Expanded memory page frame 
                                      base  */
unsigned int physical_page = {0}; /*  Physical page number  */


/*  Get unallocated pages count.  */

input_regs.h.ah = GET_FREE_COUNT;    /*  EMM function  */
int86x (EMM_INT, &input_regs, &output_regs, &segment_regs);
emm_status = output_regs.h.ah;
check_status(emm_status);            /*  Check for errors  */
if (output_regs.x.bx < 1)            /*  Check unallocated page 
                                         count  */
 {
  printf ("\x07Abort: insufficient unallocated expanded memory pages\n");
  exit(0);
 }


/*  Allocate the specified number of pages.  */
 
input_regs.h.ah = ALLOCATE_PAGES;     /*  EMM function  */
input_regs.x.bx = 1;                  /*  Number of pages to 
                                          allocate  */
int86x (EMM_INT, &input_regs, &output_regs, &segment_regs);
emm_status = output_regs.h.ah;
check_status(emm_status);             /*  Check for errors  */
*emm_handle_ptr = output_regs.x.dx;   /*  Get EMM handle  */


/*  Map the logical page into physical page 0.  */
 
   input_regs.h.ah = MAP_PAGES;          /*  EMM function  */
   input_regs.h.al = 0;                  /*  Logical page number  */
   input_regs.x.bx = physical_page;      /*  Physical page number  */
   input_regs.x.dx = *emm_handle_ptr;    /*  EMM handle  */
   int86x (EMM_INT, &input_regs, &output_regs, &segment_regs);
   emm_status = output_regs.h.ah;
   check_status(emm_status);             /*  Check for errors  */


/*  Determine the page frame address.   */

   input_regs.h.ah = GET_PAGE_FRAME_BASE; /*  EMM function  */
   int86x (EMM_INT, &input_regs, &output_regs, &segment_regs);
   emm_status = output_regs.h.ah;
   check_status(emm_status);              /*  Check for errors  */
   *expanded_memory_ptr_ptr = 
     (output_regs.x.bx * 65536)
     + (physical_page * 16 * 1024);       /*  Set the expanded memory 
                                              ptr  */
}

/*
==============================================================================
==============================================================================
Figure 7: Release_Expanded_Memory_Page Subroutine

==============================================================================
*/

release_expanded_memory_page (emm_handle)

unsigned int emm_handle;      /*  Handle identifying which page 
                                  set to deallocate  */
{

/*  Release the expanded memory pages by deallocating the handle 
    associated with those pages.  */
  
   input_regs.h.ah = DEALLOCATE_PAGES;  /*  EMM function  */
   input_regs.x.dx = emm_handle;        /*  EMM handle passed in 
                                            DX  */
   int86x (EMM_INT, &input_regs, &output_regs, &segment_regs);
   emm_status = output_regs.h.ah;
   check_status(emm_status);            /*  Check for errors  */
}
