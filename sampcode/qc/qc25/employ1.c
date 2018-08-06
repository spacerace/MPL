/* EMPLOY1.C: Demonstrate structure pointers. */
#include <stdio.h>


struct employee
{
   char name[10];
   int months;
   float wage;
};

void display( struct employee *e_ptr  );

main()
{
   static struct employee jones =
      {
      "Jones, J",
      77,
      13.68
      };

   display( &jones );
}

void display( struct employee *e_ptr )
{
   printf( "Name: %s\n", e_ptr->name );
   printf( "Months of service: %d\n", e_ptr->months );
   printf( "Hourly wage: %6.2f\n", e_ptr->wage );
}
