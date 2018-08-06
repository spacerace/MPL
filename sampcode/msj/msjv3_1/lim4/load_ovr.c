/*

Figure 10
========


/******************************************************************/
/* This function implements the DOS load function 0x4b. It sets   */
/* AL to 3 which cause the function to load the exe and apply a   */
/* relocation factor to it, but it does not execute the file.     */
/******************************************************************/

int load_overlay(load_file_name,relocation_ptr,page)
char *load_file_name ;
char *(*relocation_ptr);
unsigned int page; /* physical page at which to load */
  
{
struct reloc {
  unsigned int load_seg;
  unsigned int reloc_factor;
  } reloc_struct;
  struct reloc *reloc_struct_ptr;

  /* setup up structure to pass to load call */

  reloc_struct.load_seg = FP_SEG(*relocation_ptr) + (page * 0x400);
  reloc_struct.reloc_factor = FP_SEG(*relocation_ptr) + 
                               (page * 0x400); 
  
  /* Set up register structures for Intdos call */

  inregs.h.ah = 0x4B ;   /* Dos Exec function code */
  inregs.h.al = 3;       /* load but do not execute */
  inregs.x.dx  = FP_OFF(load_file_name);
  segregs.ds   = FP_SEG(load_file_name);

  reloc_struct_ptr = &reloc_struct ;
  inregs.x.bx  = FP_OFF(reloc_struct_ptr);
  segregs.es  = FP_SEG(reloc_struct_ptr);

  intdosx(&inregs, &outregs, &segregs);

}
