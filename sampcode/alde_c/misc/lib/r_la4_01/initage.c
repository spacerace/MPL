/****************************************************************/
/* INITAGE.C - From page 295 of the book "Microsoft C Program-  */
/* ming for the IBM by Robert Lafore. This is a continuation of */
/* a simple database program developed from page 292 to         */
/* the end of the chapter (9).                                  */
/****************************************************************/

struct personnel {
   char name [30];
   int agnumb;
};

struct personnel agent1 = {
   "Harrison Tweedbury", 012 };
struct personnel agent2 = {
   "James Bond", 007 };

main()
{
   printf("\nList of agents:\n\n");
   printf("   Name: %s\n", agent1.name);
   printf("   Agent number: %03d\n", agent1.agnumb);
   printf("   Name: %s\n", agent2.name);
   printf("   Agent number: %03d\n", agent2.agnumb);
}


