/* ED4.C */

#include "ed0.c"
#include "ed1.ccc"
char	editbuf[MAXLEN];
char pickbuf[PUTBUFLN]; /* pick buffer */
int picklines, pickend; /* pick buffer parameters */
int	editp;
int	editpmax;
int	edcflag;
edabt()
{
	edgetln();
	edredraw();
	edbegin();
	edcflag=NO;
}
edbegin()
{
	editp=0;
	outxy(0,outyget());
}
ederase()	/* new: erase from cursor to end of line */
{
	if (editp==editpmax) {
		return;
	}
	edcflag=YES;
	editpmax=editp;
	edredraw();
}
edhome()	/* new: home cursor.  subsequent calls alternate */
{		/* cursor to top and bottom of screen */
int ypos;
	if (edrepl()!=OK){
		return;
	}
	if ((ypos=outyget())<SCRNL3) {
		while((ypos<SCRNL1)&&(!bufnrbot())){
			if (bufdn()!=OK){
				return(ERR);
			}
			ypos++;
		}
	}
	else {
		while((ypos>1)&&(!bufattop())){
			if (bufup()!=OK){
				return(ERR);
			}
			ypos--;
		}
	}
	edgetln();
	outxy(edxpos(),ypos);
}
edpick(begin,end) int begin,end; {	/* pick marked lines to buffer */
int oldline,oldx,oldy,len,nlines;
	if ((begin==(-1))|(end==(-1))|(end<begin)){
		sysabort();
		return(ERR);
		}
	oldx=outxget();
	oldy=outyget();
	if (edrepl()!=OK) {
		return(ERR);
		}
	oldline=bufln();
	nlines=end-begin+1;
	len=buflength(begin,nlines);
	if (len>PUTBUFLN){
		pmtmode("Not picked: no room");
		sysabort();
		return(ERR);
		}
	pickend=len;
	buftopick(pickbuf,begin,nlines);
	bufgo(oldline);
	edgetln();
	editp=edscan(oldx);
	outxy(oldx,oldy);
	edredraw();
	picklines=nlines;
	return(OK);
	}
edput() {		/* put lines in pick buffer into main buffer */
			/* before current line */
int k,oldline,oldx,oldy;
	if (pickend==0){
		return(OK);
		}
	oldx=outxget();
	oldy=outyget();
	if (edrepl()!=OK){
		return(ERR);
		}
	oldline=bufln();
	if (picktobuf(pickbuf,pickend,picklines)!=OK) {
		return(ERR);
		}
	bufgo(oldline);
	if (edatbot()){
		;
	}
	else{
		bufout(bufln()+1,oldy+1,SCRNL1-oldy);
		}
	edgetln();
	outxy(0,oldy);
	edredraw();
	editp=min(oldx,editpmax);
	outxy(edxpos(),oldy);
	return(OK);
	}
edchng(c) char c;
{
char oldc;
int k,kk,n,m,tabpos;
	if (editp>=editpmax) {
		edins(c);
		return;
	}
/* experimental code for better treatment of virtual columns */
	if (editbuf[editp]==c) {
		edright();
		return;
	}
	tabpos=outxget()%tablength();
	if ((editbuf[editp]==TAB)&(tabpos!=tablength()-1)) {
		edins(c);
		return;
	}
	kk=editp-1;
	m=NO;
	if ((kk>=0)&(editbuf[kk]==TAB)&(tabpos!=0)) {
		editp--;
		n=outxget()-edxpos();
		if ((n+fmtlen(editbuf,editpmax)+1)<SCRNW1) {
			k=editpmax;
			while (k>(editp+1)) { 
				editbuf[k+n-1]=editbuf[k-1];
				k--;
			}
			while (n-- >0) {
				editbuf[editp++]=' ';
				editpmax++;
			}
			m=YES;
			fmtadj(editbuf,kk,editpmax);
		}
	}
/* end experimental code */
	oldc=editbuf[editp];
	editbuf[editp]=c;
	fmtadj(editbuf,editp,editpmax);
	k=fmtlen(editbuf,editpmax);
	if (k>SCRNW1) {
		editbuf[editp]=oldc;
		fmtadj(editbuf,editp,editpmax);
	}
	else {
		edcflag=YES;
		editp++;
		if ((c==TAB)|(oldc==TAB)) {
		edredraw();
		}
		else {
		fmtchdev(c);
		}
	}
/* begin experimental code */
	if ((m==YES)&(tabpos!=(tablength()-1))) {
		k=editpmax;
		while (k>editp) {
			editbuf[k]=editbuf[k-1];
			k--;
		}
		editbuf[editp]=TAB;
		editpmax++;
		fmtadj(editbuf,editp,editpmax);
		edredraw();
	}
/* end experimental code */
}
edldel()   /* deletes character before cursor */
/* eddel() in original version: no change in code */
{
int k;
	if(edxpos() < outxget()) {
		outxy(outxget()-1, outyget());
		return;
	}
	if (editp==0) {
		return;
	}
	edcflag=YES;
	k=editp;
	while (k<editpmax) {
		editbuf[k-1]=editbuf[k];
		k++;
	}
	editp--;
	editpmax--;
	edredraw();
}
edcdel()   /* deletes character at cursor */
{
int k;
	if(edxpos() < outxget()) {
		outxy(outxget()-1, outyget());
		return;
	}
	edcflag=YES;
	if (editp == editpmax)	{
		if (editp==0) {
			return;
		}
		editp--;
		editpmax--;
		edredraw();
		return;
	}
	k=editp;
	while (k<(editpmax-1)) {
		editbuf[k]=editbuf[k+1];
		k++;
	}
	editpmax--;
	edredraw();
}
eddn()
{
int oldx;
	oldx=outxget();
	if (edrepl()!=OK) {
		return(ERR);
	}
	if (bufnrbot()) {
		sysabort();
		return(OK);
	}
	if (bufdn()!=OK) {
		return(ERR);
	}
	edgetln();
	editp=edscan(oldx);
	if (edatbot()) {
		edsup(bufln()-SCRNL2);
		outxy(oldx,SCRNL1);
	}
	else {
		outxy(oldx,outyget()+1);
	}
	return(OK);
}
edend()
{
	editp=editpmax;
	outxy(edxpos(),outyget());
}
edgo(n, p) int n, p;
{
	if (edrepl()==ERR) {
		return(ERR);
	}
	if (bufgo(n)==ERR) {
		return(ERR);
	}
	if (bufatbot()) {
		if (bufup()==ERR) {
			return(ERR);
		}
	}
	bufout(bufln(),1,SCRNL1);
	edgetln();
	editp=min(p,editpmax);
	outxy(edxpos(),1);
	return(OK);
}
edins(c) char c;
{
int k,kk,n,l;
	if (editpmax>=MAXLEN){
		return;
	}
	if ((editp==editpmax) & (edxpos()<outxget())) {
		k=outxget() - edxpos();
		editpmax=editpmax+k;
		while (k-- > 0) {
			editbuf [editp++] = ' ';
		}
		editp=editpmax;
	}
/* experimental code for better treatment of virtual columns */
	kk=editp-1;
	if ((kk>=0)&(editbuf[kk]==TAB)&((l=outxget()%tablength())!=0)) {
		editp--;
		n=outxget()-edxpos();
		if ((n+fmtlen(editbuf,editpmax)+1)<SCRNW1) {
			k=editpmax;
			while (k>(editp+1)) { 
				editbuf[k+n-1]=editbuf[k-1];
				k--;
			}
			while (n-- >0) {
				editbuf[editp++]=' ';
				editpmax++;
			}
			editbuf[editp]=c;
			editp++;
			if (l!=(tablength()-1)) {
				k=editpmax;
				while (k>editp) {
					editbuf[k]=editbuf[k-1];
					k--;
				}
				editbuf[editp]=TAB;
				editpmax++;
			}
			fmtadj(editbuf,kk,editpmax);
			edredraw();
			return;
		}
	}
/* end experimental code */
	k=editpmax;
	while (k>editp) {
		editbuf[k]=editbuf[k-1];
		k--;
	}
	editbuf[editp]=c;
	editp++;
	editpmax++;
	fmtadj(editbuf,editp-1,editpmax);
	k=fmtlen(editbuf,editpmax);
	if (k>SCRNW1) {
		edldel();
	}
	else {
		edcflag=YES;
		edredraw();
	}
}
edjoin()
{
int k;
	if (bufattop()) {
		return;
	}
	if (edrepl()!=OK) {
		return;
	}
	if (bufup()!=OK) {
		return;
	}
	k = bufgetln(editbuf,MAXLEN);
	if (bufdn()!=OK) {
		return;
	}
	k=k+bufgetln(editbuf+k,MAXLEN-k);
	if (k>SCRNW1) {
		bufgetln(editbuf,MAXLEN);
		return;
	}
	if (bufup()!=OK) {
		return;
	}
	editpmax=k;
	edcflag=YES;
	if (edrepl()!=OK) {
		return;
	}
	if (bufdn()!=OK) {
		return;
	}
	if (bufdel()!=OK) {
		return;
	}
	if (bufup()!=OK) {
		return;
	}
	if (edattop()) {
		edredraw();
	}
	else {
		k=outyget()-1;
		bufout(bufln(),k,SCRNL-k);
		outxy(0,k);
		edredraw();
	}
}
edkill(c) char c;
{
int k,p;
	if (editp==editpmax) {
		return;
	}
	edcflag=YES;
	k=1;
	while ((editp+k)<editpmax) {
		if (editbuf[editp+k]==c){
			break;
		}
		else {
			k++;
		}
	}
	p=editp+k;
	while (p<editpmax) {
		editbuf[p-k]=editbuf[p];
		p++;
	}
	editpmax=editpmax-k;
	edredraw();
}
edleft()
{
int k;
	if (edxpos()<outxget()){
		outxy(max(0,outxget()-1),outyget());
	}
	else if (editp!=0){
		editp--;
		outxy(edxpos(),outyget());
	}
}
ednewdn()
{
int k;
	if (bufatbot()){
		if (bufins(editbuf,editpmax)!=OK){
			return;
		}
	}
	else if (edrepl()!=OK){
			return;
	}
	if (bufdn()!=OK){
		return;
	}
	if (bufins(editbuf,0)!=OK){
		return;
	}
	edgetln();
	if (edatbot()){
		edsup(bufln()-SCRNL2);
		outxy(edxpos(),SCRNL1);
	}
	else {
		k=outyget();
		bufout(bufln(),k+1,SCRNL1-k);
		outxy(edxpos(),k+1);
	}
}
ednewup()
{
int k;
	if (edrepl()!=OK) {
		return;
	}
	if (bufins(editbuf,0)!=OK){
		return;
	}
	edgetln();
	if (edattop()){
		edsdn(bufln());
		outxy(edxpos(),1);
	}
	else {
		k=outyget();
		bufout(bufln(),k,SCRNL-k);
		outxy(edxpos(),k);
	}
}
edright()	/* right one literal character - tabs move to next tab stop */
{
	if (edxpos()<outxget()){
		outxy(min(SCRNW1,outxget()+1),outyget());
	}
	else if (edxpos()>outxget()){
		outxy(edxpos(),outyget());
	}
	else if (editp<editpmax){
		editp++;
		outxy(edxpos(),outyget());
	}
	else {
		outxy(min(SCRNW1,outxget()+1),outyget());
	}
}
edsplit()
{
int p,q;
int k;
	edcflag = NO;
	if (bufatbot()){
		if (bufins(editbuf,editp)!=OK){
			return;
		}
	}
	else {
		if (bufrepl(editbuf,editp)!=OK){
			return;
		}
	}
	p=editpmax;
	q=editp;
	editpmax=editp;
	editp=0;
	edredraw();
	editp=0;
	while (q<p){
		editbuf[editp++] = editbuf[q++];
	}
	editpmax=editp;
	editp=0;
	if (bufdn()!=OK){
		return;
	}
	if (bufins(editbuf,editpmax)!=OK) {
		return;
	}
	if (edatbot()) {
		edsup(bufln()-SCRNL2);
		outxy(1,SCRNL1);
		edredraw();
	}
	else {
		k=outyget();
		bufout(bufln(),k+1,SCRNL1-k);
		outxy(1,k+1);
		edredraw();
	}
}
edsrch(c) char c;
{
	if (editp==editpmax){
		return;
	}
	editp++;
	while (editp<editpmax) {
		if (editbuf[editp]==c) {
			break;
		}
		else {
			editp++;
		}
	}
	outxy(edxpos(),outyget());
}
edabsright()  /* right one column - one virtual character */
{
int xpos;
	xpos=outxget();
	if (xpos<SCRNW1) {
		outxy(++xpos,outyget());
		editp=edscan(xpos);
	}
}
edabsleft()   /* left one column - one virtual character */
{
int xpos;
	xpos=outxget();
	if (xpos>0) {
		outxy(--xpos,outyget());
		editp=edscan(xpos);
	}
}
edup()
{
int oldx;
	oldx=outxget();
	if (edrepl()!=OK) {
		return(ERR);
	}
	if (bufattop()) {
		sysabort();
		return(OK);
	}
	if (bufup()!=OK){
		return(ERR);
	}
	edgetln();
	editp=edscan(oldx);
	if (edattop()) {
		edsdn(bufln());
		outxy(oldx,1);
	}
	else {
		outxy(oldx,outyget()-1);
	}
	return(OK);
}
edzap()
{
int k;
	if (bufdel()!=OK){
		return;
	}
	if (bufatbot()){
		if (bufup()!=OK){
			return;
		}
		edgetln();
		if (edattop()) {
			edredraw();
		}
		else {
			outdelln();
			outxy(0,outyget()-1);
		}
		return;
	}
	edgetln();
	if (edattop()) {
		edsup (bufln());
		outxy(0,1);
	}
	else {
		k=outyget();
		bufout(bufln(),k,SCRNL-k);
		outxy(0,k);
	}
}
edatbot()
{
	return(outyget()==SCRNL1);
}
edattop()
{
	return(outyget()==1);
}
edredraw()
{
	fmtadj(editbuf,0,editpmax);
	fmtsubs(editbuf,max(0,editp-1),editpmax);
	outxy(edxpos(),outyget());
}
edxpos()
{
	return(min(SCRNW1,fmtlen(editbuf,editp)));
}
edgetln()
{
int k;
	editp=0;
	edcflag=NO;
	k=bufgetln(editbuf,MAXLEN);
	if (k>MAXLEN) {
		error("line truncated");
		editpmax=MAXLEN;
	}
	else {
		editpmax=k;
	}
	fmtadj(editbuf,0,editpmax);
}
edrepl()
{
	if (edcflag==NO) {
		return(OK);
	}
	edcflag=NO;
	if (bufatbot()){
		return(bufins(editbuf,editpmax));
	}
	else {
		return(bufrepl(editbuf,editpmax));
	}
}
edscan(xpos) int xpos;
{
	editp=0;
	while (editp<editpmax){
		if (fmtlen(editbuf,editp)<xpos){
			editp++;
		}
		else {
			break;
		}
	}
	return(editp);
}
edsup(topline) int topline;
{
	if (outuphas()==YES){
		outsup();
		bufout(topline+SCRNL2,SCRNL1,1);
	}
	else {
		bufout(topline,1,SCRNL1);
	}
}
edsdn(topline) int topline;
{
	if (outdnhas()==YES) {
		outsdn();
		bufout(topline,1,1);
	}
	else {
		bufout(topline,1,SCRNL1);
	}
}
