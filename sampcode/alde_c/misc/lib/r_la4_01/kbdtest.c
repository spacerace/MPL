/*KBDTEST.C - Prints code of extended char set (F keys, Arrow, etc.).*/
/* From the book "Microsoft C Programming for the IBM" by Robert     */
/* Lafore, page 253.                                                 */
/*********************************************************************/

main()
{
char key, key2;

   while((key = getch()) != 'X')
      if (key == 0) {                     /* If extended code */
         key2 = getch();                  /* Read second code */
         printf("%4d, %4d\n", key, key2);
      }
      else
         printf("%4d, %12c\n", key, key);      /* Not extended code */
}

