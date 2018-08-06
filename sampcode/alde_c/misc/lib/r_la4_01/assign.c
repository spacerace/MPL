/* ASSIGN.C - From Microsoft C Programming for the IBM by Rob-  */
/* ert Lafore, page 270. This program will assign a string(com- */
/* mand) typed in by user to a chosen function key.             */
/****************************************************************/

main()
{
char string[81];
int key;

   printf("Enter number of function key: ");
   gets(string);
   key = atoi(string);
   puts("Enter string to assign to that key: ");
   gets(string);
   printf("\x1B[0;%d;\"%s\";13p", key + 58, string);
}

