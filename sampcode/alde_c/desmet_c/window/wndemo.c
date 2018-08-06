
#include <stdio.h>
#include <window.h>

main()	/* demonstration */
{
	WINDOWPTR wn, wn1;
	int		i;

/*
	for (i = 1; i < 25; i++)
		printf("The quick brown fox jumped over the lazy dog's back.\n");
*/
	if ((wn = draw_window(10, 3, 30, 10, (get_mode() < 4) ? 7 : 0)) == NULL)
	{
		printf("\n--- Error opening first window\n");
		exit(1);
	}
	show_mail(wn);

	if ((wn1 = draw_window(15, 5, 30, 1, (get_mode() < 4) ? 0x70 : 255)) == NULL)
	{
		printf("\n--- Error opening second window\n");
		exit(1);
	}
	write_text(wn1, " Read your mail more often!");

	ci();
	remove_window(wn1);

	for (i = 5; i > -10; i -= 2)
	{
		clr_window(wn);
		wn->cx = i;
		show_mail(wn);
		ci();
	}

	remove_window(wn);
	exit(0);
}


show_mail(wn)		/* display the MAIL message */
{
	write_text(wn, "Mail System");
	write_text(wn, "-----------");
	write_text(wn, "Messages Received:  3");
	write_text(wn, "From          Date      Time");
	write_text(wn, "----         ------     ----");
	write_text(wn, "John         1/24/83    0935");
	write_text(wn, "Kim          1/23/83    1547");
	write_text(wn, "Sam          1/23/83    1117");
	write_text(wn, "This line of text should be truncated before here.");
}
