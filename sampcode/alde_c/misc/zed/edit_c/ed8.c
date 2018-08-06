/* ED8.C */

#include "ed0.c"
#include "ed1.ccc"
char	keybuf[MAXKEY];  /* macro buffer for keystroke record and replay */
int	replay_count; /* how many times to replay */    
int	replay_number; /* current replay number */
int	replay_flag;  /* are we replaying from buffer? */
int	record_flag;  /* are we recording to buffer? */
int	key_position; /* current position in buffer */
sysabort()	/*  abort replay of buffer */
{
	record_flag=NO;
	replay_flag=NO;
	key_position=0;
	replay_number=0;
}
syscount(count) int count;	/* set number of replays */
{
	replay_count=count;
}
syscstat()
{
int c;
	c=scr_csts();
	if (c==0){		/* correction of original version */
		return(-1);
	}
	else {
		return(c);
	}
}
syscin() /* extensively altered to permit recording and replay of buffer */
{		
int c;
	if (replay_flag==YES) {  /* proceed if we are replaying */
		if ((c=syscstat())>0) {
			pmtmode("       "); /* abort replay if key pressed */
			record_flag=NO;
			replay_flag=NO;
			key_position=0;
			replay_number=0;
			return(c);
		}
		else {
			c=keybuf[key_position++]; /* otherwise replay */
			if (c==EOS) {
				key_position=0;
				if((++replay_number)==replay_count) {
					pmtmode("        "); /* finished */
					replay_flag=NO;
					replay_number=0;
				}
			}
			return(c);
		}
	}
	else {
		while ((c=scr_ci())==0) {
			;
		}
		if (c==RECORD) {
			if (record_flag==NO) {  /* begin record */
				pmtmode("record");
				record_flag=YES;
				replay_flag=NO;
				key_position=0;
				keybuf[key_position]=EOS;
			}
			else {
				pmtmode("end record"); /* end record */
				record_flag=NO;
			}
			return(c);
		}
		else if (c==REPLAY) {  /* start replay */
			pmtmode("replay");
			replay_flag=YES;
			record_flag=NO;
			replay_number=0;
			key_position=0;
			return(c);
		}
		else if (record_flag==YES) {
			if (key_position<MAXKEY1) {
				keybuf[key_position++]=c;
				keybuf[key_position]=EOS;
				return(c);
			}
			else {
				record_flag=NO;
				key_position=0;
				keybuf[key_position]=EOS;
				error("record overflow");
				return(c);
			}
		}
		else  {
			return(c);
		}
	}
}
syscout(c) char c;
{
	scr_co(c);
	return(c);
}
syslout(c) char c;
{
	_os(5,c);
	return(c);
}
sysend()
{
	return(_showsp()-200);
}	/* added */
syscreate(name) char *name;
{
int file;
	if ((file=creat(name))==0) {
		return(ERR);
	}
	else {
		return(file);
	}
}
sysopen(name,mode) char *name, *mode;
{
int file;
	if ((file=fopen(name,mode))==0) {
		return(ERR);
	}
	else {
		return(file);
	}
}
sysclose(file) int file;
{
	close(file);
	return(OK);
}
sysrdch(file) int file;
{
int c;
	if ((c=getc(file))==-1) {
		return(EOF);
	}
	else if (c==LF) {	
		if ((c=getc(file))==-1) {
			return(EOF);
			}
	}
	return(c);
}
syspshch(c,file) char c; int file;
{
	if (putc(c,file)==-1) {	
			error("disk write failed");
			return(ERR);
	}
	if (c==CR) {
		if (putc(LF,file)==-1) {
		error("disk write failed");
		return(ERR);
		}
	}
	return(c);
}
syspopch(file) int file;
{
	error("syspopch() not implemented");
	return(ERR);
}
syschkfn(args) char *args;
{
	if (args[0] == EOS) {	/* add check for zero-length file name */
		message("no file name");
		return(ERR);
	}
	else {
		return(OK);
	}
}
syscopfn(args,buffer) char *args, *buffer;
{
int n;
	n=0;
	while (n<SYSFNMAX-1) {
		if (args[n]==EOS) {
			break;
		}
		else {
			buffer[n]=args[n];
			n++;
		}
	}
	buffer[n]=EOS;
}
sysdnmov(n,d,s) int n,*d,*s;
{
	if (n==0) {
		return;
	}
	_move(n,s,d);
}	/* added */
sysupmov (n,d,s) int n,*d,*s;
{
	if (n==0) {
		return;
	}
	_move(n,s,d);
}	/* added */
amatch (a,b,c) int a,b,c;
/* matches pattern at b to line at a
   all all question marks match
   if flag c is NO match is only at beginning of line
*/
{
#asm
	mov	bx,[bp+06]
	mov	dx,[bp+04]
	mov	al,[bx]
	cmp	al,0
	je	no_match
	mov	cx,0
	mov	si,[bp+04]
cycles:	mov	ah,[si]
	cmp	ah,0
	je	no_match
	cmp	al,ah
	jne	not_so_far
reprieve:	inc	bx
	inc	si
	mov	al,[bx]
	cmp	al,0
	je	success
	jmp	cycles
not_so_far:	cmp	al,'?'
	je	reprieve
	mov	ax,[bp+08]
	cmp	ax,0
	je	no_match
	inc	dx
	inc	cx
	mov	si,dx
	mov	bx,[bp+06]
	mov	al,[bx]
	jmp	cycles
success:	mov	ax,cx
	jmp	returns
no_match:	mov	ax,0ffffh
returns:	nop
#
}
sysdelay()
{
}
sysinit()
{
	scr_setup(); /* start things up & initialize */
	scr_setmode(3);
	scr_clr();
	scr_chr_attr(SETNORMAL);
	scr_chr_attr(SETFAINT);
	scr_chr_attr(NOBLINK);
	replay_flag=NO;
	record_flag=NO;
	replay_count=1;
	keybuf[0]=EOS;
	key_position=0;
}
sysfinish()
{
	scr_chr_attr(SETNORMAL); /* finish up loose ends */
	scr_chr_attr(SETFAINT);
	scr_chr_attr(NOBLINK);
	scr_clr();
	scr_rowcol(0,0);
}
