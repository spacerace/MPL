/*   This is a set of 3 general functions that will perform random            *
*    file access.  `ropen()' opens a random file or creats it if it           *
*    doesn't exist.  `hirec()' returns the high or last record in a           *
*    file.  `rfile()' preforms the actual read/write operations.              *
*    YOU MUST `close(port)' A FILE WHEN YOU ARE DONE.                         *
*    EACH RECORD is a STRUCTURE of size `sz', that is `sz=sizeof(*data);'.    *
*    `data' is the pointer to the record structure that has been              *
*    loaded prior to a read, or where data is written into.                   *
*    `rfiel()' returns the number of the record acted upon, should be         *
*    the one you called UNLESS you called a recored number higher than        *
*    the last record, then the actual record number is returned.              *
*    See the comments in the text for more info.                              *
*/

/*   RFILE.C  Random file access functions  */

#include "ALL.H"    /* #defines O_RDWR = 2, O_CREAT = 0x0100 (hex)  */

int ropen(filspc)   /*  open file for random access  */

char *filspc;

{ int port;

  port=open(filspc,O_RDWR | O_CREAT);
  return(port);  /*  port<0 = error  */
}


int rfile(port,rec,data,sz,mode)   /*  random file access         *
                                   *   assumes port assigned     */
int port,rec;
int sz;          /*  sizeof(structure)  */
char *data;      /*  random record structure  */
char mode;       /*  r = read, w = write  */

{ int stat,read(),write(),lrec,hirec();
  long pos,lseek();

  stat=0;
  lrec=hirec(port,sz);
  if(rec>lrec)   {  if(mode=='w')  rec=lrec+1;
                    else rec=lrec;
                 };
  rec-=1;
  pos=rec*sz;
  lseek(port,pos,0);
  switch(mode)   {  case'r':stat=read(port,data,sz);
                            break;
                    case'w':stat=write(port,data,sz);
                            break;
                 };
  if(stat<=0) return(stat-1);   /* -1=EOF, <(-1)=error */
  else return(rec+1);
}


int hirec(port,sz)   /*  random record count  */

int port;
int sz;    /*  sizeof(structure)  */

{ int ct;
  long ef,lseek();

  ef=lseek(port,0L,2);
  ct=ef/sz;
  return(ct);
}
