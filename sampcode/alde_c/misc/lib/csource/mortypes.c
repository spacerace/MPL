                                         /* Chapter 4 - Program 2 */
/* The purpose of this file is to introduce additional data types */

main()
{
int a,b,c;            /* -32768 to 32767 with no decimal point */
char x,y,z;           /* -128 to 127 with no decimal point     */
float num,toy,thing;  /* 3.4E-38 to 3.4E+38 with decimal point */

   a = b = c = -27;
   x = y = z = 'A';
   num = toy = thing = 3.6792;

   a = y;           /* a is now 65 (character A)               */
   x = b;           /* x is now -27                            */
   num = b;         /* num will now be -27.00                  */
   a = toy;         /* a will now be 3                         */

}
