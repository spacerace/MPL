
#include <stdio.h>
#include "window.h"


int	insert;		/* Global flag to indicate if in insert mode. */
main()			/* demonstration */
{
	BORDER wborder;
	WINDOW *wn, *wn1, *wopen();
	char c;
	int i;

	insert =0;
	wborder.ul = 0x02da;
	wborder.ur = 0x02bf;
	wborder.ll = 0x02c0;
	wborder.lr = 0x02d9;
	wborder.v  = 0x02b3;
	wborder.h  = 0x02c4;

/*
	for (i = 1; i < 25; i++)
	  printf("The quick brown fox jumped over the lazy dog's back.\n");
*/
	set_page(0);
	if ((wn = wopen(&wborder,2, 2, 64, 18, (get_mode() < 4) ? 14 : 120))
		== NULL)
	{
		printf("\n--- Error opening first window\n");
		exit(1);
	}
	show_mail(wn);

	if ((wn1 = wopen(&wborder, 15, 5, 30, 1, (get_mode() < 4) ? 14 :10)) 
		== NULL)
	{
		printf("\n--- Error opening second window\n");
		exit(1);
	}
	wputstr(wn1, " Read your mail more often!");
	getch();
	wcursor(wn1, HOME);
	wins_row(wn1);

	getch();
	wclose(wn1);

	for (i = -4; i < 5; ++i)
	{
		wcls(wn);
		wn->xmargin = i;
		show_mail(wn);
		getch();
	}
	for(;;) {
		switch ( (c = getch()) ) {
			case 0:
				switch ( (c = getch()) ) {
					case INSERT:
						insert = ~insert;
						showcur(wn);
						break;
					case DELETE:
						hidecur();
						wdelch(wn);
						showcur(wn);
						break;
					default:
						if(wcursor(wn,c) <0) {
							putch(7);
							break;
						}
						showcur(wn);
						break;
				}
				break;
			case 0x17:		/* ^W */
				wclose(wn);
				if(get_page())
					set_page(0);
				else
					set_page(1);
				if ((wn = wopen(&wborder,0, 0, 78, 23,
				 (get_mode() < 4) ? 14 : 120)) == NULL) {
		 		 printf("\n--- Error opening first window\n");
				 exit(1);
				}
				show_mail(wn);
				wprinta(10,0,wn,"\n\nPage %u", get_page());
				wcursor(wn, HOME);
				showcur(wn);

				break;
			case 0x1b:		/* Esc */
				goto bye;
			case 0x14:		/* ^T */
				hidecur();
				wceol(wn);
				showcur(wn);
				break;
			case 0x19:		/* ^Y */
				hidecur();
				wdel_row(wn);
				showcur(wn);
				break;
			default:
				hidecur();
				if(insert)
					winsch(wn, c);
				else
					wputch(wn, c);
				showcur(wn);
				break;
		}
	}
bye:	wclose(wn);
	exit(0);
}

hidecur()
{
	curtype(15,14);
}

showcur(wn)
WINDOW *wn;
{
	if(wbound(wn)) {
		hidecur();
	} else {
		if(insert)
			curtype(0,11);
		else
			curtype(10,11);
	}
}

show_mail(wn)		/* display the MAIL message */
{
	hidecur();
	wputs(wn, "123456789-123456789-123456789-123456789-\r\n");
	wputs(wn, "\nMail System\r\n");
	wputs(wn, "-----------\r\n");
	wputs(wn, "Messages Received:  3\r\n");
	wputs(wn, "From          Date      Time\r\n");
	wputs(wn, "----         ------     ----\r\n");
	wputs(wn, "John         1/24/83    0935\r\n");
	wputs(wn, "Kim          1/23/83    1547\r\n");
	wputs(wn, "Sam          1/23/83    1117\r\n");
	wputs(wn, "\r\nEnter characters and use the cursor keys.\r\n");
	wputs(wn, "Also try ^T, ^Y and ^W.\r\n");
	wputs(wn, 
	"\r\nAfter a while, the ESC character returns you back to DOS.\r\n");
	wputs(wn, "\r\n\n\t\t    Have fun!    ");
	showcur(wn);
}
