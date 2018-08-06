#include <stdio.h>

#include "routine.h"
#include "weather.h"	/* common definitions for direction and stuff */


int main()
/*
**	note that argv[1] is a pointer to our array of common value
*/

{
char	string[20] ;
int	test ;
int	value[20] ;
FILE	*dataFile ;

dataFile = fopen("WEATHER.DAT","wb") ;

for(test = 0 ; test < 20 ; test++)
	value[test] = 0 ;

/*
**	value[0] = barometric pressure * 100
**	value[1] = condition of pressure
**		1-steady, 2-rise_slow, 3-rise_fast, 4-fall-slow, 5-fall-fast
**	value[2] = direction of ground wind 
**		1-n, 2-ne, 3-e, 4-se, 5-s, 6-sw, 7-w, 8-nw
**
*/

printf("\n \nWEATHER EXPERT: \n") ;
printf("\n This program attempts to prove one of the following:");
printf("\n\tthe weather is ok\n\tthe weather is improving\n\tthe weather is deteriorating\n");
printf("\nNot all weather cases are included in the forecast algorithm.");
printf("\n\nBefore we attempt a forecast, I need some data\n What is the barometric pressure reading?\n");
test = 0 ;
while((test < 2000) || (test > 4000))
	{
	printf("\n(Type in the pressure as an integer (BP*100) 30.1 = 3010...?");
	scanf("%d",&value[0]) ;
	test = value[0] ;
	}
printf("\n\t Thankyou!,  \n\n Now I need to know how the barometer is acting\n");
test = 0;
while((test <1) || (test >5))
	{
	printf("\n\t Please input the correct number for the following:\n");
	printf("\n\t 1 -- It is steady\n\t 2 -- It is rising slowly\n\t 3 -- It is rising rapidly") ;
	printf("\n\t 4 -- It is falling slowly\n\t 5 -- It is falling rapidly \n?");
	scanf("%d",&value[1]);
	test = value[1] ;
	}
test = 0 ;
printf("\n\t Thankyou,") ;
while((test < 1) || ( test > 8 ) )
	{
	printf("\n\nNow I need the wind direction, which direction is it blowing from?\n");
	printf("<n, ne, e, se, s, sw, w, nw>?") ;
	scanf("%s",string) ;
	if(0 == strcmp(string,"n"))
		value[2]=NORTH ;
	if(0 == strcmp(string,"ne"))
		value[2]=NORTH_EAST ;
	if(0 == strcmp(string,"e"))
		value[2]=EAST ;
	if(0 == strcmp(string,"se"))
		value[2]=SOUTH_EAST ;
	if(0 == strcmp(string,"s"))
		value[2]=SOUTH ;
	if(0 == strcmp(string,"sw"))
		value[2]=SOUTH_WEST ;
	if(0 == strcmp(string,"w"))
		value[2]=WEST ;
	if(0 == strcmp(string,"nw"))
		value[2]=NORTH_WEST ;
	if(0 == strcmp(string,"N"))
		value[2]=NORTH ;
	if(0 == strcmp(string,"NE"))
		value[2]=NORTH_EAST ;
	if(0 == strcmp(string,"E"))
		value[2]=EAST ;
	if(0 == strcmp(string,"SE"))
		value[2]=SOUTH_EAST ;
	if(0 == strcmp(string,"S"))
		value[2]=SOUTH ;
	if(0 == strcmp(string,"SW"))
		value[2]=SOUTH_WEST ;
	if(0 == strcmp(string,"W"))
		value[2]=WEST ;
	if(0 == strcmp(string,"NW"))
		value[2]=NORTH_WEST ;
	test = value[2] ;
	}
printf("\n\n\n\n For the next question, you should stand outside with your back");
printf("\nto the surface wind.  You now must observe the direction the");
printf("\nupper level clouds are moving.  You observe them to be moving") ;
printf("\nfrom your right, from your left, or in a direction parallel") ;
printf("\nto that which your are facing.\n") ;
printf("\nUse this information to answer the following question.  If you");
printf("\nare unable to see the upper level clouds, answer no to the following");
printf("\nquestion.\n\n") ;
fwrite(value,2,20,dataFile) ;
fclose(dataFile) ;
exit(RETURN_ROUTINE_TRUE) ;
}





