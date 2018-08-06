                                         /* Chapter 6 - Program 3 */
main()
{
enum {win,tie,bye,lose,no_show} result;
enum {sun,mon,tues,wed,thur,fri,sat} days;

   result = win;
   printf("    win = %d\n",result);
   result = lose;
   printf("   lose = %d\n",result);
   result = tie;
   printf("    tie = %d\n",result);
   result = bye;
   printf("    bye = %d\n",result);
   result = no_show;
   printf("no show = %d\n\n",result);

   for(days = mon;days < fri;days++)
      printf("The day code is %d\n",days);
}
