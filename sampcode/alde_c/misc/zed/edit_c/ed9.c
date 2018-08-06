/* ED9.C */

#include "ed0.c"
#include "ed1.ccc"
toupper(c) int c;
{
	if ((c<'a')|(c>'z')) {
		return(c);
	}
	else {
		return(c-32);
	}
}
tolower(c) int c;
{
	if ((c<'A')|(c>'Z')) {
		return(c);
	}
	else {
		return(c+32);
	}
}
number(args,val) char *args; int *val;
{
char c;
	c= *args++;
	if ((c<'0')|(c>'9')) {
		return(NO);
	}
	*val=c-'0';
	while (c= *args++) {
		if ((c<'0')|(c>'9')) {
			break;
		}
		*val=(*val*10)+c-'0';
	}
	return(YES);
}
ctoi(buf,index) char *buf; int index;
{
int k;
	while ((buf[index]==' ')|
		(buf[index]==TAB)){
		index++;
	}
	k=0;
	while ((buf[index]>='0')&(buf[index]<='9')) {
		k=(k*10)+buf[index]-'0';
		index++;
	}
	return(k);
}
max(m,n) int m,n;
{
	if (m>=n) {
		return(m);
	}
	else {
		return(n);
	}
}
min(m,n) int m,n;
{
	if (m<=n) {
		return(m);
	}
	else {
		return(n);
	}
}
putdec(n,w) int n,w;
{
char chars[10];
int i,nd;
	nd=itoc(n,chars,10);
	i=0;
	while (i<nd) {
		syscout(chars[i++]);
	}
	i=nd;
	while (i++<w) {
		syscout(' ');
	}
}
itoc(n,str,size) int n; char *str; int size;
{
int absval;
int len;
int i,j,k;
	absval=abs(n);
	str[0]=0;
	i=1;
	while (i<size) {
		str[i++]=(absval%10)+'0';
		absval=absval/10;
		if (absval==0) {
			break;
		}
	}
	if ((i<size)&(n<0)) {
		str[i++]='-';
	}
	len=i-1;
	i--;
	j=0 ;
	while (j<i) {
		k=str[i];
		str[i]=str[j];
		str[j]=k;
		i--;
		j++;
	}
	return(len);
}
abs(n) int n;
{
	if (n<0) {
		return(-n);
	}
	else {
		return(n);
	}
}
syserr(s) char *s;
{
	scr_chr_attr(SETBLINK);
	pmtmess("system error: ",s);
	scr_chr_attr(NOBLINK);
}
error(s) char *s;
{
	scr_chr_attr(SETBLINK);
	pmtmess("error: ",s);
	scr_chr_attr(NOBLINK);
}
diskerr(s) char *s;
{
	scr_chr_attr(SETBLINK);
	pmtmess("disk error: ",s);
	scr_chr_attr(NOBLINK);
}
readline(file,p,n) int file; char *p; int n;
{
int c;
int k;
	k=0;
	while (1) {
		c=sysrdch(file);
		if (c==ERR) {
			return(ERR);
		}
		if (c==EOF) {
			return(EOF);
		}
		if (c==CR) {
			return(k);
		}
		if (k<n) {
			*p++=c;
		}
		k++;
	}
}
pushline(file,p,n) int file; char *p; int n;
{
	while((n--)>0) {
		if (syspshch(*p++,file)==ERR) {
			return(ERR);
		}
	}
	return(syspshch(CR,file));
}
popline(file,p,n) int file; char *p; int n;
{
int c;
int k, kmax, t;
	c=syspopch(file);
	if (c==EOF) {
		return(EOF);
	}
	if (c==CR) {
		*p++ =CR;
		k=1;
	}
	else {
		syserr("popline:  missing CR");
		return(ERR);
	}
	while (1) {
		c=syspopch(file);
		if (c==ERR) {
			return(ERR);
		}
		if (c==EOF) {
			break;
		}
		if (c==CR) {
			if (syspshch(CR,file)==ERR) {
				return(ERR);
			}
			break;
		}
		if (k<n) {
			*p++ =c;
		}
		k++;
	}
	kmax=k;
	k=min(k,n-1);
	t=0;
	while (k>t) {
		c=p[k];
		p[k]=p[t];
		p[t]=c;
		k--;
		t++;
	}
	return(kmax);
}
