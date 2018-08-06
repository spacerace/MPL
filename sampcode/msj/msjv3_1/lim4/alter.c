/*

Figure 9
========

*/

/******************************************************************/
/* This function implements ALTER PAGE MAP AND CALL  emm function */
/******************************************************************/

int map_and_call(new_map_ptr, new_length, old_map_ptr, 
                 old_length, handle)
struct log_phys *new_map_ptr;
char new_length;
struct log_phys *old_map_ptr;
char old_length;
unsigned int handle;
{
  struct map_call_struct {
     unsigned int offset_target_address ;
     unsigned int seg_target_address ;
     char new_page_map_length ;
     unsigned int offset_new_page_map;
     unsigned int seg_new_page_map ;
     char old_page_map_length ;
     unsigned int offset_old_page_map;
     unsigned int seg_old_page_map ;
   } map_call; 
   struct map_call_struct *map_call_ptr;  

  map_call_ptr = &map_call ;
  map_call.offset_target_address = 0 ;
  map_call.seg_target_address    =0xd000;
  map_call.new_page_map_length   = new_length;
  map_call.offset_new_page_map   = FP_OFF(new_map_ptr);
  map_call.seg_new_page_map      = FP_SEG(new_map_ptr);
  map_call.old_page_map_length   = old_length;
  map_call.offset_old_page_map   = FP_OFF(old_map_ptr);
  map_call.seg_old_page_map      = FP_SEG(old_map_ptr);

  /****************************************************************/
  /* Set up for alter page map and call function                  */
  /****************************************************************/

  inregs.h.ah =0x56;
  inregs.h.al = 0;
  inregs.x.dx = handle ;
  map_call_ptr = &map_call ;
  inregs.x.si = FP_OFF(map_call_ptr);
  segregs.ds = FP_SEG(map_call_ptr);
  int86x(EMM_INT,&inregs,&outregs,&segregs);
  if (outregs.h.ah != 0) return(1);

  return(0) ;
}