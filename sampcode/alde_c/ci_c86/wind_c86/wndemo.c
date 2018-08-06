/***************************************************************
**      Dec 84  Converted to compile using CI C86 compiler    **
**      Use   link wndemo+vlib,,,c86s2s  to link.             **
**      by Mike Elkins (619) 722-4363                         **
***************************************************************/   

#include <stdio.h>
#include <window.h>
#include <window.c>

main()	/* demonstration */
{
    WINDOWPTR wn, wn1;
    int	i;

    /*
	for (i = 1; i < 25; i++)
		printf("The quick brown fox jumped over the lazy dog's back.\n");
*/
/*  Draw bottom window and fill with text from show_mail() below */
    if ((wn = draw_window(10, 3, 60, 10, 0x70)) == NULL) {
        printf("\n--- Error opening first window\n");
        exit(1);
    }
    show_mail(wn);

/* Draw second window on top of first with reversed fore/background */
    if ((wn1 = draw_window(15, 5, 30, 3, 7 )) == NULL) {
        printf("\n--- Error opening second window\n");
        exit(1);
    }
    write_text(wn1, " Read your mail more often!");

    getchar();			/* Wait for any key to be pressed */
    remove_window(wn1); /* Get rid of top window */


/* This technique will cause the window to scroll from right to left */
    for (i = 5; i > -10; i -= 2) {
        clr_window(wn);
        wn->cx = i;
        show_mail(wn);
        getchar();
    }
    insert_row(wn, 4);   /* Insert an empty row  */
    getchar();
    wn->cy = 10;         /* Place cursor on tenth row */
    write_text(wn, "******************************");
    show_mail(wn);       /* scroll everything up */
    getchar();

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


