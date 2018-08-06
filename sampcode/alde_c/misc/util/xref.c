/*
TITLE: XREF - Cross Reference;
VERSION: 1.0;
DATE: 07/31/1986;
DESCRIPTION:
 "Cross reference text from stdin to stdout.
 Optimized name storage allocation.";
KEYWORDS: binary tree, name, reference, symbol, xref;
SYSTEM: CP/M, any;
FILENAME: XREF.C;
WARNINGS: "Assumes 16-bit pointers.";
SEE-ALSO: none;
AUTHORS: Mark Antony Washburn;
COMPILERS: Small-C, any C compiler; 
REFERENCES:
 AUTHORS: Nickolas Wirth;
 TITLE: "Algorithms+Data Structures=Programs";
 CITATION: "p.206"
ENDREF;
*/

#include <A:STDIO.H>  /* find STDIO.H */

#define MEMOVERHEAD (1024)  /* leave room for stack */

#define MAXWORDLEN 79  /* maximum name length - 1 */
#define MAXWORDBUF 80  /* maximum name length */ 
#define NUMPERLINE 14  /* assumes 80 columns */
#define MAXLINENUM 9999  /* maximum number of source lines */

#define DNLEFT 0  /* name node structure definitions */ 
#define DNRIGHT 2
#define DNFIRST 4
#define DNLAST 6
#define DNID 8

#define DRLINE 0  /* reference link list structure definitions */
#define DRNEXT 2
#define DRLN 4

char *membtm,*memtop;  /* pointers to bottom and top of memory */

char *root;  /* root node pointer */
char *nword;  /* global name node pointer */ 
/* left and right node pointers, first and last reference links */
int *nleft,*nright,*nfirst,*nlast;
int *rline,*rnext;  /* global pointers reference instance */

int c;  /* current char */
int k;  /* current word index */
int n;  /* current line number */
char id[MAXWORDBUF];  /* current word */

/*
**  ouput character to stdout, then fetch next character
**  from stdin
*/
outcinc() {
  putchar(c);
  c = getchar(c);
  }

/*
** output a string to stdout
*/
outstr(l) char *l; {
  fputs(l, stdout);
  }

/*
**  output a right justified integer to stdout
*/
outdec(i) int i; {
  char l[5];
  itod(i, l, 5);
  fputs(l, stdout);
  }

/*
**  get as much memory as possible for symbol tree
*/
initmem() {
  int max;
  max = avail(YES);
  max -= MEMOVERHEAD;
  membtm = malloc(max);
  memtop = membtm + max;
  }

/* 
**  get memory for next entry, otherwise abort
*/
getmem(i) int i; {
  char *memp;
  if(memtop > membtm + i)  {
    memp = membtm;
    membtm += i;
    return(memp);
    }
  else {
    fputs("Not enough memory", stderr);
    abort(2);
    }
  }

/*
**  assign node pointers
*/
nodeptrs(w) char *w; {
  nleft = w;
  nright = w + DNRIGHT;
  nfirst = w + DNFIRST;
  nlast = w + DNLAST;
  nword = w + DNID;
  }

/*
**  assign reference pointers
*/
refptrs(x) char *x; {
  rline = x;
  rnext = x + DRNEXT;
  }

/*
**  if id is a new node then insert word and reference 
**  otherwise insert next reference
*/
insert(w) char *w; {
  int *x, *lnode;
  int i;
  if(w == NULL) {
    w = getmem(DNID + k);
    nodeptrs(w);
    strcpy(nword, id);
    *nleft = NULL;
    *nright = NULL;
    refptrs(getmem(DRLN));
    *rline = n;
    *rnext = NULL;
    *nfirst = rline;
    *nlast = rline;
    if(root == NULL) root = w;
    else return(w);
    }
  else {
    nodeptrs(w);
    i = strcmp(id, nword);
    if(i == 0) {
      x = getmem(DRLN);
      refptrs(x);
      *rline = n;
      *rnext = NULL;
      refptrs(*nlast);
      *rnext = x;
      *nlast = x;
      }
    else if(i < 0) {
      lnode = nleft;
      x = insert(*nleft);
      if(x != NULL) *lnode = x;
      }
    else {
      lnode = nright;
      x = insert(*nright);
      if(x != NULL) *lnode = x;
      }
    }
    return(NULL);
  }

/*
** print a word with its references 
*/
pword(w) char *w; {
  int l;
  int *t2;
  outstr("\n\n");
  outstr(w + DNID);
  outstr("\n");
  t2 = w + DNFIRST;
  t2 = *t2;
  l = 0;
  do {
    if(l == NUMPERLINE) {
      outstr("\n");
      l = 0;
      }
    ++l;
    outstr(" ");
    outdec(*t2);
    ++t2;
    t2 = *t2;
    } while (t2 != NULL);
  }

/*
**  print the cross reference tree
*/ 
ptree(w) char *w; {
  int *wl, *wr;
  if(w != NULL) {
    wl = w + DNLEFT;
    wr = w + DNRIGHT;
    ptree(*wl);
    pword(w);
    ptree(*wr);
    }
  }

main() {
  char l2[5];
  initmem();
  root = NULL;
  n = 0;
  c = getchar();
  while(c != EOF) {
    ++n;
    if(n > MAXLINENUM) n = 0;
    outdec(n);
    outstr(": ");
    while(c != '\n') {
      if(isalpha(c)) {
        k = 0;
        do {
          if(k < MAXWORDLEN) id[k++] = c;
          outcinc();
          } while(isalnum(c));
        id[k++] = '\0';
        insert(root);
        }
      else outcinc();
      if(c == EOF) break;
      }
    if(c != EOF) outcinc();
    }
  ptree(root);
  }
