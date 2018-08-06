
/********************************************************************

    TEMPLATE GENERATOR for custom keyboard templates.  Helps learning
        curve for common functions.

*/



#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <ctype.h>
#include <graphics.h>
#include <string.h>
#include <stdlib.h>
#include <mem.h>

#define TRUE      1
#define FALSE     0
#define NUM_LINES 4
#define NUM_KEYS  12
#define NUM_CHARS 11
#define DC2       0x12
#define SI        0x0f
#define ESC		  0x1b

void fprnt(char *text, int auto_lf);
char put_out(char character);
void stretch( char *src, char *dst);

char vert_spacing[] = {ESC,'A',0x8,0};

char title[80];

struct {                        /* structure for the top half lines */
    char            s1[2];
    char            esc;
    char            mode;
    int             len;
    unsigned char   v1[90];
    unsigned char   v2[90];
    unsigned char   v3[90];
    unsigned char   v4[90];
    unsigned char   v5[130];
    unsigned char   v6[90];
    unsigned char   v7[90];
    unsigned char   v8[50];
} top = {' ',' ',ESC,'L',720};

struct {                        /* stucture for the bottom half lines */
    char            s1[2];
    char            esc;
    char            mode;
    int             len;
    unsigned char   v1[8];
    unsigned char   v2[90];
    unsigned char   v3[90];
    unsigned char   v4[130];
    unsigned char   v5[90];
    unsigned char   v6[90];
    unsigned char   v7[90];
    unsigned char   v8[90];
    unsigned char   v9[40];
    unsigned char   v10[2];
} bottom = {' ',' ',ESC,'L',720};

char func[NUM_LINES][NUM_KEYS][NUM_CHARS+2];    /* array to hold function key data */
char *fptr;
char buffer[128];
char t0[200];
char t1[200];
char t2[200];
char t3[200];

int f, l, j, flag;
char c_cnt[3] = {0,0,0};
char f_cnt[12] = {0,0,0, 0,0,0, 0,0,0, 0,0,0};

char col[3][15][40];
char lines[15][120];

int len, col_line;
char fname[40], mode, mode_num, ans;
char buffer[128];
FILE *fs;


void main()
{
    register 	int i;
    int 		pr_flag, ok_flag = TRUE, tlen;

	fptr = &func[0][0][0];      /* fill function array with spaces */
    for(i = 0; i < sizeof(func); i++) *fptr++ = ' ';

	fptr = &col[0][0][0];
	for(i = 0; i < sizeof(col); i++) *fptr++ = ' ';

	while(ok_flag) {
		printf("\n\nDo you want the input from the KEYBOARD or a FILE? [K/F] ");
		printf("\nNote: keyboard input onlys allows function key data ");
		ans = tolower(getch());
		if(ans == 'f' || ans == 'k')
			ok_flag = FALSE;
	}

	if(ans == 'f') {

	    printf("\n");
		ok_flag = TRUE;
	    while(ok_flag) {
	        printf("Enter filename: ");
	        gets(fname);
   	        if((fs = fopen(fname, "r")) != NULL)
	            ok_flag = 0;
	    }

	    mode = 0;
	    while((fgets(buffer, 128, fs)) != NULL) {
	        if(buffer[0] == ';') continue;
	        if(buffer[0] == '*') continue;
	        if(buffer[0] == '\n') continue;

	        if(buffer[0] == '.') {
	            mode = tolower(buffer[1]);
	            if(mode == 'f' || mode == 'c')
	                mode_num = atoi(&buffer[2]) - 1;
	            continue;
	        }
	        len = strlen(buffer) - 1;             /* eleminate the LF */
	        if(!(mode == 0 || mode == 'c' || mode == 'f' || mode == 't')) {
	            printf("\n******* ERROR in input file (mode) *******");
	            continue;
	        }

	        if(mode == 't') {
	            if(len > 50) {
	                printf("\n******* ERROR in input file (title length) *******");
	            }
				tlen = min(len,50);
	            strncpy(title, buffer, tlen);
	            mode = 0;
	            continue;
	        }
	        if(mode == 'c') {
	            if(mode_num > 3) {
	                printf("\n******* ERROR in input file (column > 3) *******");
	                continue;
	            }
	            if(c_cnt[0] > 15 || c_cnt[1] > 15 || c_cnt[2] > 15) {
	                printf("\n******* ERROR in input file (column line count > 15) *******");
	                continue;
	            }
	            if(len > 35) {
	                printf("\n******* ERROR in input file (column text length > 35) *******");
	            }
	            if(buffer[0] == '/' && buffer[1] == '/')
    	            c_cnt[mode_num]++;
    	        else
    	            strncpy(&col[mode_num][c_cnt[mode_num]++][0], buffer, min(len, 35));
    	        continue;
    	    }
    	    if(mode == 'f') {
    	        if(mode_num > 11) {
    	            printf("\n******* ERROR in input file (function key number > 12) *******");
    	            continue;
    	        }
    	        for(i = 0; i < 12; i++) {
    	            if(f_cnt[i] > 4 ) {
    	                printf("\n******* ERROR in input file (line count for key %d > 4) *******",i);
    	                continue;
    	            }
    	        }
    	        if(len > 11) {
    	            printf("\n******* ERROR in input file (function key text length) *******");
    	        }
    	        if(buffer[0] == '/' && buffer[1] == '/')
    	            f_cnt[mode_num]++;
    	        else
    	            strncpy(&func[f_cnt[mode_num]++][mode_num][0], buffer, min(len, 11));
    	        continue;
	        }

	    }
	    col_line = max(c_cnt[0], max(c_cnt[1], c_cnt[2]));

	} else {
		ok_flag = TRUE;
	    printf("\n");               /* request the optional title */
	    while(ok_flag) {
	        printf("\nEnter title (50 chars max): ");
	        gets(title);
	        tlen = strlen(title);
	        if(tlen <= 50) ok_flag = 0;
	        else printf("%c%c", 0x07, 0x07);
	    }

	    for(f = 0; f < NUM_KEYS;) {     /* get the data for all 12 function keys */
	        printf("\n\n");
	        for(l = 0; l < NUM_LINES;) {
	            for(i = 0; i < NUM_CHARS; i++)
	                func[l][f][i] = ' ';
	            flag = TRUE;
	            printf("\Enter F%d line %d ( 11 chars max): ", f+1, l+1);
	            gets(buffer);
	            if(buffer[0] == '\0') break;
	            if(strlen(buffer) > NUM_CHARS) {
	                printf("..... Too many characters .....\n");
	                flag = FALSE;
	                break;
	            }
	            strncpy(&func[l][f][0], buffer, strlen(buffer));
	            l++;
	        }
	        if(flag) {
	            printf("\nIs this correct? ");
	            if(tolower(getche()) == 'y') f++;
	        }
	    }
    }
    
    printf("\n\nPrepare printer, hit any key ");
    getch();

    for(i = 0; i < 29; i++) putch(0x08);
    printf(".......... Printing ..........");

    for(i = 0; i < 2; i++) put_out('\n');   /* skip some top of form      */


	for(i = 0; i < 15; i++)
		for(j = 0; j < 3; j++)
			movmem(&col[j][i][0], &lines[i][40*j], 40);

	put_out(ESC); put_out('E');		/* emphasized */
	put_out(ESC); put_out('M');		/* pica       */
	put_out(ESC); put_out('2');     /* 6 LPI      */

	for(i = 0; i < col_line; i++) {
		for(j = 0; j < 11; j++) put_out(' ');
		for(j = 0; j < 63; j++) put_out(lines[i][j]);
		put_out('\r'); put_out('\n');
	}

	put_out(ESC); put_out('P');			/* go to 10 CPI */
	put_out(DC2);							/* reset if in condensed print */

    for(i = 0; i < 1; i++) put_out('\n');   /* skip some space      */
    put_out('\r');

    stretch(&func[0][0][0], t0);            /* fill in the extra spaces   */
    stretch(&func[1][0][0], t1);            /*  for the gaps at F4 and F8 */
    stretch(&func[2][0][0], t2);
    stretch(&func[3][0][0], t3);


    put_out(ESC); put_out('E');            /* emphasized characters      */

    fputs(vert_spacing, stdprn);            /* set line spacing           */

    for(i = 0; i < 720; i++) top.v1[i] = 0xc0;
    top.v1[0] = top.v1[1] = 0xff;
    fprnt(&top.s1[0], 1);                           /* print upper line   */

    for(i = 2; i < 720; i++) top.v1[i] = 0;
    fprnt(&top.s1[0], 1);                           /* print blank line   */

    top.v2[0] = top.v2[1] = top.v3[0] = top.v3[1] =
    top.v4[0] = top.v4[1] = top.v5[0] = top.v5[1] =
    top.v6[0] = top.v6[1] = top.v7[0] = top.v7[1] =
    top.v8[0] = top.v8[1] = 0xff;

    fprnt(&top.s1[0], 0);                           /* vertical serriations  */

    put_out(SI);                                    /* print full top line */
    for(i = 0; i < 17; i++) put_out(' ');           /* function key text   */
    for(i = 0; i < 89; i++) put_out(t0[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&top.s1[0], 0);                           /* vertical serriations  */

    put_out(SI);
    for(i = 0; i < 17; i++) put_out(' ');
    for(i = 0; i < 89; i++) put_out(t1[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&top.s1[0], 0);                           /* vertical serriations  */

    put_out(SI);
    for(i = 0; i < 17; i++) put_out(' ');
    for(i = 0; i < 89; i++) put_out(t2[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&top.s1[0], 0);                           /* vertical serriations  */

    put_out(SI);
    for(i = 0; i < 17; i++) put_out(' ');
    for(i = 0; i < 89; i++) put_out(t3[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    for(i = 92; i < 720; i++) top.v1[i] = 0xc0;
    fprnt(&top.s1[0], 1);                           /* top horizontal inside line  */


    for(i = 92; i < 720; i++) top.v1[i] = 0;

    for(i = 0; i < 5; i++) fprnt(&top.s1[0], 1);    /* left side  --- 5 times */

    for(i = 92; i < 720; i++) top.v1[i] = 3;
    fprnt(&top.s1[0], 1);                           /* bottom horizontal inside line  */

    for(i = 2; i < 720; i++) top.v1[i] = 0;
    for(i = 0; i < 2; i++) fprnt(&top.s1[0], 1);    /* left edge and blank line 6 times */
    fprnt(&top.s1[0], 0);

    if(tlen > 0) {
        put_out(0x0e);
        for(i = 0; i < 28 - (tlen/2); i++) put_out(' ');
        for(i = 0; i < (tlen/2) + 3; i++) {
            if(title[i] == '\0') break;
            put_out(title[i]);
        }
    }
    put_out('\r');
    put_out('\n');


    for(i = 0; i < 3; i++) fprnt(&top.s1[0], 1);    /* left edge and blank line 6 times */

    for(i = 0; i < 720; i++) top.v1[i] = 0xc0;
    fprnt(&top.s1[0], 1);                           /* bottom line  */


    /* print the bottom half   */

    for(i = 0; i < 4; i++) put_out('\n');          /* skip some form */
    put_out('\r');

	put_out(ESC); put_out('M');		/* elite pitch */
	put_out(ESC); put_out('2');        /* 6 LPI */
	for(i = 0; i < col_line; i++) {
		for(j = 0; j < 3; j++) put_out(' ');
		for(j = 49; j < 120; j++) put_out(lines[i][j]);
		put_out('\r'); put_out('\n');
	}

	put_out(ESC); put_out('P');			/* go to 10 CPI */
    for(i = 0; i < 1; i++) put_out('\n');   /* skip some space      */
    put_out('\r');

    fputs(vert_spacing, stdprn);

    for(i = 0; i < 718; i++) bottom.v1[i] = 0xc0;
    bottom.v10[0] = bottom.v10[1] = 0xff;
    fprnt(&bottom.s1[0], 1);                        /* top line  */

    for(i = 0; i < 718; i++) bottom.v1[i] = 0;
    fprnt(&bottom.s1[0], 1);                        /* blank line  */

    bottom.v2[0] = bottom.v2[1] = bottom.v3[0] = bottom.v3[1] =
    bottom.v4[0] = bottom.v4[1] = bottom.v5[0] = bottom.v5[1] =
    bottom.v6[0] = bottom.v6[1] = bottom.v7[0] = bottom.v7[1] =
    bottom.v8[0] = bottom.v8[1] = 0xff;

    fprnt(&bottom.s1[0], 0);                        /* vertical serriations */

    put_out(SI);
    for(i = 0; i < 5; i++) put_out(' ');
    for(i = 70; i < 186; i++) put_out(t0[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&bottom.s1[0], 0);                        /* vertical serriations */

    put_out(SI);
    for(i = 0; i < 5; i++) put_out(' ');
    for(i = 70; i < 186; i++) put_out(t1[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&bottom.s1[0], 0);                        /* vertical serriations */

    put_out(SI);
    for(i = 0; i < 5; i++) put_out(' ');
    for(i = 70; i < 186; i++) put_out(t2[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    fprnt(&bottom.s1[0], 0);                        /* vertical serriations */

    put_out(SI);
    for(i = 0; i < 5; i++) put_out(' ');
    for(i = 70; i < 186; i++) put_out(t3[i]);
    put_out(DC2);
    put_out('\r');
    put_out('\n');

    for(i = 0; i < 678; i++) bottom.v1[i] = 0xc0;
    bottom.v9[0] = bottom.v9[1] = 0xff;
    fprnt(&bottom.s1[0], 1);                        /* top inside line  */


    for(i = 0; i < 678; i++) bottom.v1[i] = 0;
    for(i = 0; i < 5; i++) fprnt(&bottom.s1[0], 1); /* right side  */

    for(i = 0; i < 678; i++) bottom.v1[i] = 3;
    fprnt(&bottom.s1[0], 1);                        /* bottom inside line  */

    for(i = 0; i < 718; i++) bottom.v1[i] = 0;
    for(i = 0; i < 2; i++) fprnt(&bottom.s1[0], 1); /* right side  */
    fprnt(&bottom.s1[0], 0);


    if(tlen > 0) {
        put_out(0x0e);
        put_out(' ');
        for( i = (tlen/2) -3; title[i] != '\0'; i++)
            put_out(title[i]);
    }
    put_out('\r');
    put_out('\n');


    for(i = 0; i < 3; i++) fprnt(&bottom.s1[0], 1); /* right side  */

    for(i = 0; i < 720; i++) bottom.v1[i] = 0xc0;
    fprnt(&bottom.s1[0], 1);                        /* bottom line  */

    printf("\n\n");

    put_out(0xc);
    fflush(stdprn);

}

/* not used - but someday ??? */

char status(void)
{
    union REGS regs;

    regs.h.ah = 2;
    regs.x.dx = 0;
    int86(0x17, &regs, &regs);
    return(regs.h.ah & 0x80);
}

/* out put to the printer */

char put_out(char character)
{
    union REGS regs;

    regs.h.ah = 0;
    regs.h.al = character;
    regs.x.dx = 0;
    int86(0x17, &regs, &regs);
    return(regs.h.ah);
}

void fprnt(char *text, int auto_lf) /* print a line with optional CR */
{
    int i;

    for( i = 0; i < 726; i++)
        put_out(*text++);
    if(auto_lf) {
        put_out('\r');
        put_out('\n');
    } else put_out('\r');
}

/* take data from thefunction key array and put it in the Tx array */
void stretch( char *src, char *dst)
{
    register int i;

    for(i = 0; i < 4*13; i++)
        *dst++ = *src++;
    for(i = 0; i < 6; i++)      /* 6 extra spaces for F4 */
        *dst++ = ' ';

    for(i = 0; i < 4*13; i++)
        *dst++ = *src++;
    for(i = 0; i < 5; i++)      /* 5 extra spaces for F8 */
        *dst++ = ' ';           /* I think my printer is non-linear or ??? */
    for(i = 0; i < 4*13; i++)
        *dst++ = *src++;
}

