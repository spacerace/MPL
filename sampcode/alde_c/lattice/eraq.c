/* ERAQ.C  a program to traverse DOS's tree-structured directory, looking
   for the file(s) specified. They are then displayed on the screen 
   and the user is prompted for ERASE or NOT ERASE. Basically, I got 
   damned tired of DOS not having an ERAQ like CPM.
    The basic traverse from directory to directory portion is a
   very versatile toy around which many useful utilities can (and will,
   I'm sure!) be written. Here's one of them. I'll do more as time
   and inclination permits, but I'll turn this one loose first because
   I think it's useful.
    This is written for MSDOS 2.0 or better, using the LATTICE C Compiler,
   on a Compaq. 7-20-84 jjw   
   ps I know this could be shortened drastically and speeded up by 
   combining some of the modules into larger functions, but this way was
   much easier to write & de-bug modularly and so I left it that way. Also,
   some of the modules might be useful in themselves to build into a
   library. This program differs from most C code I've seen before because
   1.) it does something useful.
   2.) it does not have pointers to pointers to arrays of pointers to functions
       returning pointers. It is easy and straight-forward, at the expense of
       some speed and length.
   3.) it is commented. Not extensively, but fairly well. The hard parts have
       to do with the dirstak, which is not a stack but a queue (dirqueue
       looked too silly to me and d_queue made me think of peanut buster
       parfaits). If puzzled, follow it through but keep the msdos manual
       handy to see what byte is what.
   If you have any questions about this, drop me a note. I frequent almost
   every tech-oriented CPM bulletin board around Chicago, Il. 
   Name is John Welch, or just jjw.
           2397 John Smith Dr. Apt B
           Schaumburg, Il 60194 */

#include "dos.h"
#include "stdio.h"

main(argc,argv)
int argc;
char *argv[];
{
char look[15],start[100],init_dir[100],drive[2],flag1,stuff[100];
int first,last,temp,temp1,ret;

printf("ERAQ.C Ver 1.00 1-25-85 John J. Welch\n");
printf("(for help, type ERAQ HELP)\n\n");

if (argc == 1)
 {
  /* get the filename to erase */ 
  printf("Filename to be erased -->");
  scanf("%s",look);

  /* get the drive we're going to do the erasing on */
  printf("Enter the drive letter -->");
  scanf("%s",stuff);
  drive[1] = stuff[0];

  /* get starting directory */
  printf("Directory to start in (absolute path from root) -->");
  scanf("%s",start);

  /* do we want to do all sub-directories, too, or just this one? */
  printf("Traverse all sub-directories under this one -->");
  scanf("%s",stuff);
  if ((stuff[0] == 'y') || (stuff[0] == 'Y'))
   flag1 = 1; /* if they specified Y, set the flag */
  else
   flag1 = 0; /* anything but Y or y means NO */

  }
else
 {
  if (argc != 2)
   {
HERE:
    printf("For prompts, just type ERAQ.\n");
    printf("Command line syntax is ERAQ  [drive:][starting path]file.ext[/y]\n");
    printf("where [drive:] is optional drive,\n");
    printf("  [starting path] is optional path to start at,\n");
    printf("  file.ext is the file name (wildcards ok) to delete,\n");
    printf("  [/y] is optionally if you want to do all sub-directories under the\n");
    printf("  starting directory (default value is no).\n");
    return;
   }
  else
   {
    if ((strcmp(argv[1],"HELP") == 0) || (strcmp(argv[1],"help") == 0))
     goto HERE;

    first = 0; /* first character of the command line */
    drive[1] = 255; /* set up to default to current drive */
    flag1 = 0; /* default it to not do sub-dirs */
    if (argv[1][1] == ':') /* if there was a drive specified, pick the letter */
    { /*off and store it someplace */
     drive[1] = argv[1][0];
     first = 2; /* now, the first character in the command line is #2 */
    }
    last = strlen(argv[1]); /* get the length of the command line */
    if (argv[1][last-2] == '/') /* if they used the toggle, set it up */
    {
     if ((argv[1][last-1] == 'y') || (argv[1][last-1] == 'Y'))
      flag1 = 1; /* if they specified Y, set the flag */
     else
      flag1 = 0; /* anything but Y or y means NO */
     last -= 2; /* bump the last character down */
     argv[1][last] = 0; /* and now, null terminate the command line */
    }

    for (temp = last; ((argv[1][temp] != ':') && (temp >= first) && (argv[1][temp] != '\\')); temp--); /* find last \ */

    for (temp1 = first; temp1 < temp+1; temp1++) /* get pathname (if any) */
    {start[temp1-first] = argv[1][temp1];}
    start[temp1-first] = 0; /* null terminate it */

    for (temp1 = temp+1; temp1 <= last; temp1++) /* get the filename to look for */
    {look[temp1-(temp+1)] = argv[1][temp1];}
    look[temp1-(temp+1)] = 0; /* null terminate this */

   }
  }

/* get the drive we started in */
get_drive(drive);

if (drive[1] == 255) /* if the default was for current drive, use it */
 drive[1] = drive[0];
else /* otherwise, normalize whatever they typed in to start with a=0 */
 if ((drive[1] > 'A') && (drive[1] < 'Z'))
   drive[1] = drive[1] - 'A';
 else
   drive[1] = drive[1] - 'a';

/* change to the starting drive */
ret=chg_drive(drive[1]);

if (ret < drive[1])
  printf("invalid drive selected...\n");
else
 {
  /* get the directory we started in */
  init_dir[0] = '\\';
  get_dir(init_dir+1);

  if (start[0] == 0) /* if there was no starting path, use the current one */
   strcpy(start,init_dir);

  /* do the actual work */
  doit(look,start,flag1);

  /* change back to the initial drive */
  chg_drive(drive[0]);

  /* now, change back to the initial directory */
  chg_dir(init_dir);
 } /* end else */

} /* end function */

doit(look,start,flag1)
char *look,*start,flag1;

{
 char file[3],name[128],*dirstak,curdir[140],*getmem();
 int ret,tmp,top_ndx,bottom_ndx;

 dirstak = getmem(10000); /* get a hunk of memory for the directory queue */
 if (dirstak == 0) /* if the getmem returned a null, error */
 {printf("not enough memory...\n");
  return;}

/* set the DTA to someplace we can use */
 put_dta(name);

 top_ndx = 0;    /* set pointer to top of directory queue */
 bottom_ndx = 0; /* set pointer to bottom of queue */

 /* put the starting directory in the queue */
 for (tmp = 0; start[tmp] != 0; tmp++)
  {dirstak[bottom_ndx++] = start[tmp];}
 dirstak[bottom_ndx++] = 0;

 while (top_ndx != bottom_ndx)
  {
   /* check for any sub-directories */
   ret = dirscan(file,name,curdir,dirstak,&top_ndx,&bottom_ndx);

   if (ret != 3)
    {
     /* run through the directory, querying for each file */
     filscan(look,name);
    }

   /* if only do this one directory, end here */
   if (flag1 == 0)
   {
    rlsmem(dirstak,10000);
    return(1);
   }

  } /* end while loop */
  rlsmem(dirstak,10000);

} /* end function */

dirscan(file,name,curdir,dirstak,top_ptr,bot_ptr)
char *file,*name,*curdir,*dirstak;
int *top_ptr,*bot_ptr;
{
 int ret,tmp,top_ndx,bottom_ndx,*p_file;

 bottom_ndx = *bot_ptr;
 top_ndx = *top_ptr;

 /* initialize the file name for the directory search */
 file[0] = '*';
 file[1] = '.';
 file[2] = 0;

 tmp = 0;
 for (top_ndx; dirstak[top_ndx] != 0; top_ndx++)
  { curdir[tmp++] = dirstak[top_ndx];}
 curdir[tmp] = 0;
 ++top_ndx;
 ret = chg_dir(curdir);
 if (ret != 3)
  {
   printf("\n   ON DIRECTORY %s\n",curdir);
   find_dir(name,file,curdir,dirstak,&bottom_ndx);
  }
 *bot_ptr = bottom_ndx;
 *top_ptr = top_ndx;

 return(ret);
}

find_dir(name,file,curdir,dirstak,bot_ndx)
char *name,*file,*curdir,*dirstak;
int *bot_ndx;
{
 int ret,tmp,bottom_ndx;
 unsigned temp;

 bottom_ndx = *bot_ndx; /* set the local variable = the real thing. */

/* search for first directory entry */
 temp = 0x0010;
 ret = srch1st(file,temp);

/* while the return from search is successful, check for dir attribute
   and make sure it isn't a . or .. entry, put the whole pathname (null
   terminated) at the bottom of the queue */

 while (ret != 18)
 { if ((name[21] == 0x10) && (name[30] != '.'))
   {
    for (tmp = 0; curdir[tmp] != 0; tmp++)
    {dirstak[bottom_ndx] = curdir[tmp];
     ++bottom_ndx;}
    if (curdir[1] != 0)
     {dirstak[bottom_ndx++] = '\\';}  
   for (tmp = 30; name[tmp] != 0; tmp++)
    {dirstak[bottom_ndx] = name[tmp];
     ++bottom_ndx;}
   dirstak[bottom_ndx++] = 0;

   } /* end if */
    ret = srchnext(file); /* get another directory entry */
 } /* end while */

 *bot_ndx = bottom_ndx; /*now restore the real value to its proper place */

} /* end function */

filscan(look,name)
char *look,*name;
{
 int ret;
 char tmp;
 unsigned temp;

 temp = 0x0000;
 ret = srch1st(look,temp); /* start looking for files */

 while (ret != 18)
 {
  printf("%s -->",name+30);
  tmp = getch(); /* get the key directly from keyboard */
  printf("%c\n",tmp); /* echo it back with <cr/lf> */
  if ((tmp == 'y') || (tmp == 'Y'))
    era_file(name+30);

  ret = srchnext(look); /* get another directory entry */
 } /* end while */

} /* end function */

put_dta(p_dta)
char *p_dta;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x1a;
 inreg.x.dx = p_dta;

 intdos(&inreg,&outreg);

 return(0);
}

srch1st(p_file,attr)
int *p_file;
unsigned attr;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x4e;
 inreg.x.cx = attr;
 inreg.x.dx = p_file;

 intdos(&inreg,&outreg);

 return(outreg.x.ax);
}

srchnext(p_file)
int *p_file;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x4f;

 intdos(&inreg,&outreg);

 return(outreg.x.ax);
}

chg_dir(curdir)
char *curdir;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x3b;
 inreg.x.dx = curdir;

 intdos(&inreg,&outreg);

 return(outreg.x.ax);
}

get_dir(temp)
char *temp;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x47;
 inreg.h.dl = 0; /* relative drive */
 inreg.x.si = temp; /* where to put the pathname */

 intdos(&inreg,&outreg);

 return(outreg.x.ax);
}

get_drive(where)
char *where;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x19;

 intdos(&inreg,&outreg);

 *where = outreg.h.al;
}

era_file(which)
char *which;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x41;
 inreg.x.dx = which; /* pointer to filename */

 intdos(&inreg,&outreg);

 /* there's no easy way to check for errors, so don't even do it.
    dos returns error flag in carry flag, LATTICE C can't access it without
    your own assembler routine. I have one, but it's a bit hairy. For those
    who insist on doing things right, drop me a note. jjw */
}

chg_drive(drive)
char drive;
{
 union REGS inreg;
 union REGS outreg;

 inreg.h.ah = 0x0e;
 inreg.h.dl = drive;

 intdos(&inreg,&outreg);

 return(outreg.h.al);
}
