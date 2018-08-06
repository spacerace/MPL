/*
This program is supposed to test the compatibility of VT100-compatible
terminals. However: it's also a nice demo of the capabilities of a VT100.
The author is not on the net but I will forward comments and ideas of
enhancements to him. What other VT100 functions should be tested?

The program was written under TOPS-10 so the author is probably not very
interested in enhancements pertaining to UN*X.
The responsibility for distributing this program is mine, the author has
only given his consent.

    {decvax,philabs}!mcvax!enea!ttds!johanw

Cut at this line
=========================================================================*/
/*
                               VTTEST.C

                Written nov-dec 1983 by Per Lindberg,
          Stockholm University Computer Center (QZ), Sweden.

                  THE MAD PROGRAMMER STRIKES AGAIN!

This is a program to test the compatibility (or to demonstrate the
non-compatibility) of so-called "VT100-compatible" terminals. In
conformance of the good old hacker traditions, the only documentation
of this program is the source code itself. To understand it, you also
need a copy of the original VT100 manual from DEC.

This program is not nearly finished yet, it only tests about half
of what it should. And there is neither any support for all the
different variations of VT100, like VT125, VT131 or the new VT200
series. I might fix that someday, but in the meantime, feel free to
snarf my program and hack it up.

The original version of this program was (and is still) written in a
losing implementation of C, the John Hopkins University portable JHU
C, as installed on the DECsystem-10 under TOPS-10. It is not a full
implementation of C (as defined in "the C programming language" by
Kernighan & Ritchie). One of its worst cretinisms is that identifiers
are only significant to the first five characters. I hope to get a
better C compiler for the DEC-10 soon.

*/

#include <stdio.h>


char *getlin(str)
char *str;
{
	static char buf[100];
	printf("%s", str);
	return(gets(buf));
}

main () {


#define vt100 1
#define vt102 2

    int     vttype;

    vttype = inittype ();
    tst_movements ();
    tst_screen ();
    tst_characters ();
    tst_doublesize ();
    tst_reports ();
    tst_vt52 ();
    bye ();
}

tst_movements () {

 /* Test of: CUF (Cursor Forward) CUB (Cursor Backward) CUD (Cursor Down)      IND (Index)  NEL (Next Line) CUU (Cursor
    Up)        RI  (Reverse Index) CUP (Cursor Position)  HVP (Horizontal and Vertical Position) ED  (Erase in Display)
    EL  (Erase in Line) PCALN (Screen Alignment Display) <CR> <BS> */

    int     row, col;

/*  pcaln();  */
    ed (2);
    pcaln ();
    cup (9, 10);
    ed (1);
    cup (18, 60);
    ed (0);
    el (1);
    cup (9, 71);
    el (0);
    for (row = 10; row <= 16; row++) {
	cup (row, 10);
	el (1);
	cup (row, 71);
	el (0);
    }
    cup (17, 30);
    el (2);
    for (col = 1; col <= 80; col++) {
	hvp (24, col);
	printf ("*");
	hvp (1, col);
	printf ("*");
    }
    cup (2, 2);
    for (row = 2; row <= 23; row++) {
	printf ("+");
	cub (1);
	ind ();
    }
    cup (23, 79);
    for (row = 23; row >= 2; row--) {
	printf ("+");
	cub (1);
	ri ();
    }
    cup (2, 1);
    for (row = 2; row <= 23; row++) {
	printf ("*");
	cup (row, 80);
	printf ("*");
	cub (10);
	if (row < 10)
	    nel ();
	else
	    printf ("\n");
    }
    cup (2, 10);
    cub (42);
    cuf (2);
    for (col = 3; col <= 78; col++) {
	printf ("+");
	cuf (0);
	cub (2);
	cuf (1);
    }
    cup (23, 70);
    cuf (42);
    cub (2);
    for (col = 78; col >= 3; col--) {
	printf ("+");
	cub (1);
	cuf (1);
	cub (0);
	printf ("%c", 8);
    }
    cup (1, 1);
    cuu (10);
    cuu (1);
    cuu (0);
    cup (24, 80);
    cud (10);
    cud (1);
    cud (0);

    cup (10, 12);
    for (row = 10; row <= 15; row++) {
	for (col = 12; col <= 69; col++)
	    printf (" ");
	cud (1);
	cub (58);
    }
    cuu (5);
    cuf (1);
    printf ("The screen should be cleared, and have a unbroken border");
    cup (12, 13);
    printf ("of *'s and +'s around the edge, and exactly in the");
    cup (13, 13);
    printf ("middle there should be a frame of E's around this text");
    cup (14, 13);
    printf ("with one (1) free position around it. Now, push <RETURN>");
    getlin ("");
}

tst_screen () {

 /* Test of: - PCSTBM (Set Top and Bottom Margins) - TBC    (Tabulation Clear) - HTS    (Horizontal Tabulation Set) -
    SM RM  (Set/Reset mode): - 80/132 chars - Origin: Realtive/absolute - Scroll: Smooth/jump - Wraparound */

    int     i, row, col, down, soft;

    ed (2);
    sm ("?7");						/* Wrap Around ON */
    cup (1, 1);
    for (col = 1; col <= 160; col++)
	printf ("*");
    rm ("?7");						/* Wrap Around OFF */
    cup (3, 1);
    for (col = 1; col <= 160; col++)
	printf ("*");
    sm ("?7");						/* Wrap Around ON */
    cup (5, 1);
    printf ("This should be three identical lines of *'s completely filling\n");
    printf ("the top of the screen without any empty lines between.\n");
    getlin ("Push <RETURN>");

    ed (2);
    cup (1, 1);
    for (col = 1; col <= 79; col++)
	printf ("+");
    for (row = 1; row <= 24; row++) {
	hvp (row, 80);
	printf ("*");
    }
    cup (10, 10);
    printf ("This illustrates the \"wrap around bug\" which exists on a");
    cup (11, 10);
    printf ("standard VT100. On the top of the screen there should be");
    cup (12, 10);
    printf ("a row of +'s, and the rightmost column should be filled");
    cup (13, 10);
    printf ("with *'s. But if the bug is present, some of the *'s may");
    cup (14, 10);
    printf ("be placed in other places, e.g. in the leftmost column,");
    cup (15, 10);
    printf ("and the top line of +'s may be scrolled away.");
    cup (17, 10);
    printf ("Of course, a good VT100-compatible terminal should have");
    cup (18, 10);
    printf ("means of shutting this bug off (if it has it at all!)");
    cup (20, 10);
    getlin ("Push <RETURN>");

    ed (2);
    tbc (3);
    cup (1, 1);
    for (col = 1; col <= 78; col + = 3) {
	cuf (3);
	hts ();
    }
    cup (1, 4);
    for (col = 4; col <= 78; col + = 6) {
	tbc (0);
	cuf (6);
    }
    cup (1, 7);
    tbc (1);
    tbc (2);						/* no-op */
    cup (1, 1);
    for (col = 1; col <= 78; col + = 6)
	printf ("\t*");
    cup (2, 2);
    for (col = 2; col <= 78; col + = 6)
	printf ("     *");
    cup (4, 1);
    getlin ("These two lines should look the same. Push <RETURN>");
    sm ("?3");						/* 132 cols */
    ed (2);						/* VT100 clears screen on SM3/RM3, but not obviously, so... */
    cup (1, 1);
    tbc (3);
    for (col = 1; col <= 132; col + = 8) {
	cuf (8);
	hts ();
    }
    cup (1, 1);
    for (col = 1; col <= 130; col + = 10)
	printf ("1234567890");
    printf ("12");
    cup (3, 1);
    getlin ("This is 132 column mode. Push <RETURN>");
    rm ("?3");						/* 80 cols */
    ed (2);						/* VT100 clears screen on SM3/RM3, but not obviously, so... */
    cup (1, 1);
    for (col = 1; col <= 80; col + = 10)
	printf ("1234567890");
    cup (3, 1);
    getlin ("This is 80 column mode. Push <RETURN>");
    ed (2);
    sm ("?6");						/* Origin mode (relative) */
    for (soft = -1; soft <= 0; soft++) {
	if (soft)
	    sm ("?4");
	else
	    rm ("?4");
	for (row = 12; row >= 1; row - = 11) {
	    pcstbm (row, 24 - row + 1);
	    ed (2);
	    for (down = 0; down >= -1; down--) {
		if (down)
		    cuu (24);
		else
		    cud (24);
		for (i = 1; i <= 30; i++) {
		    printf ("%s scroll %s region %d Line %d\n",
			    soft ? "Soft" : "Jump",
			    down ? "down" : "up",
			    2 * (13 - row), i);
		    if (down) {
			ri ();
			ri ();
		    }
		}
	    }
	    getlin ("Push <RETURN>");
	}
    }
    ed (2);
    pcstbm (23, 24);
    printf ("\nThis line should be at the bottom of the screen.");
    cup (1, 1);
    printf ("This line should be the one above the bottom of the screeen.");
    getlin (" Push <RETURN>");
    ed (2);
    rm ("?6");						/* Origin mode (absolute) */
    cup (24, 1);
    printf ("This line should be at the bottom of the screen.");
    cup (1, 1);
    printf ("This line should be at the top of the screen.");
    getlin (" Push <RETURN>");
    pcstbm (1, 24);
}

tst_characters () {
							/* Test of: PCLL   (Load LEDs) SCS    (Select Character Set)
							   SGR    (Select Graphic Rendition) SM RM  (Set/Reset Mode) -
							   Inverse PCSC   (Save Cursor) PCRC   (Restore Cursor) */

    int     i, j, g, cset;
    char   *tststr, chcode[5], *setmsg[5], *ledmsg[6], *ledseq[6], *attr[5];

    tststr = "*qx`";

    chcode[0] = 'A';
    chcode[1] = 'B';
    chcode[2] = '0';
    chcode[3] = '1';
    chcode[4] = '2';
    setmsg[0] = "UK / national (character set A)";
    setmsg[1] = "US ASCII (B)";
    setmsg[2] = "Special graphics and line drawing (0)";
    setmsg[3] = "Alternate character ROM standard characters (1)";
    setmsg[4] = "Alternate character ROM special graphics (2)";

    ledmsg[0] = "L1 L2 L3 L4";
    ledseq[0] = "1;2;3;4";
    ledmsg[1] = "   L2 L3 L4";
    ledseq[1] = "1;0;4;3;2";
    ledmsg[2] = "   L2 L3";
    ledseq[2] = "1;4;;2;3";
    ledmsg[3] = "L1 L2";
    ledseq[3] = ";;2;1";
    ledmsg[4] = "L1";
    ledseq[4] = "1";
    ledmsg[5] = "";
    ledseq[5] = "";

    attr[0] = ";0";
    attr[1] = ";1";
    attr[2] = ";4";
    attr[3] = ";5";
    attr[4] = ";7";

    ed (2);
    cup (10, 1);
    printf ("These LEDs (\"lamps\") on the keyboard should be on:");
    for (i = 0; i <= 5; i++) {
	cup (10, 52);
	el (0);
	printf (ledmsg[i]);
	pcll ("0");
	pcll (ledseq[i]);
	cup (12, 1);
	getlin ("Push <RETURN>");
    }
    ed (2);
    cup (1, 10);
    printf ("Selected as G0 (with SI)");
    cup (1, 48);
    printf ("Selected as G1 (with SO)");
    for (cset = 0; cset <= 4; cset++) {
	scs (1, 'B');
	cup (3 + 4 * cset);
	sgr ("1");
	printf (setmsg[cset]);
	sgr ("0");
	for (g = 0; g <= 1; g++) {
	    scs (g, chcode[cset]);
	    for (i = 1; i <= 3; i++) {
		cup (3 + 4 * cset + i, 10 + 38 * g);
		for (j = 0; j <= 31; j++) {
		    printf ("%c", i * 32 + j);
		}
	    }
	}
    }
    scs (1, 'B');
    cup (24, 1);
    getlin ("These are the installed character sets. Push <RETURN>");

    ed (2);
    cup (1, 20);
    printf ("Graphic rendition test pattern:");
    cup (4, 1);
    sgr ("0");
    printf ("vanilla");
    cup (4, 40);
    sgr ("0;1");
    printf ("bold");
    cup (6, 6);
    sgr (";4");
    printf ("underline");
    cup (6, 45);
    sgr (";1");
    sgr ("4");
    printf ("bold underline");
    cup (8, 1);
    sgr ("0;5");
    printf ("blink");
    cup (8, 40);
    sgr ("0;5;1");
    printf ("bold blink");
    cup (10, 6);
    sgr ("0;4;5");
    printf ("underline blink");
    cup (10, 45);
    sgr ("0;1;4;5");
    printf ("bold underline blink");
    cup (12, 1);
    sgr ("1;4;5;0;7");
    printf ("negative");
    cup (12, 40);
    sgr ("0;1;7");
    printf ("bold negative");
    cup (14, 6);
    sgr ("0;4;7");
    printf ("underline negative");
    cup (14, 45);
    sgr ("0;1;4;7");
    printf ("bold underline negative");
    cup (16, 1);
    sgr ("1;4;;5;7");
    printf ("blink negative");
    cup (16, 40);
    sgr ("0;1;5;7");
    printf ("bold blink negative");
    cup (18, 6);
    sgr ("0;4;5;7");
    printf ("underline blink negative");
    cup (18, 45);
    sgr ("0;1;4;5;7");
    printf ("bold underline blink negative");
    sgr ("");

    rm ("?5");						/* Inverse video off */
    cup (23, 1);
    el (0);
    getlin ("Black background. Push <RETURN>");
    sm ("?5");						/* Inverse video */
    cup (23, 1);
    el (0);
    getlin ("Inverse video. Push <RETURN>");
    rm ("?5");
    ed (2);
    cup (8, 12);
    printf ("normal");
    cup (8, 24);
    printf ("bold");
    cup (8, 36);
    printf ("underscored");
    cup (8, 48);
    printf ("blinking");
    cup (8, 60);
    printf ("reversed");
    cup (10, 1);
    printf ("stars:");
    cup (12, 1);
    printf ("line:");
    cup (14, 1);
    printf ("x'es:");
    cup (16, 1);
    printf ("diamonds:");
    for (cset = 0; cset <= 3; cset++) {
	for (i = 0; i <= 4; i++) {
	    cup (10 + 2 * cset, 12 + 12 * i);
	    sgr (attr[i]);
	    if (cset == 0 || cset == 2)
		scs (0, 'B');
	    else
		scs (0, '0');
	    for (j = 0; j <= 4; j++) {
		printf ("%c", tststr[cset]);
	    }
	    pcsc ();
	    cup (cset + 1, i + 1);
	    sgr ("");
	    scs (0, 'B');
	    printf ("A");
	    pcrc ();
	    for (j = 0; j <= 4; j++) {
		printf ("%c", tststr[cset]);
	    }
	}
    }
    sgr ("0");
    scs (0, 'B');
    cup (22, 1);
    printf ("There should be 10 characters of each flavour, and a rectangle\n");
    getlin ("of 5 x 4 A's filling the top left of the screen. Push <RETURN>");
}

tst_doublesize () {
							/* Test of: PCSWL  (Single Width Line) PCDWL  (Double Width
							   Line) PCDHL  (Double Height Line) (also implicit double
							   width) */

    int     i, w, w1;

 /* Print the test pattern in both 80 and 132 character width  */

    for (w = 0; w <= 1; w++) {
	w1 = 13 * w;

	ed (2);
	cup (1, 1);
	if (w) {
	    sm ("?3");
	    printf ("132 column mode");
	}
	else {
	    rm ("?3");
	    printf (" 80 column mode");
	}

	cup (5, 3 + 2 * w1);
	printf ("v------- left margin");

	cup (7, 3 + 2 * w1);
	printf ("This is a normal-sized line");
	pcdhl (0);
	pcdhl (1);
	pcdwl ();
	pcswl ();

	cup (9, 2 + w1);
	printf ("This is a Double-width line");
	pcswl ();
	pcdhl (0);
	pcdhl (1);
	pcdwl ();

	cup (11, 2 + w1);
	pcdwl ();
	pcswl ();
	pcdhl (1);
	pcdhl (0);
	printf ("This is a Double-width-and-height line");
	cup (12, 2 + w1);
	pcdwl ();
	pcswl ();
	pcdhl (0);
	pcdhl (1);
	printf ("This is a Double-width-and-height line");

	cup (14, 2 + w1);
	pcdwl ();
	pcswl ();
	pcdhl (1);
	pcdhl (0);
	el (2);
	printf ("This is another such line");
	cup (15, 2 + w1);
	pcdwl ();
	pcswl ();
	pcdhl (0);
	pcdhl (1);
	printf ("This is another such line");

	cup (17, 3 + 2 * w1);
	printf ("^------- left margin");

	cup (21, 1);
	printf ("This is not a double-width line");
	for (i = 0; i <= 1; i++) {
	    cup (21, 6);
	    if (i) {
		printf ("**is**");
		pcdwl ();
	    }
	    else {
		printf ("is not");
		pcswl ();
	    }
	    cup (23, 1);
	    getlin ("Push <RETURN>");
	}
    }

    rm ("?3");
    ed (2);

    scs (0, '0');
    cup (8, 1);
    pcdhl (0);
    printf ("lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
    cup (9, 1);
    pcdhl (1);
    printf ("lqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
    cup (10, 1);
    pcdhl (0);
    printf ("x                                   x");
    cup (11, 1);
    pcdhl (1);
    printf ("x                                   x");
    cup (12, 1);
    pcdhl (0);
    printf ("x                                   x");
    cup (13, 1);
    pcdhl (1);
    printf ("x                                   x");
    cup (14, 1);
    pcdhl (0);
    printf ("x                                   x");
    cup (15, 1);
    pcdhl (1);
    printf ("x                                   x");
    cup (16, 1);
    pcdhl (0);
    printf ("mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
    cup (17, 1);
    pcdhl (1);
    printf ("mqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
    scs (0, 'B');
    sgr ("1;5");
    cup (12, 3);
    printf ("The mad programmer strikes again!");
    cup (13, 3);
    printf ("The mad programmer strikes again!");
    sgr ("0");
    cup (22, 1);
    printf ("Another test pattern...  a frame with blinking bold text,\n");
    getlin ("all in double-height double-width size.  Push <RETURN>");

 /* the FACIT 4431 has a bug when you scroll    */
							/* down doublesized text thru the bottom line. */

    pcstbm (8, 24);
    cup (8, 1);						/* We are in absolute origin mode */
    for (i = 1; i <= 12; i++)
	ri ();
    cup (1, 1);
    getlin ("Exactly half of the box should remain. Push <RETURN>");
}

tst_reports () {
							/* Test of: <ENQ> (AnswerBack Message) SM RM  (Set/Reset Mode)
							   - LineFeed / Newline - Cursor Keys - Keypad */
    ;
}

tst_vt52 () {
							/* Test of: SM RM  (Set/Reset Mode) - ANSI / VT52 */
    ;
}

esc (s) char   *s; {
    printf ("%c%s", 27, s);
}

esc2 (s1, s2) char  s1, s2; {
    printf ("%c%s%s", 27, s1, s2);
}

brcstr (ps, c) char *ps, c; {
    printf ("%c[%s%c", 27, ps, c);
}

brc (pn, c) int pn;
char    c; {
    char   *str, *ptr;
    ptr = str = "[    ";
    ptr++;
    if (pn > 99) {
	*ptr++ = (pn / 100) + 48;
	pn % = 100;
    }
    if (pn > 9) {
	*ptr++ = (pn / 10) + 48;
	pn % = 10;
    }
    *ptr++ = pn + 48;
    *ptr++ = c;
    *ptr = '\0';
    printf ("%c%s", 27, str);
}

brc2 (pn1, pn2, c) int  pn1, pn2;
char    c; {
    char   *str, *ptr;
    ptr = str = "[        ";
    ptr++;
    if (pn1 > 99) {
	*ptr++ = (pn1 / 100) + 48;
	pn1 % = 100;
    }
    if (pn1 > 9) {
	*ptr++ = (pn1 / 10) + 48;
	pn1 % = 10;
    }
    *ptr++ = pn1 + 48;
    *ptr++ = ';';
    if (pn2 > 99) {
	*ptr++ = (pn2 / 100) + 48;
	pn2 % = 100;
    }
    if (pn2 > 9) {
	*ptr++ = (pn2 / 10) + 48;
	pn2 % = 10;
    }
    *ptr++ = pn2 + 48;
    *ptr++ = c;
    *ptr = '\0';
    printf ("%c%s", 27, str);
}

char   *readto (c) char c; {

 /* read characters one-by-one without echo until c is read. */

 /* NOT YET IMPLEMENTED! */

    return ("Not Yet Implemented!!");
}

cub (pn) int    pn; {					/* Cursor Backward */
    brc (pn, 'D');
}
cud (pn) int    pn; {					/* Cursor Down */
    brc (pn, 'B');
}
cuf (pn) int    pn; {					/* Cursor Forward */
    brc (pn, 'C');
}
cup (pn1, pn2) int  pn1, pn2; {				/* Cursor Position */
    brc2 (pn1, pn2, 'H');
}
cuu (pn) int    pn; {					/* Cursor Up */
    brc (pn, 'A');
}
da (pn) {						/* Device Attributes */
    char   *str;
    brc (pn, 'c');
    str = readto ('c');
}
pcaln () {						/* Screen Alignment Display */
    esc ("#8");
}
pcdhl (lower) int   lower; {				/* Double Height Line (also double width) */
    if (lower)
	esc ("#4");
    else
	esc ("#3");
}
pcdwl () {						/* Double Wide Line */
    esc ("#6");;
}
pcid () {						/* Identify Terminal (obsolete) */
    esc ("Z");
}
pckpam () {						/* Keypad Application Mode */
    esc ("=");;
}
pcpnm () {						/* Keypad Numeric Mode */
    esc (">");
}
pcll (ps) char *ps; {					/* Load LEDs */
    brcstr (ps, 'q');
}
pcrc () {						/* Restore Cursor */
    esc ("8");
}
pcreqtparm (pn) {					/* Request Terminal Parameters */
    char   *str;
    brc (pn, 'x');
    str = readto ('x');
}
pcsc () {						/* Save Cursor */
    esc ("7");;
}
pcstbm (pn1, pn2) int   pn1, pn2; {			/* Set Top and Bottom Margins */
    brc2 (pn1, pn2, 'r');
}
pcswl () {						/* Single With Line */
    esc ("#5");
}
pctst (pn) int  pn; {					/* Invoke Confidence Test */
    brc2 (2, pn, 'y');
}
dsr (pn) {						/* Device Status Report */
    char   *str;
    brc (pn, 'n');
    str = readto ('n');
}
ed (pn) int pn; {					/* Erase in Display */
    brc (pn, 'J');
}
el (pn) int pn; {					/* Erase in Line */
    brc (pn, 'K');
}
hts () {						/* Horizontal Tabulation Set */
    esc ("H");
}
hvp (pn1, pn2) int  pn1, pn2; {				/* Horizontal and Vertical Position */
    brc2 (pn1, pn2, 'f');
}
ind () {						/* Index */
    esc ("D");
}
mc (pn) int pn; {					/* Media Copy (Not VT100?) */
    brc (pn, 'i');
}
nel () {						/* Next Line */
    esc ("E");
}
ri () {							/* Reverse Index */
    esc ("M");
}
ris () {						/* Reset to Initial State */
    esc ("c");
}
rm (ps) char   *ps; {					/* Reset Mode */
    brcstr (ps, 'l');
}
scs (g, c) int  g;
char    c; {						/* Select character Set */
    printf ("%c%c%c%c%c%c%c", 27, g ? ')' : '(', c,
	    27, g ? '(' : ')', 'B',
	    g ? 14 : 15);
}
sgr (ps) char  *ps; {					/* Select Graphic Rendition */
    brcstr (ps, 'm');
}
sm (ps) char   *ps; {					/* Set Mode */
    brcstr (ps, 'h');
}
tbc (pn) int    pn; {					/* Tabulation Clear */
    brc (pn, 'g');
}

inittype () {

    int     ok;
    char   *answer;

    answer = "vt102";
    goto frotz;						/* Bypass this code for now. */

    printf ("Compatibility test of terminal with respect to VT100\n\n\n");
    do {
	answer = (getlin ("What is your terminal type (vt100, vt102)? "));
	ok = (strcmp (answer, "vt100") == 0) ||
	     (strcmp (answer, "vt102") == 0);
	if (!ok)
	    printf ("Type 'vt100' or 'vt102'!\n");
    }
    while (!ok);

frotz: 							/* bypass hack		 */

 /* Set up my personal prejudices	 */

    esc ("<");						/* Enter ANSI mode (if in VT52 mode)	 */
    rm ("?3");						/* 80 col mode		 */
    rm ("?5");						/* Black background	 */
    sm ("?7");						/* Wrap around on	 */

    return ((strcmp (answer, "vt100") == 0) ? vt100 : vt102);
}

bye () {
							/* Force my personal prejudices upon the poor luser	 */

    rm ("?3");						/* 80 col mode		 */
    rm ("?5");						/* Black background	 */
    rm ("?6");						/* Absolute origin mode	 */
    sm ("?7");						/* Wrap around on	 */
    pcstbm (0, 0);					/* No scroll region	 */

 /* Say goodbye */

    ed (2);
    cup (12, 30);
    printf ("That's all, folks!\n\n");
    exit (3);
}


