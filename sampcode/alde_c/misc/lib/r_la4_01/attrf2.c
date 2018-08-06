/* ATTRF2.C - From page 361 of "Microsoft C Programming for     */
/* the IBM" by Robert Lafore. The program uses bit fields to    */
/* change attributes. Type 'x' to exit.                         */
/****************************************************************/

#define ROMAX 25
#define COMAX 80
#define TRUE 1

main()
{
struct {
   unsigned int foregnd : 3;        /*bits 0,1,2 */
   unsigned int intense : 1;        /*bit 3*/
   unsigned int backgnd : 3;        /*bits 4, 5, 6*/
   unsigned int blinker : 1;        /*bit 7*/
} attr;
char ch;

   printf("\nType 'n' for normal,\n");
   printf("     'u' for underlined,\n");
   printf("     'i' for intensified,\n");
   printf("     'b' for blinking,\n");
   printf("     'r' for reverse video,\n");
   while((ch = getch()) != 'x') {
      switch(ch) {
         case 'n':                     /*set to normal*/
            attr.foregnd = 7;
            attr.backgnd = 0;
            break;
         case 'u':                     /*set to underline*/
            attr.foregnd = 1;
            break;
         case 'i':                     /*toggle intensity*/
            attr.intense = (attr.intense == 1) ? 0 : 1;
            break;
         case 'b':                     /*toggle blinking*/
            attr.blinker = (attr.blinker == 1) ? 0 : 1;
            break;
         case 'r':                     /*set to reverse video*/
            attr.foregnd = 0;
            attr.backgnd = 7;
            break;
      }
      fill(ch, attr);
   }
}

/* fill() */      /*fills screen with char 'ch', attribute 'attr' */
fill(ch, attr)
char ch, attr;
{
int far *farptr;
int col, row;

   farptr = (int far *) 0xB0000000;
   for(row = 0; row < ROMAX; row++)
      for(col = 0; col < COMAX; col++)
         *(farptr + row*COMAX + col) = ch | attr << 8;
}

