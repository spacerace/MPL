#include <dos.h>
dosdate(date)
char date[];
{
 	char mon[3],
		 day[3],
		 yy[5];
	union REGS DREG;
	DREG.h.ah=0x2a;
	intdos(&DREG.h,&DREG.h);
	stcu_d(mon,DREG.h.dh,3);
	stcu_d(day,DREG.h.dl,3);
	stcu_d(yy,DREG.x.cx,5);
	strcpy(date,mon);
	strcat(date,"-");
	strcat(date,day);
	strcat(date,"-");
	strcat(date,yy);

}
                                               