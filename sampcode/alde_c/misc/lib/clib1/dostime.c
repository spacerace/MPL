/********************************************************
 *  THIS FUNCTION DOES A CALL TO THE MS-DOS OPERATING   *
 *  SYSTEM AND RETURNS THE TIME.  YOU SHOULD PASS A 11  *
 *  CHARACTER ARRAY ADDRESS TO THE FUNCTION IN  WHICH			*
 *  THE TIME WILL BE PLACED UPON RETURN.  THE CALL IS   *
 *  AS FOLLOWS.																																									*
 *                char time[11];																								*
 *					 dostime(time);																								*
 *  TIME WILL HAVE THE SYSTEM TIME IN IT IN THE FORM    *
 *                HH:MM:SS:HH                           *
 *  HAVE FUN AND GOOD LUCK                              *
 *                                                      *
 *                LYNN LONG                             *
 *                TULSA IBBS C-SIG                      *
 *                1200 BAUD, 24 HOURS                   *
 *                SUBSCRIPTION BBS                      *
 *                918-749-0718                          *
 ********************************************************/

#include <dos.h>
dostime(time)
char time[];
{
	char hr[3],
		 mm[3],
		 ss[3],
		 hh[3]; 
	struct HREG TREG;
	TREG.ah=0x2c;
	intdos(&TREG,&TREG);
 	stcu_d(hr,TREG.ch,3);
	stcu_d(mm,TREG.cl,3);
	stcu_d(ss,TREG.dh,3);
	stcu_d(hh,TREG.dl,3);
	strcpy(time,hr);
	strcat(time,":");
	strcat(time,mm);
	strcat(time,":");
	strcat(time,ss);
	strcat(time,":");
	strcat(time,hh);

}
                                                                                      