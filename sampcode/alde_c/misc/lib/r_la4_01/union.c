/* UNION.C - From page 314 of "Microsoft C Programming for the  */
/* IBM" by Robert Lafore. Demonstrates unions. This program     */
/* gives an error: Fatal error:L1101 Invalid object module      */
/* when linking. In Quick C Programmers Guide user is reques-   */
/* ted to contact Microsoft.                                    */
/****************************************************************/

main()
{
union intflo {
   int intnum;
   float fltnum;
} unex;

   printf("sizeof (union intflo) = %d\n", sizeof(union intflo));
   unex.intnum = 734;
   printf("unex.intnum = %d\n", unex.intnum);
   unex.fltnum = 867.43;
   printf("unex.fltnum = %f\n", unex.fltnum);
}

