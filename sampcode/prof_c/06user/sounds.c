/*
 *	sounds -- make various sounds on demand
 */

#include <stdio.h>
#include <conio.h>
#include <math.h>

#define ESC	27

extern void sound(unsigned int, float);

main()
{
	int ch;

	fprintf(stderr, "1=warble 2=error 3=confirm 4=warn\n");
	fprintf(stderr, "Esc=quit\n");
	while ((ch = getch()) != ESC)
		switch (ch) {
		case '1':
			warble();
			break;
		case '2':
			error();
			break;
		case '3':
			confirm();
			break;
		case '4':
			warn();
			break;
		}
	exit(0);
}

#define CYCLES	3
#define LOTONE	600
#define HITONE	1200
#define PERIOD	0.1

warble()
{
	int i;

	for (i = 0; i < 2 * CYCLES; ++i)
		if (i % 2)
			sound(LOTONE, PERIOD);
		else
			sound(HITONE, PERIOD);
}

error()
{
	float d = 0.15;

	sound(440, d);
	sound(220, d);
}

confirm()
{
	float d = 0.15;

	sound(440, d);
	sound(880, d);
}

warn()
{
	float d = 0.2;

	sound(100, d);
}
