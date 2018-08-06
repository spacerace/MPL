#include <stdio.h>
#include <time.h>

/*
 *	goodie.c -- Goofy little program to print random selections from
 *		a list contained in a 'goodiefile'.
 *
 *		Version 1.01, Copyright (C) 1987 by David Sandberg
 *
 *		To use this program, you will want to create a file
 *		called 'goodies.fil' in the \etc directory (you may change
 *		the filename by changing the first 'if', but I highly recommend
 *		using an absolute pathname at least, so the program can find
 *		it no matter where it is executed from), which may contain any
 *		number of goofy quips and quotes you like. If the quip or
 *		quote is more than one line long, each line after the first
 *		must begin with a space character (ASCII 32) - this is how the
 *		program determines that it is a continuation line.
 *		EXAMPLE:

"Now is the time for all good men to
 come to the aid of their country."

 *		will be interpreted correctly as one contiguous entry, while

"Now is the time for all good men to
come to the aid of their country."

 *		will be treated incorrectly as two separate entries.
 *
 *		Note that you should have a carriage return after the last
 *		entry in your 'goodiefile', so that the program can properly
 *		read the last entry. On an editor such as 'vi', this means
 *		a blank line will appear at the end of the list of 'goodies'.
 *
 *		If you run the program dozens of times in a few minutes, you 
 *		may start to see patterns arise. Since the randomize function
 *		is driven by the time of day, this seems unavoidable. The
 *		algorithm is quite suitable for less rigorous applications,
 *		however - I use it in my AUTOEXEC.BAT file to give myself a
 *		random greeting upon powerup, with good results.
 *
 *		This program was compiled using Microsoft C version 4.00. To
 *		compile it with a different C compiler will likely require
 *		mods to the random and time functions. If anyone does so, or
 *		if someone comes up with a better random device, I'd like to
 *		see the results (simply out of curiosity) - write me on
 *		the C Station, (612) 938-4809.
 *
 *		D. Sandberg -- 10/10/87
 */

main()

{
    FILE *goodiefile;
    int filelen, line_no;

    randomize();
    if ((goodiefile = fopen("/etc/goodies.fil", "r")) == NULL) {
	printf("goodiefile missing.\n");
	exit(0);
    }
    filelen = find_size(goodiefile);
    line_no = (rand() % filelen) + 1;
    pick_line(goodiefile, line_no);
}

randomize()

{
    long *timeval, time();

    time(timeval);
    srand((int)*timeval);
}

find_size(goodiefile)
FILE *goodiefile;

{
    char buf[83];
    int filelen = 0;

    while (!feof(goodiefile)) {
	fgets(buf, 82, goodiefile);
	if (buf[0] != ' ')
	    ++filelen;
    }
    rewind(goodiefile);
    return(filelen-2);
}

pick_line(goodiefile, line_no)
FILE *goodiefile;
int line_no;

{
    int i=0;
    char buf[83];

    while (i<line_no) {
	fgets(buf, 82, goodiefile);
	if (buf[0] != ' ')
	    ++i;
    }
    printf("\n%s", buf);
    fgets(buf, 82, goodiefile);
    while (buf[0] == ' ') {
	printf("%s", buf);
	fgets(buf, 82, goodiefile);
    }
}
