/* PIE.C:  Create sample pie chart.  */
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
short far explode[MONTHS] = {0};

main()
{
  chartenv env;
  int mode = _VRES16COLOR;

  /* Set highest video mode available */
  while(!_setvideomode( mode ))
     mode--;
  if(mode == _TEXTMONO)
     return( 0 );

  /* Initialize chart library and a default pie chart */
  _pg_initchart();
  _pg_defaultchart( &env, _PG_PIECHART, _PG_PERCENT );

  /* Add titles and some chart options */
  strcpy( env.maintitle.title, "Good Neighbor Grocery" );
  env.maintitle.titlecolor = 6;
  env.maintitle.justify = _PG_RIGHT;
  strcpy( env.subtitle.title, "Orange Juice Sales" ); 
  env.subtitle.titlecolor = 6;
  env.subtitle.justify = _PG_RIGHT;
  env.chartwindow.border = FALSE;

  /* Parameters for call to _pg_chartpie are:
   *  
   *    env        - Environment variable
   *    category   - Category labels
   *    value      - Data to chart
   *    explode    - Separated pieces
   *    MONTHS     - Number of data values
   */

  if(_pg_chartpie( &env, category, value,
                    explode, MONTHS ))
  {
     _setvideomode( _DEFAULTMODE );
     _outtext( "Error:  can't draw chart" );
  }
  else
  {
     getch(); 
     _setvideomode( _DEFAULTMODE );
  }
  return( 0 );
}
