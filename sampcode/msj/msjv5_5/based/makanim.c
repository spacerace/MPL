// makanim.c RHS creates animation list from screen shots

#include<stdio.h>
#include<dos.h>
#include<fcntl.h>
#include<malloc.h>
#include<conio.h>

#include"list.h"

#define SCREENSIZE 4000
#define SCREEN_ADDRESS 0xb8000000

void show_file(char *filename);
void show_object(void far *object);


void show_file(char *filename)
    {
    int fh;
    unsigned bytes, doserror;
    void far *screen = (void far *)SCREEN_ADDRESS;

    if(doserror = _dos_open(filename,O_RDONLY,&fh))
        error_exit(doserror,"Unable to open screen file");
    if((doserror = _dos_read(fh,screen,SCREENSIZE,&bytes)) ||
            (bytes != SCREENSIZE))
        error_exit(doserror,"Unable to read screen file");
    _dos_close(fh);
    }


void show_object(void far *object)
    {
    unsigned i;
    unsigned far *screen = (void far *)SCREEN_ADDRESS;
    unsigned far *objptr = (unsigned far *)object;

    for(i = 0; i < SCREENSIZE/2; i++, screen++, objptr++)
        *screen = *objptr;
    }

void main(void)
    {
    void far *L;
    void far *object;
    char *screen = "APPART";
    char name[20];
    int fh;
    unsigned doserror,bytes;

    if(L = ListInit(170,"ANIMATE"))
        {
      int i;

      for(i = 0; i < 22; i++)
        {
        sprintf(name,"%s%02d.ACP",screen,i);
        if(object = _fmalloc(SCREENSIZE))
            {
            printf("Adding %s\n",name);
            if(doserror = _dos_open(name,O_RDONLY,&fh))
                error_exit(doserror,"Unable to open screen file");
            if((doserror = _dos_read(fh,object,SCREENSIZE,&bytes)) ||
                    (bytes != SCREENSIZE))
                error_exit(doserror,"Unable to read screen file");
            _dos_close(fh);
            ListAdd(L,object,SCREENSIZE,name);
            }
        else
            {
            printf("Unable to allocate far memory: i=%d\n",i);
            break;
            }
        }

      for(i = 20; i > 0; i--)
        {
        sprintf(name,"%s%02d.ACP",screen,i);
        if(object = _fmalloc(SCREENSIZE))
            {
            printf("Adding %s\n",name);
            if(doserror = _dos_open(name,O_RDONLY,&fh))
                error_exit(doserror,"Unable to open screen file");
            if((doserror = _dos_read(fh,object,SCREENSIZE,&bytes)) ||
                    (bytes != SCREENSIZE))
                error_exit(doserror,"Unable to read screen file");
            _dos_close(fh);
            ListAdd(L,object,SCREENSIZE,name);
            }
        else
            {
            printf("Unable to allocate far memory: i=%d\n",i);
            break;
            }
        }
      ListDump(L);
      printf("Saving list...\n");
      ListSave(L,"ANIMATE.LST");
      printf("Destroying list...\n");
      ListDestroy(L);
      printf("Restoring list...\n");
      if(L = ListRestore("ANIMATE.LST"))
          {
          printf("List restored\n");
          ListDump(L);
          }
      }
    }



