#include <graph.h>
#include <stdio.h>
#include <memory.h>
#include <dos.h>
#include <malloc.h>

/*       error =  get_screen(filename);               */
/*                                                    */
/*       Routine for retrieving EGA screens           */
/*       Used for 16 color 350x640 resolution         */
/*       No packing or unpacking of image             */
/*       Created for MSC V5.0  small model            */
/*                                                    */
/*       error of type int, returned as below         */
/*       filename - array of type char, length 13     */
/*       Routine uses a 28K buffer for fast retrieval */
/*       if data segment space is at a premium,       */
/*       could rewrite to use smaller buffers.        */
/*                                                    */
/*       If using a model with a large data space     */
/*       then all data calls are far and no buffer is */
/*       needed.  Use fread to write directly to      */
/*       video memory.                                */

 














/* DEFINE A BUFFER TO HOLD ONE OF THE FOUR PLANES */

unsigned char buffer[28000];





get_screen (filename)

char *filename;       /* filename passed, 12 character max */

{
        int flag = 0; /* return integer flag     */
                      /* flag = 0  success       */
                      /* flag = 1  error opening */
                      /* flag = 2  error reading */


        struct SREGS segregs;    /* structure for segread call */

        FILE *screen_file;       /* image file */





        segread(&segregs);       /* get data segment for movedata below */

        _setvideomode(_ERESCOLOR);  /* set EGA 640x350 16 color mode */
                                    /* 128 K required */

        if (!(_setvideomode(_ERESCOLOR)))
            {
              flag = 3;
              return(flag);
            }

        if ((screen_file = fopen(filename,"rb")) != NULL)

              {

                 outp(0x3C4,2);        /* set color plane for read */
                 outp(0x3C5,0x01);
                 fread(buffer,1,28000,screen_file); /* read 1 plane */
                 movedata(segregs.ds,(unsigned int)buffer,0xA000,0x0000,28000);

                 /* segregs.ds is data segment for variable "buffer"  */
                 /* buffer is the offset in that segment for "buffer" */
                 /* A0000000 is the far pointer to EGA video memory   */
                 /*          0xA000 = seg      0x0000 = offset        */
                 /* 28000 is the size of one 640x350 color plane      */


                                     /* repeat for the other three planes */
                 outp(0x3C4,2);
                 outp(0x3C5,0x02);
                 fread(buffer,1,28000,screen_file);
                 movedata(segregs.ds,(unsigned int)buffer,0xA000,0x0000,28000);



                 outp(0x3C4,2);
                 outp(0x3C5,0x04);
                 fread(buffer,1,28000,screen_file);
                 movedata(segregs.ds,(unsigned int)buffer,0xA000,0x0000,28000);





                 outp(0x3C4,2);
                 outp(0x3C5,0x08);
                 fread(buffer,1,28000,screen_file);
                 movedata(segregs.ds,(unsigned int)buffer,0xA000,0x0000,28000);

           if (ferror(screen_file))      /* check for read error */
                 flag = 2;



        fclose(screen_file);

               }


        else
             flag = 1;    /* set flag if failed to open file */

       return(flag);

       }
