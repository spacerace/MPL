/****************************************************************/
/* TEAM.C - From page 297 of the book "Microsoft C Program-     */
/* ming for the IBM by Robert Lafore. This is a continuation of */
/* a simple database program developed from page 292 to         */
/* the end of the chapter (9).                                  */
/****************************************************************/

struct personnel {
   char name [30];
   int agnumb;
};

struct team {
   struct personnel chief;      /*structure within structure*/
   struct personnel indian;
};

struct team team1 = {                     /*declares and initializes*/
   { "Harrison Tweedbury", 102 },         /*struct variable team1   */
   { "James Bond", 007         }
};

main()
{

   printf("\nChief:\n");
   printf("   Name: %s\n", team1.chief.name);
   printf("   Agent number: %03d\n", team1.chief.agnumb);
   printf("\n\nIndian:\n");
   printf("   Name: %s\n", team1.indian.name);
   printf("   Agent number: %03d\n", team1.indian.agnumb);
}



