/* HEX2BIN.C - From page 342 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. Converts hex to binary.           */
/****************************************************************/

#define TRUE 1

main()
{
int j, num, bit;
unsigned int mask;
char string[10];

   while(TRUE) {
      mask = 0x8000;          /* 1000000 binary */
      printf("\nEnter number: ");
      scanf("%x", &num);
      printf("\nBinary of %04x is:  ", num);
      for (j = 0; j < 16; j++) {    /*for each bit*/
         bit = (mask & num) ? 1 : 0;      /*bit is 1 or 0*/
         printf("%d ", bit);
         if (j == 7)
            printf("-- ");
         mask >>= 1;             /*shift mask to right 1*/
      }
      printf("\n");
   }
}

