/* EQLIST.C - From page 366 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. This program lists the number     */
/* of Kbytes of memory in computer and the type of equipment    */
/* on the computer system.                                      */
/****************************************************************/

#define EQLIST 0x410       /*Location of equipment list word*/
#define MEMSIZ 0x413       /*Location of memory size word*/

main()
{
int far *farptr;
unsigned int eq, data;

   farptr = (int far *) EQLIST;
   eq = *(farptr);
   data = eq >> 14;              /*printers*/
   printf("\nNumber of printers is %d.\n", data);
   if(eq & 0x2000)               /*serial printer*/
      printf("Serial printer is present.\n");
   data = (eq >> 9) & 7;         /*serial ports*/
   printf("Number of serial ports is %d.\n", data);
   if(eq & 1) {                  /*diskette drives*/
      data = (eq >> 6) & 3;
      printf("Number of diskette drives is %d.\n", data + 1);
   }
   else
      printf("No diskette drives attached.\n");
   data = (eq >> 4) & 3;         /*video mode*/
   switch (data) {
      case 1:
         printf("Video is 40 column color.\n");
         break;
      case 2:
         printf("Video is 80 column color.\n");
         break;
      case 3:
         printf("Video is 80 column monochrome.\n");
         break;
   }
   farptr = (int far *) MEMSIZ;       /*reset to mem size word*/
   printf("Memory is %d Kybtes.\n", *(farptr));
}

