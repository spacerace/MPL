/* EMPLOYEE.C: Demonstrate structures. */
#include <stdio.h>
#include <string.h>

struct employee
{
   char name[10];
   int months;
   float wage;
};

void display( struct employee show );

main()
{
   struct employee jones;

   strcpy( jones.name, "Jones, J" );
   jones.months = 77;
   jones.wage = 13.68;

   display( jones );
}

void display( struct employee show )
{
   printf( "Name: %s\n", show.name );
   printf( "Months of service: %d\n", show.months );
   printf( "Hourly wage: %6.2f\n", show.wage );
}
