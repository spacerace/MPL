#include <stdio.h>

long atol(number)
register char *number;
{
	register long n = 0, neg = 0;

	while((*number == ' ') || (*number == '\t'))
		++number;
	if(*number == '-') {
		neg = 1;
		++number;
	}
	else if(*number == '+')
		++number;
	while(isdigit(*number))
		n = (n * 10) + ((*number++) - '0');
	return(neg ? -n : n);
}

int atoi(number)
char *number;
{
	return(atol(number));
}
