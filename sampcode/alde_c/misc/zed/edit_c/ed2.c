/* ED2.C */

#include "ed0.c"
#include "ed1.ccc"
#define SIGNON "E.K. Ream/Dr Dobb's Editor - IBM PC enhancement: Mar. 10, 1984"
#define SIGNNN "            Modified by Alan D. Howard"
#define HELP1 "open <f/n> | name <f/n> | rename <f/n> | delname <f/n>"
#define HELP2 "load <f/n> | append <f/n> | rest | read <n> | write <n> | save"
#define HELP3 "extract [<f t>] | closeread | closewrite"
#define HELP4 "clear | delete [<f t>] | move [<f t n>] | copy [<f t n>]"
#define HELP5 "find | search [<f t>] | change [<f t>] | list [<f t>]"
#define HELP6 "tabs <n> | showtab | hidetab | help | dos | g <n> | count <n>"
#define CMNDMODE	1
#define INSMODE 	2
#define EDITMODE	3
#define EXITMODE	4
int	begin_line, end_line, buf_picked;
main(argc,argv) char *argv[];
{
int mode;
int i;
	sysinit();
	fmtassn(NO);
	fmtset(8);
	fmtntab();
	outclr();
	outxy(0,SCRNL1);
	message(SIGNON);
	message(SIGNNN);
	outxy(0,1);
	fileclear();	/* make sure no read or write files indicated */
	bufnew();
	begin_line=(-1);	/* imaginary to start with */
	end_line=(-1);
	buf_picked=NO;
	mode=CMNDMODE;
	if (argc>1) {
		openf(argv[1],NO);
		rest(argv[0]);
		outxy(0,1);
	}
	edgetln();
	while(1){
		if (mode ==EXITMODE) {
			sysfinish();
			break;
		}
		else if (mode==CMNDMODE) {
			mode=command();
		}
		else if (mode==EDITMODE) {
			mode=edit();
		}
		else if (mode==INSMODE) {
			mode=insert();
		}
		else {
			syserr("main: no mode");
			mode=EDITMODE;
		}
	}
}
edit()
{
char c;
	pmtedit();
	while(1){
		c=syscin();	/* tolower eliminated */
		if (c==ESC1) {		/* enter command mode */
			return(CMNDMODE);
		}
		else if (c==INS1) {	/* enter insert mode */
			return(INSMODE);
		}
		else if (special(c)==YES) {
			if (c==UP1){	/* DOWN1 now treated separately */
				return(INSMODE);
			}
			else {
				continue;
			}
		}
		else if (c==DOWN1) {	/* DOWN1 now does not	 */
			eddn(); 	/* enter insert mode but */
			pmtline();	/* just advances to next line */
			edbegin();
			pmtcol();
		}
		else if (c==DTOCH){	/* kill to character entered */
			pmtmode("edit:	kill");
			c=syscin();
			if ((special(c)==NO) &
			    (control(c)==NO)) {
				edkill(c);
			}
			pmtedit();
		}
		else if (c==GTOCH){	/* search to character entered */
			pmtmode("edit: search");
			c=syscin();
			if ((special(c)==NO) &
			    (control(c)==NO)) {
				edsrch(c);
			}
			pmtedit();
		}
		else {		/* editor now exchanges any other character */
			if ((special(c)==NO) &	/* with cursor */
			    (control(c)==NO)) {
				edchng(c);
			}
			pmtcol();
		}
	}
}
insert()
{
char c;
	pmtmode("insert");
	while(1) {
		c=syscin();
		if (c==ESC1) {
			return(CMNDMODE);
		}
		else if (c==EDIT1) {
			return(EDITMODE);
		}
		else if (c==INS1) {
			;
		}
		else if (c==DOWN1) {	/* in insert mode DOWN1 treated  */
			ednewdn();	/* differently from edit mode	 */
			pmtline();	/* inserts new line */
			continue;
		}
		else if (c==DTOCH){	/* kill to character entered */
			pmtmode("Insert: kill");
			c=syscin();
			if ((special(c)==NO) &
			    (control(c)==NO)) {
				edkill(c);
			}
		}
		else if (c==GTOCH){	/* search to character entered */
			pmtmode("Insert: search");
			c=syscin();
			if ((special(c)==NO) &
			    (control(c)==NO)) {
				edsrch(c);
			}
		}
		else if (special(c)==YES) {
			continue;
		}
		else if (control(c)==YES) {
			continue;
		}
		else {
			edins(c);
			pmtcol();
		}
	}
}
control(c) char c;
{
	if (c==TAB) {
		return(NO);
	}
	else if (c>=127) {
		return(YES);
	}
	else if (c<32) {
		return(YES);
	}
	else {
		return(NO);
	}
}
special(c) char c;
{
char sbuffer[SCRNW1];
int v,x,y;
int k;
	if (c==JOIN1) {
		edjoin();
		pmtline();
		return(YES);
	}
	if (c==SPLT1) {
		edsplit();
		pmtline();
		return(YES);
	}
	if (c==ABT1) {
		edabt();
		pmtcol();
		return(YES);
	}
	else if (c==LFTDEL) {		/* delete to left of cursor */
		edldel();
		pmtcol();
		return(YES);
	}
	else if (c==DEL1) {		/* delete at cursor */
		edcdel();
		pmtcol();
		return(YES);
	}
	else if (c==ZAP1) {
		edzap();
		pmtline();
		return(YES);
	}
	else if (c==UP2) {
		edup();
		pmtline();
		return(YES);
	}
	else if (c==UP1) {
		ednewup();
		pmtline();
		return(YES);
	}

/* DOWN1 (CR) is no longer a special character */

	else if (c==DOWN2) {
		eddn();
		pmtline();
		return(YES);
	}
	else if (c==ABSLEFT) {		/* left one column */
		edabsleft();
		pmtcol();
		return(YES);
	}
	else if (c==ABSRGHT) {         /* right one column */
		edabsright();
		pmtcol();
		return(YES);
	}
	else if (c==LEFT1) {          /* left one character */
		edleft();
		pmtcol();
		return(YES);
	}
	else if (c==RIGHT1) {         /* right one character */
		edright();
		pmtcol();
		return(YES);
	}
	else if (c==BEGINPL) {		/* mark beginning line */
		begin_line=bufln();
		pmtmode("marked buff top");
		return(YES);
		}
	else if (c==ENDPL) {		/* mark ending line */
		end_line=bufln();
		pmtmode("marked buff end");
		return(YES);
		}                               	
	else if (c==PICKIT) {		/* pick marked lines to buffer */
		if (edpick(begin_line,end_line)!=ERR) {
			pmtmode("buffer picked");
			buf_picked=YES;
			}
		return(YES);
		}
	else if (c==RIGHT1) {	/* go to end of line */
		edright();
		pmtcol();
		return(YES);
	}
	else if (c==ERASE) {	/* erase to end of line */
		ederase();
		pmtcol();
		return(YES);
	}
	else if (c==HOME) {	/* move cursor alternatingly to */
		edhome();	/* top and bottom of screen */
		pmtline();
		sysabort();
		return(YES);
	}
	else if (c==LSTRT) {	/* move to beginning of line */
		edbegin();
		pmtcol();
		return(YES);
	}
	else if (c==PUTIT) {   /* put buffer before line*/
		if (buf_picked==YES) {
			edput();
			begin_line=(-1);
			end_line=(-1);
			}
		pmtline();
		return(YES);
	}
	else if (c==DSCROL) {	/* scroll down */
		pmtmode("Scroll down");
		while (bufnrbot()==NO) {
			if (chkkey()==YES) {
				break;
			}
			if (eddn()==ERR) {
				break;
			}
		}
		pmtline();
		return(YES);
	}
	else if (c==LEND){	/* move to end of line */
		edend();
		pmtcol();
		return(YES);
	}
	else if (c==PAGEUP) {  /* move up 20 lines */
		y=bufln();
		y=max(y-20,1);
		if (bufattop()!=YES) {
			edgo(y,0);
		}
		pmtline();
		return(YES);
	}
	else if (c==PAGEDN) {   /*move down 20 lines */
		y=bufln();
		y=y+20;
		if (bufnrbot()!=YES) {
			edgo(y,0);
		}
		pmtline();
		return(YES);
	}
	else if (c==GOTO){	/* go to line entered */
		x=outxget();
		y=outyget();
		pmtcmnd("Goto: ",sbuffer);
		if(number(sbuffer,&v)) {
			edgo(v,0);
		}
		else {
			outxy(x,y);
		}
		return(YES);
	}
	else if (c==USCROL) {	/* scroll up */
		pmtmode("Scroll up");
		while (bufattop()==NO) {
			if (chkkey()==YES) {
				break;
			}
			if (edup()==ERR) {
				break;
			}
		}
		pmtline();
		return(YES);
	}
	else  {
		return(NO);
	}
}
command()
{
int v;
char c;
char args[SCRNW1];
char *argp;
int topline;
int ypos;
int oldline;
int k;
	edrepl();
	oldline=bufln();
	ypos=outyget();
	topline=oldline-ypos+1;
	while(1) {
		outxy(0,SCRNL1);
		fmtcrlf();
		pmtmode("command:");
		getcmnd(args,0);
		fmtcrlf();
		pmtline();
		c=args[0];
		if ((c==EDIT1)|(c==INS1)) {
			if (oldline==bufln()) {
				edgetln();
				bufout(topline,1,SCRNL1);
				outxy(0,ypos);
			}
			else {
				edgo(bufln(),0);
			}
			if (c==EDIT1) {
				return(EDITMODE);
			}
			else {
				return(INSMODE);
			}
		}
		else if (tolower(args[0])=='g'){
			argp=skipbl(args+1);
			if (argp[0]==EOS) {
				edgo(oldline,0);
				return(EDITMODE);
			}
			else if (number(argp,&v)==YES) {
				edgo(v,0);
				return(EDITMODE);
			}
			else {
				message("bad line number");
			}
		}
		else if (lookup(args,"append")) {
			append(args);
		}
		else if (lookup(args,"change")) {
			change(args,begin_line,end_line);
		}
		else if (lookup(args,"clear")) {
			sysabort();
			clear();
		}
		else if (lookup(args,"count")) { /* repeat count for replay */ 
			count(args);
		}
		else if (lookup(args,"delete")) {
			delete(args,begin_line,end_line);
			begin_line=(-1);
			end_line=(-1);
			buf_picked=YES;
		}
		else if (lookup(args,"dos")) {
			sysabort();
			if (chkbuf()==YES) {
				closewrite();	/* write file closed on exit */
				return(EXITMODE);
			}
		}
		else if (lookup(args,"find")) {
			if ((k=find()) >= 0) {
				edgo(bufln(),k);
				return(EDITMODE);
			}
			else {
				bufgo(oldline);
				edgetln();
				message("pattern not found");
			}
		}
		else if (lookup(args,"list")) {
			list(args,begin_line,end_line);
		}
		else if (lookup(args,"open")) { 	/* new */
			sysabort();
			openf(args,YES);
		}
		else if (lookup(args,"load")) { 	/* changed */
			sysabort();
			openf(args,YES);
			rest(args);
		}
		else if (lookup(args,"name")) {
			sysabort();
			name(args);
		}
		else if (lookup(args,"write")) {	/* new */
			sysabort();
			writel(args);
		}
		else if (lookup(args,"rename")) {	/* new */
			sysabort();
			rename(args);
		}
		else if (lookup(args,"delname")) {	/* new */
			sysabort();
			delname(args);
		}
		else if (lookup(args,"closewrite")) {	/* new */
			sysabort();
			closewrite(args);
		}
		else if (lookup(args,"showtab")) {    /* new */
			fmtytab();
		}
		else if (lookup(args,"hidetab")) {    /* new */
			fmtntab();
		}
		else if (lookup(args,"closeread")) {	/* new */
			sysabort();
			closeread(args);
		}
		else if (lookup(args,"help")) { 	/* new */
			sysabort();
			outclr();
			outxy(0,SCRNL1);
			message(HELP1);
			message(HELP2);
			message(HELP3);
			message(HELP4);
			message(HELP5);
			message(HELP6);
		}
		else if (lookup(args,"save")) {
			sysabort();
			save(args);
		}
		else if (lookup(args,"read")) { 	 /* new */
			sysabort();
			getit(args);
		}
		else if (lookup(args,"move")) { 	/* new */
			moveit(args,begin_line,end_line,bufln());
			begin_line=(-1);
			end_line=(-1);
		}
		else if (lookup(args,"copy")) { 	/* new */
			copyit(args,begin_line,end_line,bufln());
			begin_line=(-1);
			end_line=(-1);
		}
		else if (lookup(args,"search")) {
			sysabort();
			search(args,begin_line,end_line);
		}
		else if (lookup(args,"tabs")) {
			tabs(args);
		}
		else if (lookup(args,"rest")) { 	/* new */
			sysabort();
			rest(args);
		}
		else if (lookup(args,"extract")) {	/* new */
			sysabort();
			extract(args);
		}
		else {
			sysabort();
			message("command not found");
		}
	}
}
lookup(line,command) char *line, *command;
{
	while(*command) {
		if (tolower(*line++)!=*command++) {
			return(NO);
		}
	}
	if((*line==EOS)|(*line==' ')|(*line==TAB)) {
		return(YES);
	}
	else {
		return(NO);
	}
}
getcmnd(args,offset) char *args; int offset;
{
int j,k;
char c;
	outxy(offset++,outyget());
	outdeol();
	scr_chr_attr(SETINTENSE);  /* set up prompt */
	scr_chr_attr(SETBLINK);
	syscout('>');
	scr_chr_attr(NOBLINK);
	k=0;
	while ((c=syscin())!=CR) {
		if ((c==EDIT1)|(c==INS1)) {
			args[0]=c;
			return;
		}
		if ((c==DEL1)|(c==LEFT1)) {
			if (k>0) {
				outxy(offset,outyget());
				outdeol();
				k--;
				j=0;
				while (j<k) {
					outchar(args[j++]);
				}
			}
		}
		else if (c==ABT1) {
			outxy(offset,outyget());
			outdeol();
			k=0;
		}
		else if ((c!=TAB)&((c<32)|(c==127))) {
			continue;
		}
		else {
			if ((k+offset)<SCRNW1) {
				args[k++]=c;
				outchar(c);
			}
		}
	}
	args[k]=EOS;
	scr_chr_attr(SETFAINT);
	offset--;			/* erase prompt */
	outxy(offset,outyget());
	syscout(' ');
}
