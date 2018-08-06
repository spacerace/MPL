/********************************************************************/
/*                                                                  */
/*             BPLUS file indexing program - Version 1.1            */
/*                                                                  */
/*                      A "shareware program"                       */
/*                                                                  */
/*                                                                  */
/*                      Copyright (C) 1987 by                       */
/*                                                                  */
/*                      Hunter and Associates                       */
/*                      7050 NW Zinfandel Lane                      */
/*                      Corvallis, Oregon  97330                    */
/*                      (503) 745 - 7186                            */
/*                                                                  */
/********************************************************************/


#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>            /*  delete this line for Turbo C  */
#include <sys\stat.h>
#include <string.h>
#include "bplus.h"


/*  macros, constants, data types  */

#define  NULLREC      (-1L)
#define  FREE_BLOCK   (-2)

#define  ENT_ADR(pb,off)  ((ENTRY*)((char*)((pb)->entries) + off))
#define  ENT_SIZE(pe)     strlen((pe)->key) + 1 + 2 * sizeof(RECPOS)
#define  BUFDIRTY(j)      (mci->cache[j].dirty)
#define  BUFHANDLE(j)     (mci->cache[j].handle)
#define  BUFBLOCK(j)      (mci->cache[j].mb)
#define  BUFCOUNT(j)      (mci->cache[j].count)
#define  CB(j)            (pci->pos[j].cblock)
#define  CO(j)            (pci->pos[j].coffset)


                    /*    ATTENTION MICROSOFT USERS   */
                    /* Microsoft changed the memcpy   */
                    /* library routine in the C 5.0   */
                    /* compiler.  The following macro */
                    /* must be used for Microsoft 5.0 */
                    /* and the Quick C compiler.      */

/* #define memcpy            memmove */

/*  declare some global variables  */

IX_DESC      *pci;
IX_BUFFER    bt_buffer;
IX_BUFFER    *mci = &bt_buffer;
BLOCK        *block_ptr;
BLOCK        *spare_block;
int          cache_ptr = 0;
int          cache_init = 0;
int          split_size = IXB_SPACE;
int          comb_size  = (IXB_SPACE/2);

void pascal error(int, long);
void pascal read_if(long, char *, int);
void pascal write_if(int, long, char *, int);
int  pascal creat_if(char *);
int  pascal open_if(char *);
void pascal close_if(int);
void pascal update_block(void);
void pascal init_cache(void);
int  pascal find_cache(RECPOS);
int  pascal new_cache(void);
void pascal load_cache(RECPOS);
void pascal get_cache(RECPOS);
void pascal retrieve_block(int, RECPOS);
int  pascal prev_entry(int);
int  pascal next_entry(int);
int  pascal copy_entry(ENTRY *, ENTRY *);
int  pascal scan_blk(int);
int  pascal last_entry(void);
int  pascal write_free( RECPOS, BLOCK *);
RECPOS pascal get_free(void);
int  pascal find_block(ENTRY *, int *);
void pascal movedown(BLOCK *, int, int);
void pascal moveup(BLOCK *, int, int);
void pascal ins_block(BLOCK *, ENTRY *, int);
void pascal del_block(BLOCK *, int);
int  pascal split(int, ENTRY *, ENTRY *);
void pascal ins_level(int, ENTRY *);
int  pascal insert_ix(ENTRY *, IX_DESC *);
int  pascal find_ix(ENTRY *, IX_DESC *, int);
int  pascal combineblk(RECPOS, int);
void pascal replace_entry(ENTRY *);
void print_blk(BLOCK *);


/*  file I/O for B-PLUS module  */

void pascal error(j, l)
  int j;
  long l;
  {
    static char *msg[3] = {"ERROR - CANNOT OPEN/CLOSE FILE",
                           "ERROR WHILE READING FILE",
                           "ERROR WHILE WRITING FILE"};
    printf("\n  %s - Record Number %ld\n", msg[j], l);
    exit(1);
  } /* error */


void pascal read_if(start, buf, nwrt)
  long start;
  char *buf;
  int nwrt;
  {
    long err;
    err = start - lseek(pci->ixfile, start, SEEK_SET);
    if (err == 0) err = nwrt - read(pci->ixfile, buf, nwrt);
    if (err != 0) error(1, start);
  } /* read_if */


void pascal write_if(handle, start, buf, nwrt)
  int handle;
  long start;
  char *buf;
  int nwrt;
  {
    long err;
    err = start - lseek(handle, start, SEEK_SET);
    if (err == 0) err = nwrt - write(handle, buf, nwrt);
    if (err != 0) error(2, start);
  } /* write_if */


int pascal creat_if(fn)
  char *fn;
  {
    int   ret;
    ret = open(fn,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,S_IWRITE);
    if (ret  < 0) error(0,0L);
    return (ret);
  } /* creat_if */


int pascal open_if(fn)
  char *fn;
  {
    int  ret;
    ret = open(fn,O_RDWR|O_BINARY);
    if (ret < 1) error(0,0L);
    return (ret);
  } /* open_if */


void pascal close_if(handle)
  int handle;
  {
    if(close(handle) < 0)  error(2,0L);
  } /*  close_if */


int cdecl open_index(name, pix, dup)
  char *name;
  IX_DESC *pix;
  int dup;
  {
    pci = pix;
    pci->ixfile = open_if(name);
    pci->duplicate = dup;
    read_if(0L,(char *)&(pix->root), (sizeof(BLOCK) + sizeof(IX_DISK)));
    if (!cache_init)
      {
        init_cache();
        cache_init = 1;
      }
    first_key(pix);
    return ( IX_OK );
  } /* open_index */


int cdecl close_index(pix)
  IX_DESC *pix;
  {
    int i;
    write_if(pix->ixfile, 0L,(char *)&(pix->root),
               (sizeof(BLOCK) + sizeof(IX_DISK)));
    for (i = 0; i < NUM_BUFS; i++)
      if (BUFHANDLE(i) == pix->ixfile)
        {
          if (BUFDIRTY(i))
            {
              write_if(BUFHANDLE(i),
                       BUFBLOCK(i).brec,
                       (char *) &BUFBLOCK(i),
                       sizeof(BLOCK));
              BUFDIRTY(i) = 0;
            }
          BUFBLOCK(i).brec = NULLREC;
      }
    close_if(pix->ixfile);
    return( IX_OK );
  } /* close_index */


int cdecl make_index(name, pix, dup)
  char *name;
  IX_DESC *pix;
  int dup;
  {
    pci = pix;
    pci->ixfile = creat_if(name);
    pci->duplicate = dup;
    pci->dx.nl = 1;
    pci->dx.ff = NULLREC;
    pci->level = 0;
    CO(0) = -1;
    CB(0) = 0L;
    pci->root.brec = 0L;
    pci->root.bend = 0;
    pci->root.p0 = NULLREC;
    write_if(pci->ixfile, 0L,(char *)&(pix->root),
               (sizeof(BLOCK) + sizeof(IX_DISK)));
    if (!cache_init)
      {
        init_cache();
        cache_init = 1;
      }
    first_key(pix);
    return ( IX_OK );
  } /* make_index */


/*  cache I/O for BPLUS  */

void pascal update_block()
  {
    if (block_ptr != &(pci->root))
       BUFDIRTY(cache_ptr) = 1;
  } /* update_block */


void pascal init_cache()
  {
    register int  j;
    for (j = 0; j < NUM_BUFS; j++)
      {  BUFDIRTY(j) = 0;
         BUFCOUNT(j) = 0;
         BUFBLOCK(j).brec = NULLREC;
      }
  } /* init_cache */


int pascal find_cache(r)
  RECPOS r;
  {
    register int  j;
    for (j = 0; j < NUM_BUFS; j++)
      {
        if((BUFBLOCK(j).brec == r) && (BUFHANDLE(j) == pci->ixfile))
         {  cache_ptr = j;
            return (1);
      }  }
    return (-1);
  } /* find_cache */


int pascal new_cache()
  {
    register int  i;
    i = (cache_ptr + 1) % NUM_BUFS;
    if (BUFDIRTY(i)) write_if(BUFHANDLE(i),
                              BUFBLOCK(i).brec,
                              (char *) &BUFBLOCK(i),
                              sizeof(BLOCK));
    BUFHANDLE(i) = pci->ixfile;
    BUFDIRTY(i) = 0;
    return (i);
  } /* new_cache */


void pascal load_cache(r)
  RECPOS r;
  {
    cache_ptr = new_cache();
    read_if(r, (char *)&BUFBLOCK(cache_ptr), sizeof(BLOCK));
  } /* load_cache */


void pascal get_cache(r)
  RECPOS r;
  {
    if (find_cache(r) < 0)
       load_cache(r);
    block_ptr = &BUFBLOCK(cache_ptr);
  } /* get_cache */


void pascal retrieve_block(j, r)
  int j;
  RECPOS r;
  {
    if (j == 0)
       block_ptr = &(pci->root);
    else  get_cache(r);
    CB(j) = block_ptr->brec;
  } /* retrieve_block */


/*  low level functions of BPLUS  */

int pascal prev_entry(off)
  int off;
  {
     if (off <= 0)
       {
         off = -1;
         CO(pci->level) = off;
       }
     else
       off = scan_blk(off);
     return(off);
  } /* prev_entry */


int pascal next_entry(off)
  int off;
  {
     if (off == -1)
       off = 0;
     else
       {
         if (off < block_ptr->bend)
            off += ENT_SIZE(ENT_ADR(block_ptr,off));
       }
     CO(pci->level) = off;
     return (off);
  } /* next_entry */


int pascal copy_entry(to, from)
  ENTRY *to;
  ENTRY *from;
  {
    int me;
    me = ENT_SIZE(from);
    memcpy(to, from, me);
  } /* copy_entry */


int pascal scan_blk(n)
  int n;
  {
     register int off, last;
     off = 0;
     last = -1;
     while (off < n )
       {  last = off;
          off += ENT_SIZE(ENT_ADR(block_ptr,off));
       }
     CO(pci->level) = last;
     return (last);
  } /* scan_blk */


int pascal last_entry()
  {
     return( scan_blk(block_ptr->bend) );
  } /* last_entry */


/*  maintain list of free index blocks  */

int pascal write_free(r, pb)
  RECPOS r;
  BLOCK *pb;
  {
    pb->p0 = FREE_BLOCK;
    pb->brec = pci->dx.ff;
    write_if(pci->ixfile, r, (char *) pb, sizeof(BLOCK));
    pci->dx.ff = r;
  } /* write_free */


RECPOS pascal get_free()
  {
    RECPOS  r, rt;

    r = pci->dx.ff;
    if ( r != NULLREC )
      {  read_if(r, (char *)&rt, sizeof( RECPOS ));
         pci->dx.ff = rt;
      }
    else
      r = filelength (pci->ixfile);
    return (r);
  } /* get_free */


/*  general BPLUS block level functions  */

int pascal find_block(pe, poff)
  ENTRY *pe;
  int *poff;
  {
    register int pos, nextpos, ret;
    pos = -1;
    nextpos = 0;
    ret = 1;
    while ( nextpos < block_ptr->bend)
      {
        ret = strcmp((char *)(pe->key),
                     (char *)(ENT_ADR(block_ptr, nextpos)->key));
        if (ret <= 0)
          {
             if (ret == 0) pos = nextpos;
             break;
          }
        pos = nextpos;
        nextpos = next_entry(pos);
      }
    CO(pci->level) = pos;
    *poff = pos;
    return (ret);
  } /* find_block */


void pascal movedown(pb, off, n)
  BLOCK *pb;
  int off;
  int n;
  {
    memcpy(ENT_ADR(pb, off),
           ENT_ADR(pb, off + n),
           pb -> bend - (off + n));
  } /* movedown */


void pascal moveup(pb, off, n)
  BLOCK *pb;
  int off;
  int n;
  {
    memcpy(ENT_ADR(pb, off + n),
            ENT_ADR(pb, off),
            pb->bend - off);
  } /* moveup */


void pascal ins_block(pb, pe, off)
  BLOCK *pb;
  ENTRY *pe;
  int off;
  {
    int size;
    size = ENT_SIZE(pe);
    moveup(pb,off,size);
    copy_entry(ENT_ADR(pb,off),pe);
    pb->bend += size;
  } /* ins_block */


void pascal del_block(pb, off)
  BLOCK *pb;
  int off;
  {
    int ne;
    ne = ENT_SIZE(ENT_ADR(pb, off));
    movedown(pb, off, ne);
    pb->bend -= ne;
  } /* del_block */


/*  position at start/end of index  */

int cdecl first_key(pix)
  IX_DESC *pix;
  {
    pci = pix;
    block_ptr = &(pci->root);
    CB(0) = 0L;
    CO(0) = -1;
    pci->level = 0;
    while(block_ptr->p0 != NULLREC)
      {
        retrieve_block(++(pci->level), block_ptr->p0);
        CO(pci->level) = -1;
      }
    return ( IX_OK );
  } /* first_key */


int cdecl last_key(pix)
  IX_DESC *pix;
  {
    long  ads;
    pci = pix;
    block_ptr = &(pci->root);
    CB(0) = 0L;
    pci->level = 0;
    if(last_entry() >= 0)
      {
        while ((ads = ENT_ADR(block_ptr,last_entry())->idxptr) != NULLREC)
             retrieve_block(++(pci->level), ads);
      }
    CO(pci->level) = block_ptr->bend;
    return ( IX_OK );
  } /* last_key */


/*  get next, previous entries  */

int cdecl next_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    RECPOS  address;
    pci = pix;
    retrieve_block(pci->level, CB(pci->level));
    if(CO(pci->level) == -1) address = block_ptr->p0;
    else
     {
       if (CO(pci->level) == block_ptr->bend) address = NULLREC;
       else  address = ENT_ADR(block_ptr, CO(pci->level))->idxptr;
     }
    while (address != NULLREC)
      {
         retrieve_block(++(pci->level), address);
         CO(pci->level) = -1;
         address = block_ptr->p0;
      }
    next_entry(CO(pci->level));
    if (CO(pci->level) == block_ptr->bend)
      {
        do
          { if(pci->level == 0)
              {
                last_key(pci);
                return (EOIX);
              }
            --(pci->level);
            retrieve_block(pci->level, CB(pci->level));
            next_entry(CO(pci->level));
          } while (CO(pci->level) == block_ptr->bend);
      }
    copy_entry(pe, ENT_ADR(block_ptr, CO(pci->level)));
    return ( IX_OK );
  } /* next_key */


int cdecl prev_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    RECPOS  address;
    pci = pix;
    retrieve_block(pci->level, CB(pci->level));
    prev_entry(CO(pci->level));
    if (CO(pci->level) == -1)
      address = block_ptr->p0;
    else
      address = ENT_ADR(block_ptr, CO(pci->level))->idxptr;
    if (address != NULLREC)
      { do
          {
            retrieve_block(++(pci->level), address);
            address = ENT_ADR(block_ptr, last_entry())->idxptr;
          } while (address != NULLREC);
      }
    if (CO(pci->level) == -1)
      { do
          {
            if(pci->level == 0)
              {
                first_key(pci);
                return (EOIX);
              }
            --(pci->level);
          } while (CO(pci->level) == -1);
        retrieve_block(pci->level, CB(pci->level));
      }
    copy_entry(pe, ENT_ADR(block_ptr, CO(pci->level)));
    return ( IX_OK );
  } /* prev_key */


/*  insert new entries into tree  */

int pascal split(l, pe, e)
  int l;
  ENTRY *pe;
  ENTRY *e;
  {
    int  half, ins_pos, size;
    ins_pos = CO(pci->level);
    half = scan_blk(block_ptr->bend / 2 + sizeof(RECPOS));
    if (half == ins_pos)
      *e = *pe;
    else
      {
         copy_entry(e, ENT_ADR(block_ptr, half));
         size = ENT_SIZE(e);
         movedown(block_ptr, half, size);
         block_ptr->bend -= size;
      }
    spare_block = &BUFBLOCK(new_cache());
    memcpy(spare_block->entries,
           ENT_ADR(block_ptr,half),
           block_ptr->bend - half);
    spare_block->brec = get_free();
    spare_block->bend = block_ptr->bend - half;
    spare_block->p0 = e->idxptr;
    block_ptr->bend = half;
    e->idxptr = spare_block->brec;
    if (ins_pos < half)
      ins_block(block_ptr,pe,ins_pos);
    else if (ins_pos > half)
      {
         ins_pos -= ENT_SIZE(e);
         ins_block(spare_block,pe,ins_pos - half);
         CB(l) = e->idxptr;
         CO(l) = CO(l) - half;
      }
    write_if(pci->ixfile, spare_block->brec,
             (char *) spare_block, sizeof(BLOCK));
  } /* split */


void pascal ins_level(l, e)
  int l;
  ENTRY *e;
  {
    int  i;
    if ( l < 0)
      {  for (i = 1; i < MAX_LEVELS; i++)
           {  CO(MAX_LEVELS - i) = CO(MAX_LEVELS - i - 1);
              CB(MAX_LEVELS - i) = CB(MAX_LEVELS - i - 1);
           }
         memcpy(spare_block, &(pci->root), sizeof(BLOCK));
         spare_block->brec = get_free();
         write_if(pci->ixfile, spare_block->brec,
                  (char *) spare_block, sizeof(BLOCK));
         pci->root.p0 = spare_block->brec;
         copy_entry((ENTRY *) (pci->root.entries), e);
         pci->root.bend = ENT_SIZE(e);
         CO(0) = 0;
         pci->level = 0;
         (pci->dx.nl)++;
      }
    else ins_block(block_ptr,e,CO(l));
  } /* ins_level */


int pascal insert_ix(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    ENTRY    e, ee;
    int h;
    h = 0;
    pci = pix;
    ee = *pe;
    do
      {
         if(CO(pci->level) >= 0)
           CO(pci->level) +=
                  ENT_SIZE(ENT_ADR(block_ptr, CO(pci->level)));
         else
           CO(pci->level) = 0;
         update_block();
         if( (block_ptr->bend + ENT_SIZE(&ee)) <= split_size)
           {
             ins_level(pci->level, &ee);
             break;
           }
         else
           {
             h = 1;
             split(pci->level,&ee, &e);
              ee = e;
              pci->level--;
              if (pci->level < 0)
                {
                  ins_level(pci->level, &e);
                  break;
                }
              retrieve_block(pci->level, CB(pci->level));
           }
      }
    while (1);
    if (h) find_ix(pe, pix, 0);
    return ( IX_OK );
  } /* insert_ix */


/*  BPLUS find and add key functions  */

int pascal find_ix(pe, pix, find)
  ENTRY *pe;
  IX_DESC *pix;
  int find;
  {
    int      level, off, ret;
    RECPOS   ads;
    ENTRY    found;
    pci = pix;
    ads = 0L;
    level = ret = 0;
    while (ads != NULLREC)
      {  pci->level = level;
         retrieve_block(level, ads);
         if (find_block(pe, &off) == 0) ret = 1;
         if (ret && find) break;
         if (off == -1)
           ads = block_ptr->p0;
         else
           ads = ENT_ADR(block_ptr, off)->idxptr;
         CO(level++) = off;
       }
     return ( ret );
   } /* find_ix */


int cdecl find_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    int ret;
    ret = find_ix(pe, pix, 1);
    if ( ret ) copy_entry(pe, ENT_ADR(block_ptr, CO(pci->level)));
    return ( ret );
  } /* find_key */


int cdecl add_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    int ret;
    ret = find_ix(pe, pix, 0);
    if ( ret && (pci->duplicate == 0)) return ( IX_FAIL );
    pe->idxptr = NULLREC;
    return (insert_ix(pe, pix));
  } /* add_key */


int cdecl locate_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
    int ret;
    ENTRY e;
    ret = find_ix(pe, pix, 1);
    if (ret) copy_entry(pe, ENT_ADR(block_ptr, CO(pci->level)));
    else if (next_key(pe,pix) == EOIX) ret = EOIX;
    return ( ret );
  } /* locate_key */


int cdecl find_exact(pe, pix)
  ENTRY *pe;
  IX_DESC * pix;
  {
    int  ret;
    ENTRY e;
    copy_entry(&e, pe);
    ret = find_key(&e, pix);
    if ( ret && pci->duplicate)
      {
        do
          {
            ret = (e.recptr == pe->recptr);
            if( !ret )  ret = next_key(&e, pci);
            if (ret) ret = (strcmp(e.key, pe->key) == 0);
            if ( !ret ) return ( 0 );
          } while ( !ret );
      }
    copy_entry(pe, &e);
    return ( ret );
  } /* find_exact */


/* BPLUS delete key functions */

int cdecl delete_key(pe, pix)
  ENTRY *pe;
  IX_DESC *pix;
  {
     ENTRY   e;
     RECPOS  ads;
     int     h, leveli, levelf;
     if (!find_exact(pe, pix))  return( IX_FAIL );
     h = 1;
     if ((ads = pe->idxptr) != NULLREC)
       {
          leveli = pci->level;
          do
            {
               retrieve_block(++(pci->level), ads);
               CO(pci->level) = -1;
            }
          while ((ads = block_ptr->p0) != NULLREC);
          CO(pci->level) = 0;
          copy_entry(&e, ENT_ADR(block_ptr, CO(pci->level)));
          levelf = pci->level;
          pci->level = leveli;
          replace_entry(&e);
          pci->level = levelf;
       }
     while ( h )
       {
          retrieve_block(pci->level, CB(pci->level));
          del_block(block_ptr, CO(pci->level));
          update_block();
          if ( (pci->level == 0) && (block_ptr->bend == 0))
          /* tree was reduced in height */
            {
              if (pci->root.p0 != NULLREC)
                {
                  retrieve_block(++pci->level, pci->root.p0);
                  memcpy(&(pci->root), block_ptr, sizeof(BLOCK));
                  (pci->dx.nl)--;
                  write_free(block_ptr->brec, block_ptr);
                  BUFDIRTY(cache_ptr) = 0;
                  BUFHANDLE(cache_ptr) = 0;
                }
              break;
            }
          h = (block_ptr->bend < comb_size) && (pci->level > 0);
          if ( h )
              h = combineblk(CB(pci->level), block_ptr->bend);
       }
    find_ix(pe,pix,0);
    return( IX_OK );
  } /* delete_key */


int pascal combineblk(ads, size)
  RECPOS ads;
  int size;
  {
    ENTRY  e;
    RECPOS address;
    int    esize, off, ret, saveoff, ibuff;
    ret = 0;
    saveoff = CO(--(pci->level));
    retrieve_block(pci->level, CB(pci->level));
    if ((off = next_entry( saveoff )) < block_ptr->bend)
      /* combine with page on right */
      {
        if ( (ENT_SIZE(ENT_ADR(block_ptr, off)) + size) < split_size)
          {
            copy_entry(&e, ENT_ADR(block_ptr, off));
            address = ENT_ADR(block_ptr, CO(pci->level))->idxptr;
            retrieve_block(++pci->level, address);
            ibuff = cache_ptr;
            spare_block = block_ptr;
            retrieve_block(pci->level, ads);
            esize = ENT_SIZE(&e);
            if(((block_ptr->bend + spare_block->bend + esize) >= split_size)
                 && (spare_block->bend <= block_ptr->bend + esize))
               return( ret );
            e.idxptr = spare_block->p0;
            ins_block(block_ptr, &e, block_ptr->bend);
            update_block();
            if ((block_ptr->bend + spare_block->bend) < split_size)
            /* combine the blocks */
              {
                memcpy(ENT_ADR(block_ptr, block_ptr->bend),
                       ENT_ADR(spare_block, 0),
                       spare_block->bend);
                block_ptr->bend += spare_block->bend;
                write_free(spare_block->brec, spare_block);
                BUFDIRTY(ibuff) = 0;
                BUFHANDLE(ibuff) = 0;
                --pci->level;
                ret = 1;
              }
            else
            /* move an entry up to replace the one moved */
              {
                copy_entry(&e, ENT_ADR(spare_block, 0));
                esize = ENT_SIZE(&e);
                movedown(spare_block, 0, esize);
                spare_block->bend -= esize;
                spare_block->p0 = e.idxptr;
                BUFDIRTY(ibuff) = 1;
                --(pci->level);
                replace_entry(&e);
              }
          }
      }
    else
      /* move from page on left */
      {
        if ( (ENT_SIZE(ENT_ADR(block_ptr, CO(pci->level))) + size)
                 < split_size)
          {
            copy_entry(&e, ENT_ADR(block_ptr, saveoff));
            off = prev_entry(saveoff);
            if (CO(pci->level) == -1) address = block_ptr->p0;
            else address = ENT_ADR(block_ptr, CO(pci->level))->idxptr;
            retrieve_block(++pci->level, address);
            off = last_entry();
            ibuff = cache_ptr;
            spare_block = block_ptr;
            retrieve_block(pci->level, ads);
            esize = ENT_SIZE(&e);
            if(((block_ptr->bend + spare_block->bend + esize) >= split_size)
                 && (spare_block->bend <= block_ptr->bend + esize))
               return( ret );
            BUFDIRTY(ibuff) = 1;
            CO(pci->level) = 0;
            e.idxptr = block_ptr->p0;
            ins_block(block_ptr, &e, 0);
            if ((block_ptr->bend + spare_block->bend) < split_size)
            /* combine the blocks */
              {
                memcpy(ENT_ADR(spare_block, spare_block->bend),
                       ENT_ADR(block_ptr, 0),
                       block_ptr->bend);
                spare_block->bend += block_ptr->bend;
                write_free(block_ptr->brec, block_ptr);
                BUFDIRTY(cache_ptr) = 0;
                BUFHANDLE(cache_ptr) = 0;
                CO(--(pci->level)) = saveoff;
                ret = 1;
              }
            else
            /* move an entry up to replace the one moved */
              {
                 block_ptr->p0 = ENT_ADR(spare_block,off)->idxptr;
                 copy_entry(&e, ENT_ADR(spare_block, off));
                 spare_block->bend = off;
                 update_block();
                 CO(--(pci->level)) = saveoff;
                 replace_entry(&e);
              }
          }
      }
    return ( ret );
  } /* combineblk */


void pascal replace_entry(pe)
  ENTRY *pe;
  {
    retrieve_block(pci->level, CB(pci->level));
    pe->idxptr = ENT_ADR(block_ptr, CO(pci->level))->idxptr;
    del_block(block_ptr, CO(pci->level));
    prev_entry(CO(pci->level));
    insert_ix(pe, pci);
  } /* replace_entry */

