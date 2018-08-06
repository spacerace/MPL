

/* ---------------------------------------------------------------------- */
/*        Program to calculate # of days between two dates                */
/*        This program was tested & compiled on a DeSmet 'C' compiler     */
/*        ver 2.2 -- but should run on any good 'C' compiler              */
/* ---------------------------------------------------------------------- */

/* Please note that this program only works from 01-12-1600 A.D. onward   */



struct date                                 /*   structure to hold date   */
  {
   int     month;
   int    day;
   int    year;
  } date_1;


long int funct1 (y,m)                       /*   part of # of days calc.  */
    int y, m;
    {
     long int result;
     if ( m <= 2 )
       y -= 1;
     result = y;
     return (result);
    }

long int funct2 (m)
    int m;
    {
     long int result;
     if ( m <= 2 )
       result = m + 13;
     else
       result = m + 1;
       return(result);
    }

/* Function to calculate the number of days in dates */

long int day_count (m, d, y)
    int m, d, y;
    {
     long int number;
     number = 1461 *  funct1(y,m) / 4 + 153 * funct2(m) / 5 + d;

     return (number);
    }

main ()
{
    long int number_of_days1;
    int day_of_week, screw_up = 0;

    printf("\n\n*****************************************************************\n");
    printf("THIS PROGRAM WILL COMPUTE THE DAY OF THE WEEK (SUNDAY - SATURDAY)\n");
    printf("\t\tTHAT A GIVEN DATE WILL FALL ON\n");
    printf("*****************************************************************\n\n");

    printf ("Enter a date (mm dd yyyy) i.e. 03 12 1985  \n");
    scanf ("%d %d %d", &date_1.month, &date_1.day, &date_1.year);

    number_of_days1 = day_count (date_1.month, date_1.day, date_1.year);

    printf ("\nThe date is:  " );

    day_of_week = (number_of_days1 - 621049) % 7;

    switch (day_of_week)
      {
        case 0 :
            printf ("Sunday,");
            break;
        case 1 :
            printf ("Monday,");
            break;
        case 2 :
            printf ("Tuesday,");
            break;
        case 3 :
            printf ("Wednesay,");
            break;
        case 4 :
            printf ("Thursday,");
            break;
        case 5 :
            printf ("Friday,");
            break;
        case 6 :
            printf ("Saturday,");
            break;
        default:
            printf ("Something is screwed up -- Maybee you entered\n");
            printf ("a date earlier than 01 12 1600\n\n");
            screw_up = 1;
      }
      if ( !screw_up )
        printf (" %02d/%02d/%02d\n", date_1.month, date_1.day, date_1.year);
}