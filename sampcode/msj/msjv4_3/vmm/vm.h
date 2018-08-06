
/*******************************************************************/
/* Virtual Memory Manager          VM.H                            */
/*                                                                 */
/* (C) Copyright 1988  Marc Adler/Magma Systems-All Rights Reserved*/
/*                                                                 */
/* This software is for personal use only, and may not be used in  */
/* any commercial product, nor may it be sold in any way.          */
/*                                                                 */
/*******************************************************************/

#define NO              0
#define YES             1

#define MAXSLOTS        1024
#define PAGESIZE        4096
#define EMM_PAGESIZE    16384
#define MAXPATHLEN      65

typedef unsigned PAGEID;
typedef unsigned long HANDLE;


/*
   The page header ....
*/
typedef struct page
{
  struct page *next;            /* chain to next free page in list */
  char far *memaddr;            /* memory address of the page block */
  unsigned long diskaddr;       /* disk address of the page block */
  PAGEID   id;                  /* page identifier */
  unsigned long LRUcount;       /* least-recently-used count      */
  unsigned pagesize;            /* how many bytes is this page    */
  unsigned freebyte;            /* index of 1st free byte in page */
  unsigned bytesfree;           /* # of bytes free in this page   */
  unsigned maxcontigfree;       /* max # of contiguous free bytes */

  unsigned flags;
#define PAGE_IN_MEM     0x0001
#define PAGE_ON_DISK    0x0002
#define IS_DIRTY        0x0004
#define SET_PAGE_DIRTY(p)		((p)->flags |= IS_DIRTY)
#define NON_SWAPPABLE   0x0008
#define PAGE_IN_EMM     0x0010
} PAGE;


typedef struct disktable
{
#define SECTOR_FREE ((PAGE *) NULL)
  PAGE     *page;
} PAGE_DISK_ENTRY;


/*
   The free block header .....
*/
typedef struct freeinfo
{
  unsigned nextfree;  /* offset of next free entry (0xFFFF at end) */

#define FREELIST_END  0xFFFF

  unsigned bytesfree; /* # of bytes in this chunk */
} FREEINFO;


/*
  This contains info about the swap file...
*/
typedef struct vmfile
{
  char filename[MAXPATHLEN];    /* name of the VM file */
  int  fd;                      /* file handle to the VM file */
  PAGE_DISK_ENTRY slottable[MAXSLOTS];
} VMFILE;


/* External declarations */
extern char     VMInitialized;
extern unsigned TotalPages;
extern PAGE     *PageList;
extern VMFILE   VMFile;
extern unsigned VMPageSize;

/*global*/  int VMInit(void);
/*global*/  int VMTerminate(void);
/*global*/  char *AllocPageText(struct page *, unsigned int );
/*global*/  struct page *AllocPage(void);
/*global*/  int ReadPage(struct page *);
/*global*/  int WritePage(struct page *);
/*global*/  int SwapoutPage(struct page *,int );
/*global*/  int SwapinPage(struct page *);
/*global*/  struct page *FindLRUPage(void);
/*global*/  int FindSlotFree(void);
/*global*/  HANDLE MyAlloc(unsigned int );
/*global*/  void MyFree(HANDLE);
/*global*/  struct page *FindNBytesFree(unsigned int );
/*global*/  struct page *FindNContigBytesFree(unsigned int );
/*global*/  int CompactifyPage(struct page *);
/*global*/  char far *MemDeref(HANDLE);
/*global*/  PAGE *PageDeref(HANDLE);
/*global*/  char *mymalloc(unsigned);
