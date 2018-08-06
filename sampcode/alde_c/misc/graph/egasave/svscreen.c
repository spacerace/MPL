#include <graph.h>
#include <stdio.h>
#include <memory.h>
#include <dos.h>
#include <malloc.h>


/*        Program for saving the EGA screen FAST       */
/*        This version for MSC 5.0 C, small model      */
/*        Uses 28K buffer in data segment, you may     */
/*        need change the program to use a smaller     */ 
/*        buffer if data space is at a premium.  This  */
/*        version works well for a fast "slide show"   */
/*        type of effect where there is not much       */
/*        program data.                                */
/*                                                     */
/*        If using a model with a large data segment   */
/*        so that all data calls are far, then the     */
/*        buffer is not needed, you can fwrite         */
/*        directly from video memory.                  */
/*                                                     */
/*        usage:    error = save_screen(filename);     */
/*                  error is type int                  */
/*                  filename is char array of size 13  */
/*                  error returned as stated below     */










unsigned char buffer[28000];   /* buffer for one plane of video mem */




save_screen (filename)

char *filename;   /*  file name passed from calling program */

{

      unsigned char plane;    /* variable to hold current video plane */
      int flag;          /* Error Flag */
                         /* 0 = success */
                         /* 1 = open file error */
                         /* 2 = write file error */

      FILE *screen_save;
      struct SREGS segregs;    /* structure for segread call */


      segread(&segregs);

      if ((screen_save = fopen(filename, "wb")) == NULL)
            return(1);


      for (plane = 0; plane < 4; plane++)
         {
             outp(0x3CE, 4);
             outp(0x3CF, plane);

             movedata(0xA000, 0x0000, segregs.ds, (unsigned int)buffer, 28000);

               /* 0xA000 = segment for EGA video memory */
               /* 0x0000 = offset  for EGA video memory */
               /* segregs.ds = segment for variable "buffer" */
               /* (unsigned int)buffer = offset for variable "buffer" */
               /* 28000 is the byte size of one video plane */

             fwrite(buffer,1,28000,screen_save);
             if (ferror(screen_save))
                  return(2);

          }


    fclose(screen_save);


}




