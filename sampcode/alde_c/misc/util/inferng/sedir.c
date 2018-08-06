
#include <stdio.h>
#include "routine.h"
#include "weather.h"

main()
{
FILE *dataFile ;
int	value[20] ;

dataFile = fopen("WEATHER.DAT","rb") ;
fread(value,2,20,dataFile) ;
fclose(dataFile) ;
if(value[2]==SOUTH_EAST)
	return(RETURN_ROUTINE_TRUE) ;
else
	return(RETURN_ROUTINE_FALSE) ;
}

