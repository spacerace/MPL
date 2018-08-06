// list.c RHS based pointer linked list routines
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<dos.h>
#include<fcntl.h>

static _segment _tempseg;

typedef struct _list LIST;
typedef struct _litem LISTITEM;

struct _list
    {
    _segment seg;
    unsigned num;
    unsigned segsize;
    char _based(_tempseg) *name;
    LISTITEM _based(_tempseg) *item;
    };

struct _litem
    {
    void far *object;
    unsigned objectsize;
    char _based(_tempseg) *name;
    LISTITEM _based(_tempseg) *prev;
    LISTITEM _based(_tempseg) *next;
    };

typedef struct _listheader          // for Save/Restore only
    {
    unsigned segsize;
    unsigned offset;
    } LISTHEADER;

typedef LIST far                    *PLIST;
typedef LIST _based(_tempseg)       *BPLIST;
typedef LISTITEM _based(_tempseg)   *BPLITEM;

#define LITEMHDL              BPLITEM
#define _listinit()     (_tempseg = plist->seg)

PLIST ListInit(unsigned members, char *name);
BPLITEM ListAdd(PLIST plist, void far *object, unsigned size,
                char *name);
LITEMHDL ListFind(PLIST plist, char *name);
void far *ListDelete(PLIST plist, char *name);
static void far *_ListDelete(PLIST plist, BPLITEM bplitem);
void ListDestroy(PLIST plist);
void ListSetObject(PLIST plist, void far *object, unsigned size,
                   char *name);
void far *ListGetObject(PLIST plist, char *name);
void ListDeleteObject(PLIST plist, char *name);
void ListSetName(PLIST plist, char *name);
static PLIST _ListInit(unsigned realsize);
unsigned ListGetNumItems(PLIST plist);
LITEMHDL ListGetItem(PLIST plist);
LITEMHDL ListItemGetNextItem(PLIST plist, LITEMHDL bplitem);
void far *ListItemGetObject(PLIST plist, LITEMHDL bplitem);

void error_exit(int err, char *msg);
void ListDump(PLIST plist);
void ListDumpItem(BPLITEM temp);


    // creates a list and returns a handle (void far *) to it
PLIST ListInit(unsigned members, char *name)
    {
    long realsize;
    PLIST plist;

    if(!members)              // if no number specified, set high
        members = ((0xffff/2) / sizeof(LISTITEM));
    realsize = sizeof(LISTITEM);
    realsize *= members;

    if(realsize > 65535L)     // check size value
        return NULL;

    if((plist = _ListInit((unsigned)realsize+sizeof(LIST))) != NULL)
        {
        plist->name = _NULLOFF;
        ListSetName(plist,name);
        plist->num = 0;
        }
    return plist;
    }

    // internal function: intializes a list and returns a pointer
static PLIST _ListInit(unsigned realsize)
    {
    BPLIST plist;
                              // allocate segment for entire list
    if((_tempseg = _bheapseg(realsize)) == _NULLSEG)
        return NULL;
                              // allocate space for header
    if((plist = (BPLIST)_bmalloc(_tempseg, sizeof(LIST)))== _NULLOFF)
        {
        _bfreeseg(_tempseg);
        return (PLIST)NULL;
        }
    plist->seg = _tempseg;
    plist->segsize = realsize;
    plist->item = _NULLOFF;
    return (PLIST)plist;
    }

    // sets the name of a list; can be used to change the name
void ListSetName(PLIST plist, char *name)
    {
    _listinit();

    if(plist->name == _NULLOFF)
        _bfree(plist->seg, plist->name);
    if((plist->name = _bmalloc(plist->seg,strlen(name)+1)) !=
                                                            _NULLOFF)
        _fstrcpy((char far *)plist->name,(char far *)name);
    }

    // adds a new item to the list and returns a pointer to it
BPLITEM ListAdd(PLIST plist, void far *object, unsigned size,
                char *name)
    {
    BPLITEM plistitem;

    _listinit();       // set _tempseg for this operation

                       // if unable to allocate item
    if((plistitem= _bmalloc(plist->seg,sizeof(LISTITEM)))== _NULLOFF)
        return _NULLOFF;

    plistitem->object = object;         // set object pointer
    plistitem->objectsize = size;
                                        // allocate item name and set
    if((plistitem->name = _bmalloc(plist->seg,strlen(name)+1)) !=
                                                            _NULLOFF)
        _fstrcpy((char far *)plistitem->name,(char far *)name);
    if(plist->item == _NULLOFF)    // find next available list member

        {
        plist->item = plistitem;   // use list->item if not used
        plistitem->prev = _NULLOFF;
        }
    else
        {
        BPLITEM temp = plist->item; // or find one in list

        for( ; temp->next != _NULLOFF; temp = temp->next);
        temp->next = plistitem;
        plistitem->prev = temp;
        }
    plistitem->next = _NULLOFF;
    plist->num++;

    return plistitem;
    }

    // deletes a named item from a list; returns the object's pointer
void far *ListDelete(PLIST plist, char *name)
    {
    BPLITEM temp;

    _listinit();

    if((temp = ListFind(plist,name)) != _NULLOFF)
        return _ListDelete(plist, temp);
    plist->num--;
    return (void far *)NULL;
    }

    // internal function: deletes item from a list and returns object
static void far *_ListDelete(PLIST plist, BPLITEM bplitem)
    {
    void far *object;

    _listinit();
    object = bplitem->object;

    if(bplitem->prev != _NULLOFF)       // remove item from list
                                        // set prev's next to next
        bplitem->prev->next = bplitem->next;
    else                                // unless there is no prev
                                   // then set parent's item to next
        plist->item = bplitem->next;
    if(bplitem->next != _NULLOFF)            // if a next
        bplitem->next->prev= bplitem->prev;  // set it's prev to prev
    _bfree(plist->seg,bplitem->name);        // free the name
    _bfree(plist->seg,bplitem);              // free the item
    return object;                           // return object to user
    }

    // relates an object to a list item
void ListSetObject(PLIST plist, void far *object, unsigned size,
    char *name)
    {
    BPLITEM temp;

    _listinit();

    if((temp = ListFind(plist,name)) != _NULLOFF)
        {
        temp->object = object;          // set object ptr
        temp->objectsize = size;        // set object size
        }
    }

    // returns the count of items in the list
unsigned ListGetNumItems(PLIST plist)
    {
    _listinit();

    return plist->num;                  // return number of items
    }

    // frees the object related to the named item
void ListDeleteObject(PLIST plist, char *name)
    {
    BPLITEM temp;

    _listinit();

    if((temp = ListFind(plist,name)) != _NULLOFF)
        {
        _ffree(temp->object);           // free object
        temp->object = (void far *)NULL;// set ptr to NULL
        }
    }

    // frees list and related objects
void ListDestroy(PLIST plist)
    {
    BPLITEM bplitem;

    _listinit();

    for(bplitem = plist->item; bplitem != _NULLOFF;
        bplitem = bplitem->next)
        _ffree(bplitem->object);        // free each object

    _bfreeseg(plist->seg);              // free list segment
    }

    // finds an item in the list by name, and returns a handle
LITEMHDL ListFind(PLIST plist, char *name)
    {
    BPLITEM temp;

    _listinit();

    for( temp = plist->item; temp != _NULLOFF; temp = temp->next)
        if(!_fstrncmp((char far *)temp->name,
            (char far *)name, strlen(name)))
            return (LITEMHDL)temp;
    return 0;
    }

    // dumps list contents to the screen
void ListDump(PLIST plist)
    {
    BPLITEM temp;

    _listinit();

    printf("List Name=%Fs\n", (char far *)plist->name);

    printf("Located @ %lp, occupies %u bytes in %04x:0000"
            " and contains:\n",
            (void far *)plist,
            plist->segsize,
            plist->seg);

    if(!plist->item)
        {
        printf("No items.\n");
        return;
        }
    for( temp = plist->item; temp != _NULLOFF; temp = temp->next)
        ListDumpItem(temp);
    }

    // dumps an individual item to the screen
void ListDumpItem(BPLITEM bplitem)
    {
    char far * name1;
    char far * name2;
    void far * prev;
    void far * next;

    if (bplitem->next != _NULLOFF)
        {
        next = (void far *)bplitem->next;
        name1 = (char far *)bplitem->next->name;
        }
    else
        {
        next = NULL;
        name1 = "NULL";
        }

    if (bplitem->prev != _NULLOFF)
        {
        prev = (void far *)bplitem->prev;
        name2 = (char far *)bplitem->prev->name;
        }
    else
        {
        prev = NULL;
        name2 = "NULL";
        }

    printf("Item: %Fs is located @ %lp, "
            "(stores related object @ %lp)"
            "\n\t(prev=%lp \"%Fs\" next=%lp \"%Fs\")\n",
            (char far *)bplitem->name,
            (void far *)bplitem,
            (void far *)bplitem->object,
            prev,
            name2,
            next,
            name1);
    }

    // returns a pointer to the object related to bplitem
void far *ListItemGetObject(PLIST plist, LITEMHDL bplitem)
    {
    _listinit();

    return (bplitem)->object;
    }

    // returns a pointer to the object related to the named item
void far *ListGetObject(PLIST plist, char *name)
    {
    BPLITEM temp;

    _listinit();

    if((temp = ListFind(plist,name)) != _NULLOFF)
        return temp->object;
    return (void far *)NULL;
    }

    // returns a handle to the first item in the list
LITEMHDL ListGetItem(PLIST plist)
    {
    _listinit();

    return (LITEMHDL)plist->item;
    }

    // returns a handle to the next item in the list after bplitem
LITEMHDL ListItemGetNextItem(PLIST plist, LITEMHDL bplitem)
    {
    _listinit();

    return (LITEMHDL)(bplitem)->next;
    }

    // saves LIST to file
unsigned ListSave(PLIST plist, char *filename)
    {
    int fh;
    unsigned doserror, bytes;
    BPLITEM temp;
    LISTHEADER lh;
    void far *buf;

    _listinit();

    FP_OFF(buf) = 0;
    FP_SEG(buf) = (unsigned)plist->seg;

    lh.segsize = plist->segsize;
    lh.offset = FP_OFF(plist);

        // create list file
    if(doserror = _dos_creat(filename,_A_NORMAL,&fh))
        error_exit(doserror,"Unable to create list file");

        // write file header with segsize and list header offset
    if((doserror = _dos_write(fh,(void far *)&lh,sizeof(lh),&bytes))
            || (bytes != sizeof(lh)))
        error_exit(doserror,"Unable to write list file header");

        // write segment
    if((doserror = _dos_write(fh,buf,plist->segsize,&bytes))
            || (bytes != plist->segsize))
        error_exit(doserror,"Unable to write list file segment");

        // write each object
    for(temp = plist->item; temp != _NULLOFF; temp = temp->next)
        if(temp->object)                // if node has an object
            if((doserror=_dos_write(fh,temp->object,temp->objectsize,
                    &bytes)) || (bytes != temp->objectsize))
                error_exit(doserror,
                           "Unable to write list file object");
    _dos_close(fh);
    return 0;
    }

    // restores list from file
PLIST ListRestore(char *filename)
    {
    int fh;
    unsigned doserror,bytes;
    PLIST plist;
    LISTHEADER lh;
    void far *buf;
    BPLITEM temp;

        // open list file
    if(doserror = _dos_open(filename,O_RDONLY,&fh))
        error_exit(doserror,"Unable to open list file");

        // read list file header with segsize and list header offset
    if((doserror = _dos_read(fh,(void far *)&lh,sizeof(lh),&bytes))
            || (bytes != sizeof(lh)))
        error_exit(doserror,"Unable to read list file header");

    if(!(plist = _ListInit(lh.segsize)))
        {
        _dos_close(fh);
        return (PLIST)NULL;
        }

    FP_OFF(plist) = lh.offset;          // reset to original offset
    FP_SEG(buf) = plist->seg;           // set buf to plist segment
    FP_OFF(buf) = 0;                    // and base of the segment

                                        // read entire segment
    if((doserror = _dos_read(fh,buf,lh.segsize,&bytes)) ||
            (bytes != lh.segsize))
        error_exit(doserror,"Unable to read list file segment");

    plist->seg = (_segment)FP_SEG(buf); // reset (since overwritten)

    for(temp = plist->item; temp != _NULLOFF; temp = temp->next)
        if(temp->object)                // if item had an object
            {
            if(!(temp->object = _fmalloc(temp->objectsize)))
                error_exit(0,"Unable to allocate space for object");
            if((doserror= _dos_read(fh,temp->object,temp->objectsize,
                    &bytes)) || (bytes != temp->objectsize))
                error_exit(doserror,"Unable to read list object");
            }
    _dos_close(fh);
    return plist;
    }

void error_exit(int err, char *msg)
    {
    printf("%s\n",msg);
    exit(err);
    }


// end of program
