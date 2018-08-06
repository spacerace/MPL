/* BITCALC.C - From page 345 of "Microsoft C Programming for    */
/* the IBM" by Robert Lafore. It demonstrates the use of the    */
/* 6 bitwise operators.  x1 and x2 were made unsigned int       */
/* (different from book) to make the >> operator not insert     */
/* a 1 in the leftmost bit.                                     */
/****************************************************************/

#define TRUE 1

main()
{
char op[10];
unsigned int x1, x2;

   while(TRUE) {
      printf("\n\nEnter expression (example 'ff00 & 1111'): ");
      scanf("%x %s %x", &x1, op, &x2);
      printf("\n\n");
      switch(op[0]) {
         case '&':
            pbin(x1);
            printf("& (and)\n");
            pbin(x2);
            pline();
            pbin(x1 & x2);
            break;
         case '|':
            pbin(x1);
            printf("| (incl or)\n");
            pbin(x2);
            pline();
            pbin(x1 | x2);
            break;
         case '^':
            pbin(x1);
            printf("^ (excl or)\n");
            pbin(x2);
            pline();
            pbin(x1 ^ x2);
            break;
         case '>':
            pbin(x1);
            printf(">> ");
            printf("%d\n", x2);
            pline();
            pbin(x1 >> x2);
            break;
         case '<':
            pbin(x1);
            printf("<< ");
            printf("%d\n", x2);
            pline();
            pbin(x1 << x2);
            break;
         case '~':
            pbin(x1);
            printf("~ (compliment)\n");
            pline();
            pbin(~x1);
            break;
         default:
            printf("\nNot valid operator.\n");
      }
   }
}

/* pbin */  /*prints number in hex and binary*/
pbin(num)
int num;
{
unsigned int mask;
int j, bit;

   mask = 0x8000;                /*one-bit mask*/
   printf("%04x  ", num);        /*print in hex*/
   for(j = 0; j < 16; j++) {     /*for each bit in num*/
      bit = (mask & num) ? 1 : 0;   /*bit is 1 or 0*/
      printf("%d ", bit);
      if (j == 7)
         printf("-- ");
      mask >>= 1;             /*shift mask right by 1 bit*/
   }
   printf("\n");
}

/* pline */
pline()
{
   printf("----------------------------------------\n");
}

