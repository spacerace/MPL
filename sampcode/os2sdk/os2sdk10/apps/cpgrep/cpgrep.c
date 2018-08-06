/* cpgrep
*
* HISTORY:
* 23-Mar-87     danl    exit(x) -> DOSEXIT(1, x)
*
*/

#include		<stdio.h>
#include		<fcntl.h>
#include		<ctype.h>

int far pascal		DOSCREATETHREAD(void (far *)(),int far *,int far *);
int far pascal		DOSGETMACHINEMODE(char far *);
int far pascal		DOSQHANDTYPE(int,int far *,int far *);
int far pascal		DOSREAD(int,char far *,int,int far *);
int far pascal		DOSSEMCLEAR(long far *);
int far pascal		DOSSEMREQUEST(long far *,long);
int far pascal		DOSSEMSET(long far *);
int far pascal		DOSSEMWAIT(long far *,long);
int far pascal		DOSSLEEP(long);
int far pascal		DOSWRITE(int,char far *,int,int far *);
int far pascal		DOSEXIT(int, int);

#define	BEGLINE		0x40
#define	DEBUG		0x08
#define	ENDLINE		0x80
#define	FILBUFLEN	(SECTORLEN*30)
#define	FILNAMLEN	80
#define	INVERT		0x10
#define	ISCOT		0x0002
#define	LG2SECLEN	9
#define	LINELEN		128
#define	LINENOS		0x04
#define	LNOLEN		8
#define	MAXSTRLEN	128
#define	NAMEONLY	0x02
#define	OUTBUFLEN	(SECTORLEN*4)
#define	SECTORLEN	(1 << LG2SECLEN)
#define	SHOWNAME	0x01
#define	STKLEN		256
#define	TIMER		0x20
#define	TRTABLEN	256
#define	s_text(x)	(((char *)(x)) - ((x)->s_must))

typedef struct stringnode
  {
    struct stringnode	*s_alt;		/* List of alternates */
    struct stringnode	*s_suf;		/* List of suffixes */
    int			s_must;		/* Length of portion that must match */
  }
			STRINGNODE;

char			filbuf[(FILBUFLEN + 2)*2];
char			*bufptr[] = { filbuf, filbuf + FILBUFLEN + 2 };
char			outbuf[OUTBUFLEN*2];
char			*obuf[] = { outbuf, outbuf + OUTBUFLEN };
char			*optr[] = { outbuf, outbuf + OUTBUFLEN };
int			ocnt[] = { OUTBUFLEN, OUTBUFLEN };
int			oi = 0;
char			transtab[TRTABLEN] = { 0 };
STRINGNODE		*stringlist[TRTABLEN/2];
int			arrc;		/* I/O return code for DOSREAD */
int			awrc;		/* I/O return code for DOSWRITE */
int			casesen = 1;	/* Assume case-sensitivity */
int			cbread;		/* Bytes read by DOSREAD */
int			cbwrite;	/* Bytes written by DOSWRITE */
int			clists = 1;	/* One is first available index */
int			flags;		/* Flags */
int			lineno;		/* Current line number */
char			pmode;		/* Protected mode flag */
long			readdone;	/* Async read done semaphore */
long			readpending;	/* Async read pending semaphore */
int			status = 1;	/* Assume failure */
char			*t2buf;		/* Async read buffer */
int			t2buflen;	/* Async read buffer length */
int			t2fd;		/* Async read file */
char			*t3buf;		/* Async write buffer */
int			t3buflen;	/* Async write buffer length */
int			t3fd;		/* Async write file */
long			writedone;	/* Async write done semaphore */
long			writepending;	/* Async write pending semaphore */
char			target[MAXSTRLEN];
					/* Last string added */
int			targetlen;	/* Length of last string added */

int			countlines();	/* See CPGREPSB.ASM */
int			countmatched();	/* See CPGREPSB.ASM */
char			*findlist();	/* See CPGREPSB.ASM */
char			*findone();	/* See CPGREPSB.ASM */
void			flush1buf();	/* See below */
int			grepbuffer();	/* See below */
int			revfind();	/* See CPGREPSB.ASM */
void			write1buf();	/* See below */
char			*(*find)() = findlist;
void			(*flush1)() = flush1buf;
int			(*grep)() = grepbuffer;
void			(*write1)() = write1buf;

void			freenode(x)
register STRINGNODE	*x;		/* Pointer to node to free */
  {
    register STRINGNODE	*y;		/* Pointer to next node in list */

    while(x != NULL)			/* While not at end of list */
      {
	if(x->s_suf != NULL) freenode(x->s_suf);
					/* Free suffix list */
	y = x;				/* Save pointer */
	x = x->s_alt;			/* Move down the list */
	free(y);			/* Free the node */
      }
  }


STRINGNODE		*newnode(s,n)
char			*s;		/* String */
int			n;		/* Length of string */
  {
    register STRINGNODE	*new;		/* Pointer to new node */
    char		*t;		/* String pointer */
    char		*malloc();	/* Storage allocator */

    if((t = malloc(sizeof(STRINGNODE) + n + (n & 1))) == NULL)
      {					/* If allocation fails */
	fprintf(stderr,"Out of memory\n");
	DOSEXIT(1, 2);			      /* Print error message and die */
      }
    if(n & 1) ++t;			/* END of string word-aligned */
    strncpy(t,s,n);			/* Copy string text */
    new = (STRINGNODE *)(t + n);	/* Set pointer to node */
    new->s_alt = NULL;			/* No alternates yet */
    new->s_suf = NULL;			/* No suffixes yet */
    new->s_must = n;			/* Set string length */
    return(new);			/* Return pointer to new node */
  }


void			reallocnode(node,s,n)
register STRINGNODE	*node;		/* Pointer to node */
char			*s;		/* String */
register int		n;		/* Length of string */
  {
    if(n > node->s_must)		/* If node must grow */
      {
	fprintf(stderr,"Internal error\n");
					/* Error message */
	DOSEXIT(1, 2);			      /* Error exit */
      }
    node->s_must = n;			/* Set new length */
    memcpy(s_text(node),s,n);		/* Copy new text */
  }


void			addstring(s,n)
char			*s;		/* String to add */
int			n;		/* Length of string */
  {
    register STRINGNODE	*cur;		/* Current string */
    register STRINGNODE	**pprev;	/* Pointer to previous link */
    STRINGNODE		*new;		/* New string */
    int			i;		/* Index */
    int			j;		/* Count */
    int			k;		/* Count */

    if(n <= 0 || n > 127) return;	/* Should never happen */
    i = transtab[*s];			/* Get current index */
    if(i == 0)				/* If no existing list */
      {
	/*
	 *  We have to start a new list
	 */
	if((i = clists++) >= TRTABLEN/2)
	  {				/* If too many string lists */
	    fprintf(stderr,"Too many string lists\n");
					/* Error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
	stringlist[i] = NULL;		/* Initialize */
	transtab[*s] = i;		/* Set pointer to new list */
	if(!casesen && isalpha(*s)) transtab[*s ^ '\x20'] = i;
					/* Set pointer for other case */
      }
    else if(stringlist[i] == NULL) return;
					/* Check for existing 1-byte string */
    if(--n == 0)			/* If 1-byte string */
      {
	freenode(stringlist[i]);	/* Free any existing stuff */
	stringlist[i] = NULL;		/* No record here */
	return;				/* Done */
      }
    ++s;				/* Skip first char */
    pprev = stringlist + i;		/* Get pointer to link */
    cur = *pprev;			/* Get pointer to node */
    while(cur != NULL)			/* Loop to traverse match tree */
      {
	i = (n > cur->s_must)? cur->s_must: n;
					/* Find minimum of string lengths */
	matchstrings(s,s_text(cur),i,&j,&k);
					/* Compare the strings */
	if(j == 0)			/* If complete mismatch */
	  {
	    if(k < 0) break;		/* Break if insertion point found */
	    pprev = &(cur->s_alt);	/* Get pointer to alternate link */
	    cur = *pprev;		/* Follow the link */
	  }
	else if(i == j)			/* Else if strings matched */
	  {
	    if(i == n)			/* If new is prefix of current */
	      {
		reallocnode(cur,s_text(cur),n);
					/* Shorten text of node */
		if(cur->s_suf != NULL)	/* If there are suffixes */
		  {
		    freenode(cur->s_suf);
					/* Suffixes no longer needed */
		    cur->s_suf = NULL;
		  }
		return;			/* All done */
	      }
	    pprev = &(cur->s_suf);	/* Get pointer to suffix link */
	    if((cur = *pprev) == NULL) return;
					/* Done if current is prefix of new */
	    s += i;			/* Skip matched portion */
	    n -= i;
	  }
	else				/* Else partial match */
	  {
	    /*
	     *	We must split an existing node.
	     *	This is the trickiest case.
	     */
	    new = newnode(s_text(cur) + j,cur->s_must - j);
					/* Unmatched part of current string */
	    reallocnode(cur,s_text(cur),j);
					/* Set length to matched portion */
	    new->s_suf = cur->s_suf;	/* Current string's suffixes */
	    if(k < 0)			/* If new preceded current */
	      {
		cur->s_suf = newnode(s + j,n - j);
					/* FIrst suffix is new string */
		cur->s_suf->s_alt = new;/* Alternate is part of current */
	      }
	    else			/* Else new followed current */
	      {
		new->s_alt = newnode(s + j,n - j);
					/* Unmatched new string is alternate */
	        cur->s_suf = new;	/* New suffix list */
	      }
	    return;
	  }
      }
    *pprev = newnode(s,n);		/* Set pointer to new node */
    (*pprev)->s_alt = cur;		/* Attach alternates */
  }


int			addfancy(buffer,buflen,seplist)
register char		*buffer;	/* Buffer */
int			buflen;		/* Length of buffer */
char			*seplist;	/* List of separators */
  {
    register char	*bufend;	/* Pointer to end of buffer */
    int			strcnt = 0;	/* String count */
    int			len;		/* String length */
    char		c;		/* One char buffer */

    bufend = buffer + buflen;		/* Set end pointer */
    while(buffer < bufend)		/* Loop through all strings */
      {
	len = strncspn(buffer,seplist,bufend - buffer);
					/* Length of string */
	if(flags & ENDLINE)		/* If match must be at end of line */
	  {
	    c = buffer[len];		/* Save 1st character past string */
	    buffer[len++] = '\r';	/* Carriage return marks end of line */
	  }
	if(findlist(buffer,buffer + len) == NULL)
	  {				/* If no match within string */
	    addstring(buffer,len);	/* Add string to list */
	    if(strcnt++ == 0)		/* If first string */
	      {
		memcpy(target,buffer,len);
					/* Save first string in buffer */
		targetlen = len;	/* Remember length */
	      }
	  }
	buffer += len;			/* Skip over string */
	if(flags & ENDLINE) (--buffer)[0] = c;
					/* Restore saved character */
	buffer += strnspn(buffer,seplist,bufend - buffer);
					/* Skip over trailing separators */
      }
    return(strcnt);			/* Return string count */
  }


int			addplain(buffer,buflen,seplist)
register char		*buffer;	/* String list buffer */
int			buflen;		/* Buffer length */
char			*seplist;	/* List of separators */
  {
    int			strcnt;		/* String count */
    register int	len;		/* String length */
    char		c;		/* One char buffer */

    strcnt = 0;
    while((len = strncspn(buffer,seplist,buflen)) > 0)
      {					/* While not at end of input list */
	if(flags & ENDLINE)		/* If match must be at end of line */
	  {
	    c = buffer[len];		/* Save 1st character past string */
	    buffer[len++] = '\r';	/* Carriage return marks end of line */
	  }
	if(strcnt == 0)			/* Save first string */
	  {
	    strncpy(target,buffer,len);	/* Save string in buffer */
	    targetlen = len;		/* Save string length */
	  }
	addstring(buffer,len);		/* Add the string to the table */
	if(flags & ENDLINE) buffer[--len] = c;
					/* Restore saved character */
	buffer += len;			/* Skip the string */
	buflen -= len;
	len = strnspn(buffer,seplist,buflen);
					/* Skip separators */
	buffer += len;
	buflen -= len;
	++strcnt;			/* Increment string count */
      }
    return(strcnt);			/* Return string count */
  }


void			dumplist(node,indent)
register STRINGNODE	*node;		/* Pointer to list to dump */
int			indent;		/* Current length of buffer */
  {
    int			i;		/* Counter */

    while(node != NULL)			/* While not at end of list */
      {
	for(i = 0; i < indent; ++i) fputc(' ',stderr);
	fwrite(s_text(node),sizeof(char),node->s_must,stderr);
	fprintf(stderr,"\n");
	if(node->s_suf != NULL)
	  dumplist(node->s_suf,indent + node->s_must);
					/* Recurse to do suffixes */
	node = node->s_alt;		/* Do next alternate in list */
      }
  }


void			dumpstrings()
  {
    int			i;		/* Index */

    for(i = 0; i < TRTABLEN; ++i)	/* Loop through translation table */
      {
	if(transtab[i] == 0) continue;	/* Skip null entries */
	fprintf(stderr,"%c\n",i);	/* Print the first byte */
	dumplist(stringlist[transtab[i]],1);
					/* Dump the list */
      }
  }


int			openfile(name)
char			*name;		/* File name */
  {
    int			fd;		/* File descriptor */

    if((fd = open(name,0)) == -1)	/* If error opening file */
      {
	fprintf(stderr,"Cannot open %s\r\n",name);
					/* Print error message */
      }
    return(fd);				/* Return file descriptor */
  }


void far		thread2()	/* Read thread */
  {
    while(DOSSEMREQUEST((long far *) &readpending,-1L) == 0)
      {					/* While there is work to do */
	arrc = DOSREAD(t2fd,(char far *) t2buf,t2buflen,(int far *) &cbread);
					/* Do the read */
	DOSSEMCLEAR((long far *) &readdone);
					/* Signal read completed */
      }
    fprintf(stderr,"Thread 2: DOSSEMREQUEST failed\n");
					/* Print error message */
    DOSEXIT(1, 2);			      /* Die */
  }


void far		thread3()	/* Write thread */
  {
    while(DOSSEMREQUEST((long far *) &writepending,-1L) == 0)
      {					/* While there is work to do */
	awrc = DOSWRITE(t3fd,(char far *) t3buf,t3buflen,(int far *) &cbwrite);
					/* Do the write */
	DOSSEMCLEAR((long far *) &writedone);
					/* Signal write completed */
      }
    fprintf(stderr,"Thread 3: DOSSEMREQUEST failed\n");
					/* Print error message */
    DOSEXIT(1, 2);			      /* Die */
  }


void			startread(fd,buffer,buflen)
int			fd;		/* File handle */
char			*buffer;	/* Buffer */
int			buflen;		/* Buffer length */
  {
    if(pmode)				/* If protected mode */
      {
	if(DOSSEMREQUEST((long far *) &readdone,-1L) != 0)
	  {				/* If we fail to get the semaphore */
	    fprintf(stderr,"DOSSEMREQUEST failed\n");
					/* Error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
	t2fd = fd;			/* Set parameters for read */
	t2buf = buffer;
	t2buflen = buflen;
	DOSSEMCLEAR((long far *) &readpending);
					/* Wake thread 2 for read */
	DOSSLEEP(0L);			/* Yield the CPU */
      }
    else arrc = DOSREAD(fd,(char far *) buffer,buflen,(int far *) &cbread);
  }


int			finishread()
  {
    if(pmode && DOSSEMWAIT((long far *) &readdone,-1L) != 0)
      {					/* If protected mode and wait fails */
	fprintf(stderr,"DOSSEMWAIT failed\n");
					/* Print error message */
	DOSEXIT(1, 2);			      /* Die */
      }
    return((arrc == 0)? cbread: -1);	/* Return number of bytes read */
  }


void			startwrite(fd,buffer,buflen)
int			fd;		/* File handle */
char			*buffer;	/* Buffer */
int			buflen;		/* Buffer length */
  {
    if(pmode)				/* If protected mode */
      {
	if(DOSSEMREQUEST((long far *) &writedone,-1L) != 0)
	  {				/* If we fail to get the semaphore */
	    fprintf(stderr,"DOSSEMREQUEST failed\n");
					/* Error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
	t3fd = fd;			/* Set parameters for write */
	t3buf = buffer;
	t3buflen = buflen;
	DOSSEMCLEAR((long far *) &writepending);
					/* Wake thread 3 for read */
	DOSSLEEP(0L);			/* Yield the CPU */
      }
    else awrc = DOSWRITE(fd,(char far *) buffer,buflen,(int far *) &cbwrite);
  }


int			finishwrite()
  {
    if(pmode && DOSSEMWAIT((long far *) &writedone,-1L) != 0)
      {					/* If protected mode and wait fails */
	fprintf(stderr,"DOSSEMWAIT failed\n");
					/* Print error message */
	DOSEXIT(1, 2);			      /* Die */
      }
    return((awrc == 0)? cbwrite: -1);	/* Return number of bytes written */
  }


void			write1nobuf(buffer,buflen)
char			*buffer;	/* Buffer */
register int		buflen;		/* Buffer length */
  {
    int			cb;		/* Count of bytes written */

    if(DOSWRITE(1,(char far *) buffer,buflen,(int far *) &cb) != 0 ||
      cb != buflen)			/* If write fails */
      {
	fprintf(stderr,"write error %d\n",awrc);
					/* Print error message */
	DOSEXIT(1, 2);			      /* Die */
      }
  }


void			write1buf(buffer,buflen)
char			*buffer;	/* Buffer */
register int		buflen;		/* Buffer length */
  {
    register int	cb;		/* Byte count */

    while(buflen > 0)			/* While bytes remain */
      {
	if(awrc != 0)			/* If previous write failed */
	  {
	    fprintf(stderr,"write error %d\n",awrc);
					/* Print error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
	if((cb = ocnt[oi]) == 0)	/* If buffer full */
	  {
	    startwrite(1,obuf[oi],OUTBUFLEN);
					/* Write the buffer */
	    ocnt[oi] = OUTBUFLEN;	/* Reset count and pointer */
	    optr[oi] = obuf[oi];
	    oi ^= 1;			/* Switch buffers */
	    cb = ocnt[oi];		/* Get space remaining */
	  }
	if(cb > buflen) cb = buflen;	/* Get minimum */
	memcpy(optr[oi],buffer,cb);	/* Copy bytes to buffer */
	ocnt[oi] -= cb;			/* Update buffer length and pointers */
	optr[oi] += cb;
	buflen -= cb;
	buffer += cb;
      }
  }


void			flush1nobuf()
  {
  }


void			flush1buf()
  {
    int			cb;		/* Byte count */

    if((cb = OUTBUFLEN - ocnt[oi]) > 0)	/* If buffer not empty */
      {
	startwrite(1,obuf[oi],cb);	/* Start write */
	if(finishwrite() != cb)		/* If write failed */
	  {
	    fprintf(stderr,"write error %d\n",awrc);
					/* Print error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
      }
  }


int			grepnull(cp,endbuf,name)
register char		*cp;		/* Buffer pointer */
char			*endbuf;	/* End of buffer */
char			*name;		/* File name */
  {
    return(0);				/* Do nothing */
  }


int			grepbuffer(startbuf,endbuf,name)
char			*startbuf;	/* Start of buffer */
char			*endbuf;	/* End of buffer */
char			*name;		/* File name */
  {
    register char	*cp;		/* Buffer pointer */
    char		*lastmatch;	/* Last matching line */
    int			linelen;	/* Line length */
    int			namlen = 0;	/* Length of name */
    char		lnobuf[LNOLEN];	/* Line number buffer */
    char		nambuf[LINELEN];/* Name buffer */

    cp = startbuf;			/* Initialize to start of buffer */
    lastmatch = cp;			/* No previous match yet */
    while((cp = (*find)(cp,endbuf)) != NULL)
      {					/* While matches are found */
	if((flags & BEGLINE) && cp[-1] != '\n' && cp > startbuf)
	  {				/* If begin line conditions not met */
	    ++cp;			/* Skip first char of match */
	    continue;			/* Keep looking */
	  }
	status = 0;			/* Match found */
	if(flags & NAMEONLY)		/* If filename only wanted */
	  {
	    (*write1)(nambuf,sprintf(nambuf,"%s\r\n",name));
					/* Print the name */
	    return(1);			/* Punt remainder of buffer */
	  }
	cp -= revfind(cp,'\n',cp - startbuf);
					/* Point at last linefeed */
	if(*cp == '\n') ++cp;		/* Point at start of line */
	if(flags & SHOWNAME)		/* If name wanted */
	  {
	    if(namlen == 0) namlen = sprintf(nambuf,"%s:",name);
					/* Format name if not done already */
	    (*write1)(nambuf,namlen);	/* Show name */
	  }
	if(flags & LINENOS)		/* If line number wanted */
	  {
	    lineno += countlines(lastmatch,cp);
					/* Count lines since last match */
	    (*write1)(lnobuf,sprintf(lnobuf,"%d:",lineno));
					/* Print line number */
	    lastmatch = cp;		/* New last match */
	  }
	linelen = strncspn(cp,"\n",endbuf - cp) + 1;
					/* Calculate line length */
	(*write1)(cp,linelen);		/* Print the line */
	cp += linelen;			/* Skip the line */
      }
    if(flags & LINENOS) lineno += countlines(lastmatch,endbuf);
					/* Count remaining lines in buffer */
    return(0);				/* Keep searching */
  }


void			showv(name,lastmatch,thismatch)
char			*name;
register char		*lastmatch;
char			*thismatch;
  {
    register int	linelen;
    int			namlen = 0;	/* Length of name */
    char		lnobuf[LNOLEN];	/* Line number buffer */
    char		nambuf[LINELEN];/* Name buffer */

    if(flags & (SHOWNAME | LINENOS))
      {
	while(lastmatch < thismatch)
	  {
	    if(flags & SHOWNAME)	/* If name wanted */
	      {
		if(namlen == 0) namlen = sprintf(nambuf,"%s:",name);
					/* Format name if not done already */
		(*write1)(nambuf,namlen);
					/* Write the name */
	      }
	    if(flags & LINENOS)
	      {
		(*write1)(lnobuf,sprintf(lnobuf,"%d:",lineno++));
	      }
	    linelen = strncspn(lastmatch,"\n",thismatch - lastmatch) + 1;
	    (*write1)(lastmatch,linelen);
	    lastmatch += linelen;
	  }
      }
    else (*write1)(lastmatch,thismatch - lastmatch);
  }


int			grepvbuffer(startbuf,endbuf,name)
char			*startbuf;	/* Start of buffer */
char			*endbuf;	/* End of buffer */
char			*name;		/* File name */
  {
    register char	*cp;		/* Buffer pointer */
    register char	*lastmatch;	/* Pointer to line after last match */

    cp = startbuf;			/* Initialize to start of buffer */
    lastmatch = cp;
    while((cp = (*find)(cp,endbuf)) != NULL)
      {
	if((flags & BEGLINE) && cp[-1] != '\n' && cp > startbuf)
	  {				/* If begin line conditions not met */
	    ++cp;			/* Skip first char of match */
	    continue;			/* Keep looking */
	  }
	status = 1;			/* Match found */
	if(flags & NAMEONLY) return(1);	/* Skip rest of file if NAMEONLY */
	cp -= revfind(cp,'\n',cp - startbuf);
					/* Point at last linefeed */
	if(*cp == '\n') ++cp;		/* Point at start of line */
	showv(name,lastmatch,cp);	/* Show from last match to this */
	cp += strncspn(cp,"\n",endbuf - cp) + 1;
					/* Skip over line with match */
	lastmatch = cp;			/* New "last" match */
	++lineno;			/* Increment line count */
      }
    if(!(flags & NAMEONLY)) showv(name,lastmatch,endbuf);
					/* Show buffer tail if not NAMEONLY */
    return(0);				/* Keep searching file */
  }


void			qgrep(name,fd)
char			*name;		/* File name */
int			fd;		/* File descriptor */
  {
    register int	cb;		/* Byte count */
    register char	*cp;		/* Buffer pointer */
    char		*endbuf;	/* End of buffer */
    int			taillen;	/* Length of buffer tail */
    int			bufi;		/* Buffer index */
    char		line[LINELEN];	/* Line buffer */

    lineno = 1;				/* File starts on line 1 */
    taillen = 0;			/* No buffer tail yet */
    bufi = 0;				/* Initialize buffer index */
    cp = bufptr[0];			/* Initialize to start of buffer */
    finishread();			/* Make sure no I/O activity */
    arrc = DOSREAD(fd,(char far *) cp,FILBUFLEN,(int far *) &cbread);
					/* Do first read synchronously */
    while((cb = finishread()) + taillen > 0)
      {					/* While search incomplete */
	if(cb == 0)			/* If buffer tail is all that's left */
	  {
	    taillen = 0;		/* Set tail length to zero */
	    *cp++ = '\r';		/* Add end of line sequence */
	    *cp++ = '\n';
	    endbuf = cp;		/* Note end of buffer */
	  }
	else				/* Else start next read */
	  {
	    taillen = revfind(cp + cb - 1,'\n',cb);
					/* Find length of partial line */
	    endbuf = cp + cb - taillen;	/* Get pointer to end of buffer */
	    cp = bufptr[bufi ^ 1];	/* Pointer to other buffer */
	    memcpy(cp,endbuf,taillen);	/* Copy tail to head of other buffer */
	    cp += taillen;		/* Skip over tail */
	    startread(fd,cp,(FILBUFLEN - taillen) & (~0 << LG2SECLEN));
					/* Start next read */
	  }
	if((*grep)(bufptr[bufi],endbuf,name)) return;
					/* Done if NAMEONLY and match found */
	bufi ^= 1;			/* Switch buffers */
      }
    if((flags & (NAMEONLY | INVERT)) == (NAMEONLY | INVERT))
      (*write1)(line,sprintf(line,"%s\r\n",name));
					/* Write name if -lv */
  }


void			usage(verbose)
int			verbose;	/* Verbose message flag */
  {
    static char		*opts[] =
      {
	"-? - print this message",
	"-B - match pattern if at beginning of line",
	"-E - match pattern if at end of line",
	"-l - print only file name if file contains match",
	"-n - print line number before each matching line",
	"-v - print only lines not containing a match",
	"-x - print lines that match exactly (-BE)",
	"-y - treat upper and lower case as equivalent",
	"-e - treat next argument as the search string",
	"-f - read search strings from file named by next argument",
	"-i - read file list from file named by next argument",
	0
      };
    register char	**opt = opts;	/* Option list */

    fprintf(stderr,"usage: CPGREP [-?BElnvxy][-e][-f <file>][-i <file>][<strings>][<files>]\n");
    if(verbose)				/* If verbose message wanted */
      {
	while(*opt != 0) fprintf(stderr,"%s\n",*opt++);
					/* Print option list */
      }
    DOSEXIT(1, 2);			      /* Error exit */
  }


void			main(argc,argv)
int			argc;
char			**argv;
  {
    register char	*cp;
    int			fd;
    FILE		*fi;
    char		filnam[FILNAMLEN];
    int			i;
    char		*inpfile = NULL;
    int			j;
    char		*seplist = " \t";
    int			strcnt;
    char		*strfile = NULL;
    long		start;		/* Start time */
    int			(*add)();
    int			t2stk[STKLEN];	/* Read thread stack */
    int			t3stk[STKLEN];	/* Write thread stack */
    long		time();		/* Time and date in seconds */

    DOSGETMACHINEMODE((char far *) &pmode);
    flags = 0;
    for(i = 1; i < argc && argv[i][0] == '-'; ++i)
      {
	switch(argv[i][1])
	  {
	    case 'f':
	    case 'i':
	      if(i == argc - 1)
		{
		  fprintf(stderr,"File name missing after -%c\n",argv[i][1]);
		  DOSEXIT(1, 2);
		}
	      if(argv[i++][1] == 'i') inpfile = argv[i];
	      else strfile = argv[i];
	      break;

	    case '?':
	    case 'B':
	    case 'E':
	    case 'N':
	    case 'S':
	    case 'd':
	    case 'l':
	    case 'n':
	    case 't':
	    case 'v':
	    case 'x':
	    case 'y':
	      for(cp = &argv[i][1]; *cp != '\0'; ++cp)
		{
		  switch(*cp)
		    {
		      case '?':
			usage(1);	/* Verbose usage message */

		      case 'B':
			flags |= BEGLINE;
			break;

		      case 'E':
			flags |= ENDLINE;
			break;

		      case 'N':
			grep = grepnull;
			break;

		      case 'S':
			pmode = 0;	/* Force synchronous I/O */
			break;

		      case 'd':
			flags |= DEBUG;
			break;

		      case 'l':
			flags |= NAMEONLY;
			break;

		      case 'n':
			flags |= LINENOS;
			break;

		      case 't':
			flags |= TIMER;
			break;

		      case 'v':
			status = 0;	/* Assume success */
			flags |= INVERT;
			grep = grepvbuffer;
			break;

		      case 'x':
			flags |= BEGLINE | ENDLINE;
			break;

		      case 'y':
			casesen = 0;
			break;

		      default:
			fprintf(stderr,"-%c ignored\n",*cp);
			break;
		    }
	        }
	      break;

	    case 'e':
	      if(strfile == NULL)
		{
		  ++i;
		  seplist = "";		/* Allow anything in string */
		  goto endfor0;
		}
	      /* Drop through to "default" */

	    default:
	      fprintf(stderr,"%s ignored\n",argv[i]);
	      break;
	  }
      }
    endfor0:

    if(i == argc && strfile == NULL) usage(0);
					/* Simple usage message if arg error */
    if(flags & TIMER) start = time(NULL);
					/* Get start time if timer on */
    if(pmode)				/* Initialize semaphores and threads */
      {
	DOSSEMCLEAR((long far *) &readdone);
	DOSSEMCLEAR((long far *) &writedone);
	DOSSEMSET((long far *) &readpending);
	DOSSEMSET((long far *) &writepending);
	if(DOSCREATETHREAD(thread2,(int far *) &fd,
	  (int far *) t2stk + STKLEN) != 0 ||
	  DOSCREATETHREAD(thread3,(int far *) &fd,
	  (int far *) t3stk + STKLEN) != 0)
	  {				/* If thread creation fails */
	    fprintf(stderr,"Failed to create child threads\n");
					/* Print error message */
	    DOSEXIT(1, 2);		      /* Die */
	  }
      }
    setmode(fileno(stdout),O_BINARY);
    add = addplain;			/* Assume plain string adds */
    if(strfile != NULL)			/* If strings from file */
      {
	if(!(flags & BEGLINE)) add = addfancy;
					/* Try to add intelligently */
	if((fd = open(strfile,0)) == -1)
	  {				/* If open fails */
	    fprintf(stderr,"Cannot read strings from %s\n",strfile);
	    DOSEXIT(1, 2);		      /* Print message and die */
	  }
	for(cp = filbuf, j = 0; (j = read(fd,cp,FILBUFLEN*2 - j)) > 0; cp += j);
					/* Read strings file into buffer */
	j = cp - filbuf;		/* Get total length of buffer */
	close(fd);			/* Close strings file */
	filbuf[j] = '\0';		/* Null-terminate the buffer */
	cp = filbuf;			/* Set pointer to string list */
	seplist = "\r\n";		/* Only '\r' and '\n' are separators */
      }
    else				/* Else strings on command line */
      {
	cp = argv[i++];			/* Set pointer to strings */
	j = strlen(cp);			/* Get length of strings */
      }
    if((strcnt = (*add)(cp,j,seplist)) == 0)
      {					/* If no strings */
	fprintf(stderr,"No search strings\n");
	DOSEXIT(1, 2);			      /* Print error message and die */
      }

    /*
     *  Check type of handle for std. out.
     */
    if(DOSQHANDTYPE(fileno(stdout),(int far *) &j,(int far *) &fd) != 0)
      {					/* If error */
	fprintf(stderr,"Standard output bad handle\n");
					/* Print error message */
	DOSEXIT(1, 2);			      /* Die */
      }
    if(j != 0 && (fd & ISCOT))		/* If handle is console output */
      {
	write1 = write1nobuf;		/* Use unbuffered output */
	flush1 = flush1nobuf;
      }

    if(strcnt > 1)			/* If more than one string */
      {
	if(flags & DEBUG)		/* Print debug info maybe */
	  {
	    fprintf(stderr,"Here are the strings:\n");
	    dumpstrings();
	  }
      }
    else if(casesen) find = findone;	/* Else use findone() */
    if(inpfile != NULL)			/* If file list from file */
      {
	flags |= SHOWNAME;		/* Always show name of file */
	if((fi = fopen(inpfile,"r")) == NULL)
	  {				/* If open fails */
	    fprintf(stderr,"Cannot read file list from %s\r\n",inpfile);
					/* Error message */
	    DOSEXIT(1, 2);		      /* Error exit */
	  }
	while(fgets(filnam,FILNAMLEN,fi) != NULL)
	  {				/* While there are names */
	    filnam[strcspn(filnam,"\r\n")] = '\0';
					/* Null-terminate the name */
	    if((fd = openfile(filnam)) == -1) continue;
					/* Skip file if it cannot be opened */
	    qgrep(filnam,fd);		/* Do the work */
	    close(fd);			/* Close the file */
	  }
	fclose(fi);			/* Close the list file */
      }
    else if(i == argc)
      {
	flags &= ~(NAMEONLY | SHOWNAME);
	setmode(fileno(stdin),O_BINARY);
	qgrep(NULL,fileno(stdin));
      }
    if(argc > i + 1) flags |= SHOWNAME;
    for(; i < argc; ++i)
      {
	if((fd = openfile(argv[i])) == -1) continue;
	qgrep(argv[i],fd);
	close(fd);
      }
    (*flush1)();
    if(flags & TIMER) fprintf(stderr,"%ld seconds\n",time(NULL) - start);
					/* Print elapsed time if timer on */
    DOSEXIT(1, status);
  }
