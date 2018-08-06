/*		EDIT.C  - Screen Editor
		by Bill Kinnersley
		MSU Physics Dept
		January 1982
EDIT has many of the conveniences of WordStar, but is a much smaller and
faster program.  The program is presently limited to editing files of less
than 44K total length, and lines shorter than 80 characters. */

#define ESC		0x1b		/* escape */
#define BELL		0x07
#define DC1		0x11		/* dc1 = x-on */
#define DC3		0x13		/* dc3 = x-off */
#define EOFCHAR		0x1a		/* end-of-file marker */

#include stdio.h

char *topmem;			/* pointer to top of memory */
char *topfile;			/* pointer to top of file */
char *botfile;			/* pointer to bottom of file */
char *topscr;			/* pointer to top of screen */
char *botscr;			/* pointer to bottom of screen */
char *cursor;			/* pointer to current cursor position in file */
char *null;			/* null pointer*/
char *mark;			/* temporary pointer */
int lines;			/* line # where botscr is (1 <= lines <=25) */
int csr_col;			/* current cursor column # */
int want_col;			/* cursor column desired */
int csr_line;			/* current cursor line # */
int next_char;			/* next input char, input during screen output*/
int ch;				/* current character input from keyboard */
int flag;			/* true if cursor==topscr */
int itop;			/* topfile as an integer */
int ibot;			/* botfile as an integer */
int mar_left;			/* left margin */
int mar_right;			/* right margin */
char bufin[80];
char fid[28];			/* name of file to edit */
char backup[28];		/* name of backup file */
int file;			/* file descriptor # used by C */
int new;			/* true if file was new */
int i;				/* number of records written to disk */
main(argc,argv)
int argc;			/* number of arguments in command line */
int *argv[];			/* pointers to those arguments */
{
	topmem = CCAVAIL() - 4096;	/* get size of available memory */
	cursor = CCALLOC(topmem);
	*cursor++ = '\n';		/* dummy new/line */
	botfile = topfile = botscr = topscr = cursor;
	topmem = botfile + topmem;	/* point to top of available memory */
	null = "";
	next_char = 0;
	mar_left = 0; mar_right = 79;	/* set margins */
	term_init();
	if (argc == 1)		/* no command tail */
	{	fputs("\nFILE? : ",stderr);	/* ask for fid */
		gets(fid);
	}
	else strcpy(fid,argv[1]);	/* fid from command line */
	fid[26] = 0;			/* trim to 26 chars */
	new = - swapin(fid,topfile);		/* try to read in */
	clear_screen();
	csr_line = csr_col = want_col = 0;
	if (new)
	{	fputs("NEW FILE\n\n",stderr);	/* must be new file*/
		lines = 3;
		*botfile = EOFCHAR;
		csr_line = 2;
	}
	else
	{	while (*botfile != EOFCHAR) botfile++;	/* look for botfile*/
		lines = 1;
		setbot();
		putscr(topfile);
		home();
	}
 
	while(TRUE)
	{	ch = get_conin();	/* wait for console input */
		if (ch == ESC ) ch = escape();	/* convert escape sequence */
		if (((ch >= ' ') && (ch < 127)) || (ch == '\t') || (ch == '\r'))
			ins_character();
		else if (special())	/* process special characters */
			return;		/* quit */
	}
}
ins_character()
/* insert character c at current cursor position */
{	disable_keyboard();
	e_eol();
	ibot = ++botfile; itop = ++cursor;
	++botscr;
	movmem(cursor-1,cursor,ibot-itop+1);
	if (ch == '\r') ch = '\n';	/* convert CR to NEW/LINE */
	*(cursor-1) = ch;
	if (ch == '\n')
		{where();
		csr_col = want_col = 0;
		if (csr_line == 23)
			{botscr = cursor;
			lines = 25;
			printbot();
			}
		else
			{putch('\n');
			setbot();
			ins_line();
			putline(cursor);
			csr_line++;
			put_cursor(csr_line,csr_col);
			}
		}
	else
		{putch(ch);
		if (ch != '\t') csr_col++;
		else csr_col = ((csr_col>>3)+1)<<3;
		want_col = csr_col;
		putline(cursor);
		position();
		}
	enable_keyboard();
}
special()
/* procedure to process special characters */
{		switch (ch) {
/* cursor right */	case 'D'-64 :if (cursor == botfile) break;
				else if ((ch=*cursor++)=='\n')
					{printbot(); csr_col=0;}
				else if (ch=='\t')
					{putch('\t');
					csr_col = ((csr_col>>3)+1)<<3;}
				else {cup_right(); csr_col++;}
				want_col = csr_col;
				break;
/* cursor left */	case 'S'-64 :
			case 'H'-64 : flag = (cursor==topscr);
				if (cursor==topfile) break;
				else if ((ch=*--cursor)=='\n')
					{reverse_index();
					if (flag)
						{topscr=startline(cursor);
						setbot();
						putch('\r');
						putline(topscr);}
					position();
					}
				else if (ch=='\t') position();
				else {putch('\b'); csr_col--;}
				want_col = csr_col;
 				break;
/* cursor up */		case 'E'-64 :
				if ((mark=startline(cursor))==topfile) break;
				cursor = startline(mark-1);
				disable_keyboard();
				reverse_index();
				where();
				if (mark==topscr) {topscr = cursor;
						setbot();
						cup_home();
						putline(topscr);}
				set_cursor();
				enable_keyboard();
				break;
/* cursor down */	case 'X'-64 :
			case 'J'-64 : if ((mark = next(cursor))==null) break;
				cursor = mark;
				printbot();
				where();
				set_cursor();
				break;
/* up screenful */	case 'R'-64 : if (topscr==topfile) break;
				disable_keyboard();
				where();
				botscr = topscr;
				settop();
				newscr();
				enable_keyboard();
				break;
/* down screenful */	case 'C'-64 : if (botscr==botfile) break;
				disable_keyboard();
				topscr = botscr;
				setbot();
				newscr();
				enable_keyboard();
				break;
/* top file */		case 'T'-64 : if (topscr==topfile) break;
				topscr = topfile;
				setbot();
				newscr();
				break;
/* bottom file */	case 'B'-64 : if (botscr==botfile) break;
				botscr = botfile;
				settop();
				lines = 24;
				newscr();
				cursor=botscr;
				csr_line=23; csr_col=0;
				put_cursor(csr_line,csr_col);
				break;
/* scroll down */	case 'Z'-64 : if (botscr==botfile) break;
				where();
				put_cursor(23,0);
				bdos(6,'\n');
				botscr=next(botscr);
				settop();
				putline(startline(botscr-1));
				if (cursor < topscr) cursor=next(cursor);
				if (csr_line > 0) csr_line--;
				set_cursor();
				break;
/* scroll up */		case 'W'-64 : if (topscr==topfile) break;
				cup_down();	/* move cursor down */
				where();	/* save current cursor position */
				cup_home();
				reverse_index();
				topscr=startline(topscr-1);
				setbot();
				putline(topscr);
				if (cursor >= botscr) cursor=startline(botscr-1);
				set_cursor();
				break;
/* find */		case 'F'-64 : clear_screen(); con_flush();
				fputs("FIND? :",stderr);
				gets(bufin);
				cursor = topfile;
/* refind */		case 'L'-64 : for (mark=cursor; mark!=botfile; mark++)
					if (*mark == bufin[0])
						if (comp())
							{topscr=startline(mark);
							setbot();
							newscr();
							cursor=mark;
							position();
							break;}
				if (mark==botfile) {
					putch(BELL);
					/* goto top; */
					if (cursor == topfile) {
						topscr=startline(cursor);
						setbot();
						newscr();
						position();
					}
				}
				break;
/* keep */		case 'K'-64 : clear_screen(); con_flush();
				fputs("Saving ",stderr);
				fputs(fid,stderr);
				fputs(".\n",stderr);
				if (!new) {strcpy(backup,fid);
					for (ch=0; ch<15; ch++)
						if (backup[ch] == '.' || backup[ch] == ' ')
							backup[ch]=0;
					strcat(backup,".BACKUP");
					unlink(backup); rename(fid,backup);
					}
				if ((file=creat(fid))==NULL) {
					fputs("Can't open ",stderr);
					fputs(fid,stderr);
					fputs(".\n",stderr);
					return;
				}
				ibot = botfile;	/* change pointers */
				itop = topfile;	/* to integers  */
				i = (ibot-itop)/128+1;
				if (write(file,topfile,i)!=i)
 					fputs("WRITE ERROR\nDisk full?",stderr);
				term_reset();
				exit(0);
/* quit */		case 'Q'-64 : clear_screen(); con_flush();
				fputs("QUIT? ",stderr);
				if((ch=getchar())=='Y'||ch=='y'||ch=='Y'-64)
					{fputs("\nEdit Abandoned\n",stderr);
					term_reset(); return TRUE;}
				clear_screen();
				putscr(cursor=topscr);
				home();
				break;
/* delete character */	case 'G'-64 : if (cursor == botfile) break;
				disable_keyboard();
				ibot = --botfile; itop = cursor;
				--botscr;
				ch = *cursor;
				movmem(cursor+1,cursor,ibot-itop);
				if (ch == '\n')
					{position();
					 where();
					 setbot();
					 putline(cursor);
					 putch('\n');
					 delete_line();
					 put_cursor(csr_line,csr_col);
					}
				else {putline(cursor);
					position();}
				enable_keyboard();
				break;
/* delete */		case 0x7f : if (cursor == topfile) break;
				disable_keyboard();
				flag = (cursor == topscr);
				ibot = --botfile; itop = --cursor;
				--botscr;
				ch = *cursor;
				movmem(cursor+1,cursor,ibot-itop+1);
				if (flag)
					{putline(topscr=startline(cursor));
					position();
					}
				else if (ch == '\n')
					{cup_up();
					 position();
					 where();
					 setbot();
					 putline(cursor);
					 putch('\n');
					 delete_line();
					 put_cursor(csr_line,csr_col);
					}
				else {if (ch == '\t') position();
						else putch('\b');
					putline(cursor);
					position();}
				enable_keyboard();
				break;
			default: putch(BELL);	/* invalid character */
	}
	return FALSE;
}
comp()
{int i;
	for (i=0; bufin[i]!=0; i++) if (bufin[i] != mark[i]) return FALSE;
	mark = mark + i;
	return TRUE;
}
newscr()
/* puts a new screen */
{	clear_screen();
	putscr(cursor=topscr);
	home();
}
startline(start)
char *start;
/* returns pointer to first character in current line */
{char *mark;
	for(mark=start-1; mark!=topfile; mark--)
		if (*mark=='\n') return ++mark;
	return topfile;
}
next(start)
char *start;
/* returns pointer to first character in next line */
{char *mark;
	for(mark=start; mark!=botfile; mark++)
		if (*mark=='\n') return ++mark;
	return null;
}
position()
/* positions screen cursor in the current line */
{char *mark,i;
	csr_col=0;
	for (mark=startline(cursor); mark!=cursor; mark++)
		if (*mark=='\t')
			csr_col=((csr_col>>3)+1)<<3;/* count up column position*/
			else csr_col++;
	put_cursor(26,csr_col);	/* dummy for H19 */
	want_col = csr_col;
}
set_cursor()
/* positions file cursor to position screen cursor on desired column */
{int tab_col;
	csr_col=0;
	for (cursor=startline(cursor); *cursor!='\n'; cursor++) {
		if (cursor == botfile) break;
		if (csr_col == want_col) break;
		if (*cursor != '\t') csr_col++;
		else {
			tab_col=((csr_col>>3)+1)<<3;
			if (want_col < tab_col) break;
			csr_col = tab_col;
		}
	}
	put_cursor(csr_line,csr_col);
}
putline(start)
char *start;
/* outputs from "start" to end of current line;
   returns pointer to first char in next line */
{char *mark; int i;
	e_eol();
	for (mark=start; mark!=botfile; mark++)
		if (*mark=='\n') return mark+1;
		else putch(*mark);
	return botfile;
}
printbot()
/* prints a line at the bottom of screen and scrolls up */
{	putch('\n');
	if (cursor == botscr)
		{if (lines==25) while (*topscr++ != '\n');
		if (botscr == botfile);
		botscr = putline(cursor);
		putch('\r');
		}
}
setbot()
/* measures down 24 lines from topscr to set botscr */
{	lines=0;
	for (botscr=topscr; botscr!=null; botscr=next(botscr))
		 if (++lines==25) return;
	botscr=botfile;
}
putscr(start)
char *start;
/* outputs from "start" to botscr */
{char *mark;
	if (start==botscr) return;
	e_eol();
	for (mark=start; mark!=(botscr-1); mark++) {
		if (*mark == '\n') {
			e_eol();		/* erase to end of line */
			bdos(6,'\r');		/* output carriage return */
			if (mark != (botscr-1)) bdos(6,'\n');
		}
		else putch(*mark);
		if (!next_char) {
			next_char = bdos(6,-1);	/* look-ahead for dc3 */
			if (next_char == DC3) {
				while (!(next_char=bdos(6,-1)));
				if (next_char == DC1) next_char = 0;
			}
		}
	}
	if (lines<25) putch(*mark);
	e_eos();
}
home()
{	cup_home();
	csr_line = csr_col = want_col = 0;
}
settop()
/* measures up 23 or 24 lines from "botscr" */
{int i,j;
	i = j = (botscr == botfile);
	for (topscr=botscr; topscr!=topfile; topscr=startline(topscr-1))
			if (++i==25) break;
	lines = i - j;
	setbot();
}
reverse_index()
/* move cursor up one line, scrolling screen down if at top */
{	putch(ESC); putch('M');
}
ins_line()
/* insert new blank line on screen at current position */
{	putch(ESC); putch('['); putch('L');
}
delete_line()
/* delete the current line, scroll bottom of screen, output bottom line */
{	putch(ESC); putch('['); putch('M');
	if (lines<25) return;
	putch(ESC); putch('['); putch('2'); putch('4'); putch(';');
	putch('1'); putch('H');
	putline(startline(botscr-1));
}
putch(c)
int c;
/* replacement for library putc (will not swallow characters) */
{	if (c == '\n') bdos(6,'\r');
	bdos(6,c);
}
get_conin()
{int c;
	c = next_char;
	if (c) next_char = 0;
	else while (!(c=bdos(6,-1)));;
	return c;
}
con_flush()
{	while(bdos(6,-1));		/* flush type-ahead */
}
escape()
/* procedure to process escape sequences */
{int c;
	c = get_conin();	/* get next character */
	if (c == '[') {
		c = get_conin();	/* get next character */
		switch(c) {
/* cursor up */		case 'A' : c = 'E'-64; break;
/* cursor down */	case 'B' : c = 'X'-64; break;
/* cursor forward */	case 'C' : c = 'D'-64; break;
/* cursor backword */	case 'D' : c = 'S'-64; break;
/* home */		case 'H' : c = 'T'-64; break;
/* delete character */	case 'P' : c = 'G'-64; break;
			default : c = 0; putch(BELL);
		}
	}
	else if (c == 'O') {	/* keypad or function key */
		c = get_conin();	/* get next character */
		switch(c) {
/* blue */		case 'P' : c = 'K'-64; break;
/* red */		case 'Q' : c = 'Q'-64; break;
/* f1 */		case 'S' : c = 'F'-64; break;
/* f2 */		case 'T' : c = 'L'-64; break;

/* shift down */	case 'r' : c = 'C'-64; break;
/* shift home */	case 'u' : c = 'B'-64; break;
/* shift up */		case 'x' : c = 'R'-64; break;
			default : c = 0; putch(BELL);
		}
	}
	else {c = 0; putch(BELL);}
	return c;	/* return modified character */
}
where()
{	int c;
	putch(ESC); putch('['); putch('6'); putch('n');/* get cursor position */
	while (get_conin() != ESC);
	while (get_conin() != '[');
	csr_line = csr_col = 0;
	while ((c = get_conin()) != ';') csr_line = csr_line * 10 + c - '0';
	while ((c = get_conin()) != 'R') csr_col = csr_col * 10 + c - '0';
	--csr_line; --csr_col;		/* adjust cursor to 0-origin */
}
put_cursor(line,col)
int line;
int col;
{	bdos(6,ESC); bdos(6,'[');
	++line; ++col;
	if (line > 9) bdos(6,line/10 + '0');
	bdos(6,line%10 + '0');
	bdos(6,';');
	if (col > 9) bdos(6,col/10 + '0');
	bdos(6,col%10 + '0');
	bdos(6,'H');
}
clear_screen()
{	cup_home();
	bdos(6,ESC); bdos(6,'['); bdos(6,'J');
}
e_eol()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'K');
}
e_eos()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'J');
}
cup_home()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'H');
}
cup_up()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'A');
}
cup_down()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'B');
}
cup_right()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'C');
}
disable_keyboard()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'2'); bdos(6,'h');
}
enable_keyboard()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'2'); bdos(6,'l');
}
term_init() {
	bdos(6,ESC); bdos(6,'['); bdos(6,'>');
	bdos(6,'6');			/* Enter keypad shifted mode */
	bdos(6,';');
	bdos(6,'7');			/* Enter alternate keypad mode */
	bdos(6,'h');
}
term_reset()
{	bdos(6,ESC); bdos(6,'['); bdos(6,'>');
	bdos(6,'6');			/* Exit keypad shifted mode */
	bdos(6,';');
	bdos(6,'7');			/* Exit alternate keypad mode */
	bdos(6,'h');
}
bdos (bc, de) int bc, de; {
#asm
	MOVE.L	8(SP),D0	get bc (function)
	CMPI.B	#6,D0
	BEQ.S	*+4
	RTS
	MOVE.L	4(SP),D2	get de
	BMI.S	CON_IN	console input
	MOVEQ	#8,D1
	SC	SC:DEVOUT
	RTS
CON_IN	DS	0
	MOVEQ	#8,D1
	SC	SC:DEVSTS
	BNE.S	CON_IN1
	MOVEQ	#0,D0
	RTS
CON_IN1	DS	0
	MOVEQ	#8,D1
	SC	SC:DEVIN
	MOVE.L	#$FF,D0
	AND.L	D1,D0
;	RTS		supplied by CC
#endasm
}
strcpy( d, s) char *d, *s; {
	while (*s) *d++ = *s++;
	*d = 0;
}
strcat( d, s) char *d, *s; {
	while (*d) ++d;			/* locate end of destination string */
	while (*s) *d++ = *s++;		/* copy source to destination */
	*d = 0;
}
movmem(s, d, l) char *s, *d; int l; {
#asm
	MOVE.L	4(SP),D0	get length
	BGT.S	*+4		don't move if zero
	RTS
	CMPI.L	#$1FFFF,D0
	BLT.S	*+6
	TRAP	15
	DC	0	breakpoint trap
	MOVE.L	12(SP),A0	get source address
	MOVE.L	8(SP),A1	get destination address
	CMPA.L	A1,A0	compare source with destination
	BLT.S	MOVE_0	move top to bottom
	SUBQ.L	#1,D0	adjust for DBRA counter
	MOVE.B	(A0)+,(A1)+
	DBRA	D0,*-2
	ADDQ.W	#1,D0
	SUBQ.L	#1,D0
	BGT	*-8
	RTS
MOVE_0	DS	0
	ADD.L	D0,A0
	ADD.L	D0,A1
	SUBQ.L	#1,D0	adjust for DBRA counter
	MOVE.B	-(A0),-(A1)
	DBRA	D0,*-2
	ADDQ.W	#1,D0
	SUBQ.L	#1,D0
	BGT	*-8
;	RTS			; return supplied by CC
#endasm
}
gets(s) char *s; {
	return (fgets(s, 20, stderr));
}
swapin( fname, addr) char *fname, *addr; {
	int i;
	if ((file=fopen(fname,"r"))==NULL) return 1;
	while (1) {
		if ((fgets(addr,topmem-addr,file))==0) break;
		while (*addr) ++addr;
		*addr++ = '\n';
	}
	*addr = EOFCHAR;
	fclose(file);
	return 0;
}
rename(from, to) char *from, *to; {
	if (CCRENAME(from, to) == ERR) fputs("Error on rename\n",stderr);
}
creat(fname) char *fname; {
	return fopen(fname,"w");
}
write(file,buf,sects) int file, sects; char *buf; {
	*botfile = 0;
	fputs(buf,file);
	return sects;
}
P),A1	get destination address
	CMPA.L	A1,A0	compare source with destination
	BLT.S	MOVE_0	move top to bottom
	SUBQ.L	#1,D0	