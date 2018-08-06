#include <stdio.h>
#include "window.h"
#include <signal.h>
void (*signal())();
char color[] =
{
	14,13,12,11,10
};

main()
{
	int i,j,k;
	WINDOW *wopen(), *windows[5];
	BORDER wborder;
	(void)signal(SIGINT,SIG_IGN);
	set_page(1);

	wborder.ul = 0x3BDA;
	wborder.ur = 0x3BBF;
	wborder.ll = 0x3BC0;
	wborder.lr = 0x3BD9;
	wborder.v  = 0x3BB3;
	wborder.h  = 0x3BC4;
	for (i = 0; i < 5; i++)
	{
		if (NULL ==
			(windows[i] = 
			wopen(&wborder,(i * 10),	/* x */
						   (i * 4),		/* y */
						   30,10,
						   color[i]
		   )))
		{
			set_page(0);
			printf("Error opening window #%d\n",i);
			goto bye;
		}
		wcls(windows[i]);
		for (j = 0; j < 9; j++)
			wprintf(windows[i],"Hello everybody from window %d!!!\r\n",
				i);
	}
	for (i = 4 ; i >= 0; i--)
	{
		bdos(7);
		wclose(windows[i]);
	}
bye:
	set_page(0);
	exit(0);
}
