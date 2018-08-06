/*UNISTRUC.C - From page 317 of "Microsoft C Programming for    */
/* IBM" by Robert Lafore. It demonstrates union of structures.  */
/* This file has same problem as UNION.C on this disk and is    */
/* described in it's source file description located at the     */
/* top of the source just like this description.                */
/****************************************************************/

main()
{
struct twoints {
   int intnum1;
   int intnum2;
} stex;

union intflo {
   struct twoints stex;
   float fltnum;
} unex;

   printf("sizeof(union intflo) = %d\n", sizeof(union intflo));
   unex.stex.intnum1 = 734;
   unex.stex.intnum2 = -333;
   printf("unex.stex.intnum1 = %d\n", unex.stex.intnum1);
   printf("unex.stex.intnum2 = %d\n", unex.stex.intnum2);
   unex.fltnum = 867.43;
   printf("unex.fltnum = %f\n", unex.fltnum);
}


