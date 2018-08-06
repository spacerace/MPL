/* SHIFTEST.C - From page 340 of "Microsoft C Programming for   */
/* the IBM" by Robert Lafore. This program demonstrates the     */
/* bitwise right shift operator.                                */
/****************************************************************/

#define TRUE 1

main()
{
unsigned char x1, x2;

   while(TRUE) {
      printf("\n\nEnter hex number (ff or less) and number of bits ");
      printf("\nto shift (8 or less; example 'cc 3'): ");
      scanf("%x %d", &x1, &x2);
      printf("\n%02x >> %d = %02x\n", x1, x2, x1 >> x2);
   }
}

