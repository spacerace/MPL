/*

Figure 6
========

*/

/******************************************************************/
/* This function implements MOVE or EXCHANGE MEMORY REGION        */
/* function to move or exchange conventional memory with expanded */
/* memory pages                                                   */
/******************************************************************/

int move_exchg_to_expanded(function_number, conv_buffer,  handle, 
                           page, length)
unsigned int function_number ; /* Move or Exchange*/
char far *conv_buffer ;        /* conventional memory with */
int handle;                    /* EMM memory associated with 
                                  this handle */
int page ;                     /* at this physical page */
unsigned long int length;      /* and this many bytes */

{
struct move_exchg 
 {
   unsigned long int region_length;
   char source_type ;
   unsigned int source_handle ;
   unsigned int source_offset ;
   unsigned int source_seg_page;
   char dest_type;
   unsigned int dest_handle;
   unsigned int dest_offset;
   unsigned int dest_seg_page;
  } move_exchg_struct;
struct move_exchg *move_exchg_ptr;  

  /* START OF FUNCTION CODE */

  move_exchg_struct.region_length  = length ;
  move_exchg_struct.source_type    = 0;
  move_exchg_struct.source_handle  = 0;
  move_exchg_struct.source_offset   = FP_OFF(conv_buffer);
  move_exchg_struct.source_seg_page = FP_SEG(conv_buffer);
  move_exchg_struct.dest_type      = 1;
  move_exchg_struct.dest_handle    = handle;
  move_exchg_struct.dest_offset    = 0 ;
  move_exchg_struct.dest_seg_page  = page;

  /* Setup structure to make int86x call */

  inregs.x.ax = function_number;
  move_exchg_ptr = &move_exchg_struct;
  inregs.x.si = FP_OFF(move_exchg_ptr);
  segregs.ds  = FP_SEG(move_exchg_ptr);
  int86x(EMM_INT, &inregs, &outregs, &segregs);
  if (outregs.h.ah != 0) exit(1);

  return(outregs.x.ax) ;
         
}