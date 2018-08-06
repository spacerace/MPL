/* ATTRFILL.C - From page 358 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. This progam uses direct memory    */
/* access to change the screen attributes. Enter 'x' to quit.   */
/****************************************************************/

#define ROMAX 25
#define COMAX 80
#define TRUE 1

main()
{
char ch, attr;

   printf("\nType 'n' for normal,\n");
   printf("     'u' for underlined,\n");
   printf("     'i' for intensified,\n");
   printf("     'b' for blinking,\n");
   printf("     'r' for reverse video,\n\n");
   while((ch = getch()) != 'x') {
      switch(ch) {
         case 'n':
            attr = 0x07;            /*set to normal*/
            break;                  /*0000 0111*/
         case 'u':
            attr = attr & 0x88;     /*set to underline*/
            attr = attr | 0x01;     /* x000 x001 */
            break;
         case 'i':
            attr = attr ^ 0x08;     /*toggle intensified*/
            break;                  /* xxxx Txxx */
         case 'b':
            attr = attr ^ 0x80;     /*toggle blinking*/
            break;                  /* 1xxx xxx */
         case 'r':
            attr = attr & 0x88;     /*set to reverse video*/
            attr = attr | 0x70;     /* x111 x000 */
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

