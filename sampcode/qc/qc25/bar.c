/* BAR.C:  Create sample bar chart. */
#include <conio.h>
#include <string.h>
#include <graph.h>
#include <pgchart.h>
#define MONTHS 12
typedef enum {FALSE, TRUE} boolean;
float far value[MONTHS] = 
{ 
   33.0, 27.0, 42.0, 64.0,106.0,157.0,
  182.0,217.0,128.0, 62.0, 43.0, 36.0
};
char far *category[MONTHS] =
{
  "Jan", "Feb", "Mar", "Apr",
  "May", "Jun", "Jly", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};

main()
{
  chartenv env;
  int mode = _VRES16COLOR;
  /* Set highest video mode available */
  while(!_setvideomode( mode ))
     mode--;
  if(mode == _TEXTMONO)
     return(0);

  /* Initialize chart library and a default bar chart */     
  _pg_initchart();

  _pg_defaultchart( &env, _PG_BARCHART, _PG_PLAINBARS 
);
  /* Add titles and some chart options */
  strcpy( env.maintitle.title, "Good Neighbor Grocery" );
  env.maintitle.titlecolor = 6;
  env.maintitle.justify = _PG_RIGHT;
  strcpy( env.subtitle.title, "Orange Juice Sales" );
  env.subtitle.titlecolor = 6;
  env.subtitle.justify = _PG_RIGHT;
  strcpy( env.yaxis.axistitle.title, "Months" );
  strcpy( env.xaxis.axistitle.title, "Quantity (cases)" );
  env.chartwindow.border = FALSE;

  /* Parameters for call to _pg_chart are:
   *    env        - Environment variable
   *    category   - Category labels
   *    value      - Data to chart
   *    MONTHS     - Number of data values */

  if(_pg_chart( &env, category, value, MONTHS ))
  {
     _setvideomode( _DEFAULTMODE );
     _outtext( "Error:  can't draw chart" );
  }
  else
  {
      getch(); 
      _setvideomode( _DEFAULTMODE );
   }
   return(0);
}
