/* SCATTER.C:  Create sample scatter diagram. */

#include <conio.h>
#include <string.h>
#include <graph.h>
#include <pgchart.h>

#define MONTHS 12
typedef enum {FALSE, TRUE} boolean;

/* Orange juice sales */

float far xvalue[MONTHS] = 
{ 
   33.0, 27.0, 42.0, 64.0,106.0,157.0,
  182.0,217.0,128.0, 62.0, 43.0, 36.0
};
/* Hot chocolate sales */

float far yvalue[MONTHS] = 
{ 
  37.0, 37.0, 30.0, 19.0, 10.0,  5.0,
   2.0,  1.0,  7.0, 15.0, 28.0, 39.0
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

  /* Initialize chart library and default
   * scatter diagram
   */

  _pg_initchart();

  _pg_defaultchart( &env, _PG_SCATTERCHART,
                    _PG_POINTONLY );

  /* Add titles and some chart options */

  strcpy( env.maintitle.title, "Good Neighbor Grocery" );
  env.maintitle.titlecolor = 6;
  env.maintitle.justify = _PG_RIGHT;
  strcpy( env.subtitle.title,
          "Orange Juice vs Hot Chocolate" );
  env.subtitle.titlecolor = 6;
  env.subtitle.justify = _PG_RIGHT;
  env.yaxis.grid = TRUE;
  strcpy( env.xaxis.axistitle.title,
          "Orange Juice Sales" );
  strcpy( env.yaxis.axistitle.title,
          "Hot Chocolate Sales" );
  env.chartwindow.border = FALSE;

  /* Parameters for call to _pg_chartscatter are:
   *    env        - Environment variable
   *    xvalue     - X-axis data
   *    yvalue     - Y-axis data
   *    MONTHS     - Number of data values
   */

  if(_pg_chartscatter( &env, xvalue,
                        yvalue, MONTHS ))
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
