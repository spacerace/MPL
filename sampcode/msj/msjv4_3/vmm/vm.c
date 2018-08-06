/*******************************************************************/
/* Virtual Memory Manager          VM.C                            */
/*                                                                 */
/* (C) Copyright 1988 Marc Adler/Magma Systems-All Rights Reserved */
/*                                                                 */
/* This software is for personal use only, and may not be used in  */
/* any commercial product, nor may it sold in any way.             */
/*                                                                 */
/*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "vm.h"


/* macro to return a ptr to a freeinfo structure */
#define FREEPTR(pg, offset)   (FREEINFO *) (pg->memaddr + offset)

unsigned TotalPages = 0;     /* Total # of pages allocated */
char VMInitialized = NO;     /* TRUE if the VMM has be initialized */
PAGE *PageList = NULL;       /* Linked list of pages */
VMFILE VMFile;               /* VM File info */
unsigned VMPageSize = PAGESIZE; /* Pagesize of a VM block */
unsigned long LRUclock = 0;     /* Clock used for LRU swapping */
unsigned Emergency = NO;        /* TRUE if DOS has no more memory */



/* VMInit
     Initialize the Virtual Memory Manager (VMM).
*/
VMInit()
{
  char *mktemp();
  char *pascal strend();
  int  i;
  char *tempdir;
  
  /* 
     Create the VM swap file. Try to use the path specified in the
     METEMP environment variable. If METEMP was not specified, use
     the current directory.
  */
  VMFile.filename[0] = '\0';
  if ((tempdir = getenv("METEMP")) != NULL)
  {
    char *end = strend(strcpy(VMFile.filename, tempdir));
    if (end[-1] != '\\')                    /* add the backslash */
    {
      *end = '\\';
      *++end = '\0';
    }
  }

  strcat(VMFile.filename, mktemp("VMXXXXXX"));
  if ((VMFile.fd = open(VMFile.filename, 
                         O_RDWR|O_TRUNC|O_CREAT|O_BINARY,
                         S_IWRITE|S_IREAD)) < 0)
  {
    err("VMM - Cannot initialize VM system");
    die(-1);
  }
  
  for (i = 0;  i < MAXSLOTS;  i++)
    VMFile.slottable[i].page = NULL;

  VMInitialized = YES;
  return 0;
}


/* VMTerminate
     Close and delete the VM swap file.
*/
VMTerminate()
{
  if (VMInitialized)
  {
    close(VMFile.fd);
    unlink(VMFile.filename);
    VMInitialized = NO;
  } 
}


/* MemDeref
     main routine for dereferencing a VM handle.
*/
char far *MemDeref(h)
  HANDLE h;
{
  int  pid, offset;
  PAGE *p, *prevp;

  /* Separate the handle into the page id 
     and the offset within that page */ 

  pid = (int) ((h >> 16) & 0x0000FFFF);
  offset = (int) (h & 0x0000FFFF);
  
  /* Search the linked list of pages for 
     the page with id 'pid'. */ 

  for (prevp=p=PageList; p && p->id != pid; prevp=p, p=p->next)
    ;

  if (!p)
    return (HANDLE) NULL;
    
  /* If the page is not in conventional memory, swap it in. */
  if (!(p->flags & PAGE_IN_MEM))
    SwapinPage(p);

  /* Update the LRU count and bring the 
     page to the front of the pagelist */

  p->LRUcount = LRUclock++;
  if (p != PageList)
  {
    prevp->next = p->next;
    p->next = PageList;
    PageList = p;
  }

  /* Return the absolute address which the handle references. */
  return p->memaddr + offset;
}


MakePageDirty(h)
  HANDLE h;
{
  PAGE *p;
  if ((p = PageDeref(h)) != NULL)
    SET_PAGE_DIRTY(p);
}


PAGE *PageDeref(h)
  HANDLE h;
{
  int  pid, offset;
  PAGE *p;

  /* Separate the handle into the page id 
     and the offset within that page */ 

  pid = (int) ((h >> 16) & 0x0000FFFF);
  offset = (int) (h & 0x0000FFFF);
  
  /* Search the linked list of pages for the page with id 'pid'. */
  for (p = PageList;  p && p->id != pid;  p = p->next)
    ;
  return p;
}


/* AllocPage
     Allocates a new page and links it to the page list.
*/
PAGE *AllocPage()
{
  PAGE *p;
  char *s;
  FREEINFO *f;
  
  /* Make sure that the VMM is ready. */
  if (!VMInitialized)
    VMInit();

  /* Allocate the page structure from the heap */
  if ((p = (PAGE *) calloc(sizeof(PAGE), 1)) == NULL)
    return NULL;
    
  /* Allocate the page's text buffer from DOS or EMM */
  if ((s = AllocPageText(p, VMPageSize)) == NULL)
    return NULL;

  /* Fill the page structure. */ 
  p->memaddr       = s;
  p->flags        |= PAGE_IN_MEM;
  p->id            = ++TotalPages;
  p->LRUcount      = LRUclock++;
  p->pagesize      = VMPageSize;
  p->freebyte      = 0;
  p->bytesfree     = VMPageSize;
  p->maxcontigfree = VMPageSize;

  /*
     Set up the page's initial free node. 

        p                  memaddr
    ----------------     ---------------
    | freebyte | 0 |---> |nextfree |-1 |
    ----------------     ---------------
    | bytesfree|4K |     |bytesfree|4K |
    ----------------     ---------------
  */
 
 f = (FREEINFO *) s;
  f->bytesfree = VMPageSize;
  f->nextfree  = FREELIST_END;

  /* Link the page to the head of the pagelist. */ 
  p->next = PageList;
  PageList = p;

  return p;
}


/* AllocPageText
 * Allocs a block of 'size' bytes to be used as the page's buffer.
 */

char far *AllocPageText(page, size)
  PAGE     *page;
  unsigned size;
{
  PAGE *p;
  char far *s;

  /* Use DOSALLOC() to allocate the 4K page buffer */
  while ((s = mymalloc(size)) == NULL /* || test_swap */)
  {
    /* Swap out the Least Recently Used page. 
       Return the LRU page ptr. */

    if ((p = FindLRUPage()) == NULL)
    {
      err("VMM - FindLRUPage() can't find a page to swap"); 
      return NULL;
    }

    /* Use the swapped-out page's text buffer as the new buffer. */
    s = p->memaddr;
    SwapoutPage(p, NO);
    memset(s, 0, size);
    break;
  }

  return s;
}


/*
  ReadPage()
  This is called from MemDeref() when the referenced page is not in
  conventional memory.
  
  If the page is already in memory, then just return;
  
  Seek to the disk address and read the proper number of bytes
  mark the page as being in memory

*/

ReadPage(p)
  PAGE *p;
{
  char *s;

  if (p->flags & PAGE_ON_DISK)
  {
    /* Obtain a text buffer which the 
       swapped page will be read into. */

    if ((s = AllocPageText(p, p->pagesize)) == NULL)
      return -1;

    /* Seek to the proper place and read the page. */
    lseek(VMFile.fd, (long) (p->pagesize * p->diskaddr), 0);
    if (read(VMFile.fd, p->memaddr = s, p->pagesize) != p->pagesize)
    {
      err("FATAL ERROR! read() failed in ReadPage()");
      die(-1);
    }

    /* Say that the page is now in conventional 
       memory as well as on disk. */

    p->flags |= PAGE_IN_MEM;
    p->LRUcount = LRUclock++;
  }

  return 0;
}


/*
  WritePage()
  
  If the page is on disk or it's in memory and there 
  is a copy out on disk and the page isn't dirty return
    
  Find a free sector in the VM file
  Seek to the free sector and write the page
  Clear the dirty bit
  make an entry in the page/disk table and put the page in there
*/

WritePage(p)
  PAGE *p;
{
  unsigned sector;

  /* If the page is not in memory, then don't write it to disk */
  if (!(p->flags & PAGE_IN_MEM))
    return 0;
    
  /* The page has a disk sector allocated to it already */
  if ((p->flags & PAGE_ON_DISK) && !(p->flags & IS_DIRTY))
    return 0;  /* the in-mem copy is the same as the copy on disk */
  
  if ((sector = FindSlotFree()) == 0xFFFF)
  {
    err("VMM - No more slots free.");
    return -1;
  }

  /* See to the sector and dump the page text. */ 
  lseek(VMFile.fd, (long) (sector * (long) p->pagesize), 0);
  if (write(VMFile.fd, p->memaddr, p->pagesize) != p->pagesize)
  {
    err("FATAL ERROR! write() failed in WritePage()");
    exit(-1);
  }
  
  VMFile.slottable[sector].page = p;
  p->diskaddr = (long) sector;
  p->flags |= PAGE_ON_DISK;
  p->flags &= ~IS_DIRTY;
  p->LRUcount = LRUclock++;
}


SwapOutAllPages()
{
  PAGE *p;
  for (p = PageList;  p;  p = p->next)
    SwapoutPage(p, YES);
}


/*
  This is called from AllocPageText() when there 
  is no more DOS memory free and no more Expanded 
  memory free to allocate a page.
*/

SwapoutPage(p, free_it)
  PAGE *p;
{
  WritePage(p);
  if (free_it)
  {
    if (p->flags & PAGE_IN_MEM)
      my_free(p->memaddr);
  }
  p->flags &= ~PAGE_IN_MEM;
}


SwapInAllPages()
{
  PAGE *p;
  for (p = PageList;  p;  p = p->next)
    SwapinPage(p);
}

SwapinPage(p)
  PAGE *p;
{
  ReadPage(p);
}


/*
   FindLRUPage 
   Finds the Least Recently Used page and returns a pointer to it.
*/

PAGE *FindLRUPage()
{
  PAGE *p;
  PAGE *retp = NULL;
  unsigned long minLRU = 0xFFFFFFFF;
  
  for (p = PageList;  p;  p = p->next)
  {
    if ((p->flags & PAGE_IN_MEM) && 
         p->LRUcount <= minLRU)       /* && IS_SWAPPABLE */
    {
      retp = p;
      minLRU = p->LRUcount;
    }
  }
  
  return retp;
}


/* 
  FindSlotFree
     returns the first unused entry in VMFile.slottable
*/
FindSlotFree()
{
  register int i;
  
  for (i = 0;  i < MAXSLOTS;  i++)
    if (VMFile.slottable[i].page == NULL)
      return i;
  return -1;
}


/**********************************************************/
/*                                                        */
/*         Routines to allocate mem from a page           */
/*                                                        */
/**********************************************************/

/*
  MyAlloc(n)
  
  We want to allocate n bytes from the system.
  
  Make sure that n is less than VMPageSize.
  Make sure that n is a mutiple of 8.

  Find a page which has a contiguous block of n 
  bytes free. Give preference to a page which is 
  already in memory.
  
  If a page has n bytes free but not contiguous, 
  then we will do compaction on the page.

  If there is still no page with n bytes free, 
  then allocate a new page.
  
  At this point, we have a page 'p' with at least 
  n bytes free.
  
  Decrease the bytes-free count of the page.
  Link the unallocated bytes onto the page's free list.
  Update the MaxContiguousFree count of the page.

  Return the Page/Offset value to the caller.
*/

HANDLE MyAlloc(needed)
  unsigned needed;
{
  FREEINFO *f, *prevf, *newf;
  PAGE     *p, *FindNBytesFree();
  unsigned offset, bytes_needed, bytes_left;
  unsigned maxcontig, new_offset;
  HANDLE h;

  needed = (needed + sizeof(FREEINFO));
  if (needed <= 0 || needed > VMPageSize)
  {
    return (HANDLE) NULL;
  }

  /* Return a page that has at least 'needed' bytes free */
  if ((p = FindNContigBytesFree(needed)) == NULL)
    return (HANDLE) NULL; 

  /* Traverse the list of free chunks in 
     the page and find the first  */

  /* chunk with 'needed' bytes free. 'F' 
     will be the addr of the chunk. */

  maxcontig = 0;
  offset = p->freebyte;

  for (f = (FREEINFO *) (p->memaddr + p->freebyte);  
           f->bytesfree < needed;
           f = (FREEINFO *) (p->memaddr + offset))
  {
    prevf = f;   /* save ptr to previous chunk for linking */
    if ((offset = f->nextfree) == FREELIST_END)

      /* If we get here, then there is not 'needed' 
         contiguous bytes free */

      return (HANDLE) NULL; 
    maxcontig = max(maxcontig, f->bytesfree);
  }
      
  /* Unlink and relink f (make sure there 
     is enough room for the link) */

  bytes_left = f->bytesfree - needed;
  if (bytes_left <= sizeof(FREEINFO))  /* less than 4 
                                          bytes remaining? */
  {
    needed = f->bytesfree;  /* no room for the link - */
    bytes_left = 0;         /* alloc the whole thing  */
  }

  /* 
     If the chunk that we are allocating is smaller than the 
     page's biggest free chunk, then there should be no change 
     to the max. We set maxcontig equal to the page's maxcontig 
     so that we won't traverse the rest of the free chain below.
   */

  if (f->bytesfree < p->maxcontigfree)
    maxcontig = p->maxcontigfree;

  if (bytes_left == 0)
  { /* We allocate the entire chunk */
    if (offset == p->freebyte)   /* The chunk is the 1st free one */
      p->freebyte = f->nextfree; /* so, relink the head pointer.  */
    else
      prevf->nextfree = f->nextfree;    /* Relink the previous */
    new_offset = f->nextfree;           /* chunk to next       */
  }

  else /* if (bytes_left) */
  { /* There is some space left over in the chunk */
    if (offset == p->freebyte)
      p->freebyte = offset + needed;
    else
      prevf->nextfree = offset + needed;
    /* Create a new chunk from the remaining bytes */
    newf = (FREEINFO *) (p->memaddr + offset + needed);
    newf->nextfree = f->nextfree;
    newf->bytesfree = bytes_left;
    maxcontig = max(maxcontig, newf->bytesfree);
    new_offset = offset + needed;
  }

  /* Now we traverse the rest of the freelist looking for a chunk */
  /* with more bytes free than maxcontig.                         */
  if (maxcontig < p->maxcontigfree && new_offset != FREELIST_END)
  {
    FREEINFO *f2;
    for (f2 = (FREEINFO *) (p->memaddr + new_offset);
         f2->bytesfree < p->maxcontigfree;
         f2 = (FREEINFO *) (p->memaddr + new_offset))
    if ((new_offset = f2->nextfree) == FREELIST_END)
      break;
    maxcontig = max(maxcontig, f2->bytesfree);
  }

  p->bytesfree -= needed; /* decrease # of free bytes in the page */
  p->maxcontigfree = maxcontig;
  f->bytesfree = needed;  /* set the length field 
                             in the returned chunk */
  
  /* Clear the allocated memory to zeroes */
  memset(p->memaddr + offset + sizeof(FREEINFO), 
         '\0', needed-sizeof(FREEINFO));

  /* Return offset */
  h = (HANDLE) (((long) p->id) << 16) | 
      (long) (offset + sizeof(FREEINFO));
  return h;
}


/* Return a pointer to a page with N bytes free */
PAGE *FindNContigBytesFree(needed)
  unsigned needed;      /* # of bytes needed */
{
  PAGE *diskpage = NULL;
  PAGE *p;
  
  /*
     Traverse the page list looking for a page with the needed
     amount of bytes free in one contiguous chunk.
  */
  for (p = PageList;  p;  p = p->next)
  {
    if (p->maxcontigfree >= needed)
    {
      if (p->flags & PAGE_IN_MEM)
      { /* If the page is in memory, then return it immediately */
        return p;
      }
      else if (p->flags & PAGE_ON_DISK)
      { /* If the page is on disk, then mark it as the candidate */
        if (!diskpage) diskpage = p;
      }
    }
  }
  
  /* 
     At this point, we have no memory-based page with  
     n contiguous bytes, and possibly a disk-based page 
     with n contiguous bytes.
  */

  if (diskpage)
  {
    SwapinPage(diskpage);
    CompactifyPage(diskpage);
    return diskpage;
  }
  else
  {

    /* No disk page had the needed bytes!!! 
       Try to allocate a new page! */

    return AllocPage();
  }
}


CompactifyPage(p)
  PAGE *p;
{
  /* 
     This routine has nothing in it yet.... Maybe one day, 
     we'll add compaction....
  */
}



void MyFree(h)
  HANDLE h;
{
  char *s;
  int  pid;
  PAGE *pg;
  FREEINFO *f, *hFree, *prevf;
  unsigned fOffset, hOffset, prevfOffset;

  /* Swap the page in if necessary */
  if ((s = MemDeref(h)) == NULL)
    return;

  pid = (int) ((h >> 16) & 0x0000FFFF);
  hOffset = (int) (h & 0x0000FFFF);
  hOffset -= sizeof(FREEINFO);
  for (pg = PageList;  pg && pg->id != pid;  pg = pg->next)  ;
  SET_PAGE_DIRTY(pg);


  hFree = FREEPTR(pg, hOffset);

  pg->bytesfree += hFree->bytesfree;

  /* See if we have an empty free list */
  if (pg->freebyte >= pg->pagesize)
  {
    /*
         pg          h
       freebyte---->nextfree--->X
    */
    pg->freebyte = hOffset;
    pg->maxcontigfree = hFree->bytesfree;
    hFree->nextfree = FREELIST_END;
    return;
  }
  
  /* Traverse free chain until we get a free chunk whose address */
  /*  is larger than the address of the chunk about to be freed. */
  for (f = FREEPTR(pg, (fOffset = pg->freebyte));
       hOffset > fOffset && f->nextfree != FREELIST_END;
       prevfOffset=fOffset, prevf=f, 
       f = FREEPTR(pg, (fOffset = f->nextfree)))
    ;
    
  /* See if we should insert the new chunk after the tail */
  /*
       100         200
        f           h
     nextfree--->nextfree--->X

  */
  if (hOffset > fOffset) /* we got here cause we hit the end */
  {
    /* Try to combine chunks f and h */
    if (fOffset + f->bytesfree == hOffset)
    {
      f->bytesfree += hFree->bytesfree;
      /* We should examine the block after f for coalescing */
    }
    else        /* Can't combine */
    {
      hFree->nextfree = FREELIST_END;
      f->nextfree = hOffset;
      /* We should examine the block after h for coalescing */
set_f_to_h:
      f = hFree;
      fOffset = hOffset;
      pg->maxcontigfree = max(pg->maxcontigfree, hFree->bytesfree);
    }
  }
  
  /* See if we should insert before the first free chunk */
  else if (pg->freebyte == fOffset)
  {
    /*
         pg->freebyte = 300

         200     300
          h------>f
    */
    
    hFree->nextfree = fOffset;
    pg->freebyte = hOffset;
    /* We should examine the block after h for coalescing */
    goto set_f_to_h;
  }
  
  /* We must be inserting between two existing free chunks */
  else
  {
    /*
       100         200        300
      prevf         h          f
     nextfree--->nextfree--->nextfree--->
    */
    /* See if we can coalesce prevf and h */
    if (prevfOffset + prevf->bytesfree == hOffset)
    {
      prevf->bytesfree += hFree->bytesfree;
      /* We should examine the block after prevf for coalescing */
      f = prevf;
      fOffset = prevfOffset;
      pg->maxcontigfree = max(pg->maxcontigfree, prevf->bytesfree);
    }
    else
    {
      hFree->nextfree = prevf->nextfree;
      prevf->nextfree = hOffset;
      /* We should examine the block after h for coalescing */
      goto set_f_to_h;
    }
  }
  
  /* We want to examine the block after 
     chunk f to see if we can coalesce */

  if (f->nextfree != FREELIST_END  &&  
                     fOffset + f->bytesfree == f->nextfree)
  {
    hFree = FREEPTR(pg, f->nextfree);
    f->nextfree = hFree->nextfree;
    f->bytesfree += hFree->bytesfree;
    pg->maxcontigfree = max(pg->maxcontigfree, f->bytesfree);
  }
}



char far *mymalloc(size)
  unsigned int size;
{
  char *s;
  unsigned seg;
  unsigned paras;
  
  /* We know that we have no DOS memory left */
  if (Emergency)
    return NULL;

  /* Convert bytes to paragraphs */
  paras = (size + 15) >> 4;

  /* Let DOS to do the allocation */
  if (_dos_allocmem(paras, &seg) != 0)
  {
    Emergency++;
    return NULL;
  }

  /*
    Convert the alocated memory into a far address and clear it out
  */

  FP_SEG(s) = seg;
  FP_OFF(s) = 0;
  memset(s, 0, paras << 4);
  return s;
}


my_free(s)
  char far *s;
{
  /* The memory must be returned to DOS */
  _dos_freemem(FP_SEG(s));
  Emergency = FALSE;	/* we can alloc some more DOS memory */
}


SetVMPageSize(kbytes)
  int kbytes;
{
  VMPageSize = (unsigned) kbytes * 1024;
}

