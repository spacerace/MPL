/* DEFINED.C: Demonstrate defined operator. */

#define DEBUG 12345

main()
{
   #if defined( DEBUG )
      printf( "Hi\n" );
   #endif
}
