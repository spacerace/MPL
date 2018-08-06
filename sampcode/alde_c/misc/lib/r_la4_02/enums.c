/* ENUMS.C - From page 545 of "Microsoft C Programming for      */
/* the IBM" by Robert Lafore. This program is an example of     */
/* using enumerated data types. This program gives an error:    */
/* Fatal error:L1101 Invalid object module when linking.        */
/* In Quick C Programmers Guide user is requested to contact    */
/* Microsoft.                                                   */
/****************************************************************/

main()
{
enum empcats {
   management,
   research,
   clerical,
   sales
};
struct {
   char name[30];
   float salary;
   enum empcats category;
} employee;

   strcpy(employee.name, "Benjamin Franklin");
   employee.salary = 118.50;
   employee.category = research;

   printf("\n\nName = %s", employee.name);
   printf("\nSalary = %6.2f", employee.salary);
   printf("\nCategory = %d", employee.category);

   if(employee.category == clerical)
      printf("\n\nEmployee category is clerical.");
   else
      printf("\nEmployee category is not clerical.");
}

