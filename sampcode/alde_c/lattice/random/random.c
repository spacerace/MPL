/*
    test deck for random.asm  - random number generator
*/
extern unsigned xrand;
main()
{
     unsigned random();
     int   i,j;

 /*  xrand = 23965;  */

     for (i=1; i != 10; ++i) {
	  for (j=1; j != 5; ++j)
	       printf("  %d\t",random());
	  printf("\n");
     }
}
