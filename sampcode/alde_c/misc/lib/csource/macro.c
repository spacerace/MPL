                                        /* Chapter 6 - Program 2 */
#define WRONG(A) A*A*A          /* Wrong macro for cube     */
#define CUBE(A) (A)*(A)*(A)     /* Right macro for cube     */
#define SQUR(A) (A)*(A)         /* Right macro for square   */
#define ADD_WRONG(A) (A)+(A)    /* Wrong macro for addition */
#define ADD_RIGHT(A) ((A)+(A))  /* Right macro for addition */
#define START 1
#define STOP  7

main()
{
int i,offset;

   offset = 5;
   for (i = START;i <= STOP;i++) {
      printf("The square of %3d is %4d, and its cube is %6d\n",
              i+offset,SQUR(i+offset),CUBE(i+offset));
      printf("The wrong of  %3d is %6d\n",i+offset,WRONG(i+offset));
   }

   printf("\nNow try the addition macro's\n");
   for (i = START;i <= STOP;i++) {
      printf("Wrong addition macro = %6d, and right = %6d\n"
                               ,5*ADD_WRONG(i),5*ADD_RIGHT(i));
   }
}
