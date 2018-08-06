#include <stdio.h>
#include <osbind.h>

rand()
{
	return((Random() >> 3) & 0x7FFF);
}
