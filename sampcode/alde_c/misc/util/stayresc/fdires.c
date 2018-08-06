
/****************************************************************************/
/*                                                                          */
/*     FDIRES.C   Version 1.0  10/23/85                Brian Irvine         */
/*                                                                          */
/*        Released to the Public Domain for use without profit              */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* fdires.c - Program to demonstrate use of stayres.c in creating a         */
/*            terminate-and-stay-resident program in DeSmet C.              */
/*                                                                          */
/*            This program demonstrates the techniques used to create a     */
/*            program which will terminate but remain resident as part      */
/*            of DOS.  The program is a sorted directory displayer which    */
/*            will show a sorted and formatted listing of files on the      */
/*            current directory.  It is supplied mainly to demonstrate      */
/*            the techniques involved in creating TSR programs.             */
/*                                                                          */
/*            The program must be linked with the other module in this      */
/*            library using the -s option of bind with a stack allowance    */
/*            of 0x1000 (4096 decimal).  This should be more than           */
/*            adequate for 99% of all applications.                         */
/*                                                                          */
/*             Brian Irvine                                                 */
/*             3379 St Marys Place                                          */
/*             Santa Clara, CA 95051                                        */
/*             [71016,544]                                                  */
/*                                                                          */
/****************************************************************************/

#include   "stdio.h"

#define    MAX_DIRS        1000

#define    DOS_INT         0x21
#define    GET_DRV         0x19

#define    FILE_NOT_FOUND  2           /* DOS function error codes */
#define    INVALID_DRIVE   15
#define    NO_MORE_FILES   18

struct {
        char   reserved[21];           /* disk transfer area */
        char   attribute;
        int    time;
        int    date;
        long   file_size;
        char   name[13];
        } dta;

struct dir_entry{                      /* directory entry */
        char    e_attribute;
        int     e_time;
        int     e_date;
        long    e_file_size;
        char    e_name[13];
        } entry[MAX_DIRS];             /* enough room for 1000 entries */

long   free_space;
long   total_space;
int    entries = 0;
int    _rax, _rbx, _rcx, _rdx,     /* variables to hold register values */
              _rsi, _rdi, _rds, _res;
int    row, column;                /* cursor position - must be global */

extern char    _carryf;

char   label [12];
char   pathname [65];
char   buffer [65];
static char   save_screen[4000];          /* save the screen here */
long   get_free ();


extern void    stayres();      /* the code to terminate and stay resident */


/*----- Main ---------------------------------------------------------------*/
/*                                                                          */
/*     In this application all we have to do is call the tsr code.  Other   */
/*     initialization can be performed here before fencing ourselves off.   */
/*                                                                          */
/*--------------------------------------------------------------------------*/

main ()
{

   stayres ();

}

/*----- Main program entry point -------------------------------------------*/

fdir ()
{

   int     n, stat, curr_drv;
   int     mode;
   unsigned    screen_seg;

   /* get the video mode */

   mode = get_mode ();

   /* save the user's screen and cursor position */

   find_cursor ();
   if ( mode == 7 )
       screen_seg = 0xB000;
   else
       screen_seg = 0xB800;
   _lmove ( 4000, 0, screen_seg, save_screen, _showds() );
   scr_clr ();
   scr_rowcol (0,0);

   entries = 0;
   _setmem (buffer, 65, '\0' );        /* initialize buffers explicitly each */
   _setmem (pathname, 65, '\0' );      /* time through because it changes */
   if ( get_path () == ERR )
       {
       puts ("Invalid drive specifier.\n");
       exit(1);
       }

/*     Now construct the path name using current drive                      */

   curr_drv = get_curr_drv ();
   buffer[0] = curr_drv + 'A';
   buffer[1] = ':';
   if ( strlen ( pathname ) == 0 )
       buffer[2] = '\0';
   else
       {
       buffer[2] = '\\';
       buffer[3] = '\0';
       }

   strcat ( buffer, pathname );
   strcpy ( pathname, buffer );
   n = ( 80 - strlen ( pathname ) - 14 ) / 2;
   strncpy ( buffer, "                                ", n);
   strcat ( buffer, "Directory of " );
   strcat ( buffer, pathname );
   printf ( "%s\n\n", buffer );
   set_dta ();

/*     Get the first matching directory entry                               */

   if ( get_first () )
       {
       puts ("File not found.\n");
       exit (1);
       }

/*     Now collect all the other matching names                             */

   while ( ( stat = get_next () ) != NO_MORE_FILES )
       ;
   free_space = get_free ();
   sort_files ();
   print_files ();

   /* now put everything back where it was */

   puts ("\nPress any key to continue...");
   while ( !scr_csts() )
       ;

   _lmove ( 4000, save_screen, _showds(), 0, screen_seg );
   scr_rowcol ( row, column );

}



/*----- Get the current drive number ---------------------------------------*/

int    get_curr_drv ()
{

   return ( _os ( GET_DRV, 0 ) );

}



/*----- Get path name of current directory ---------------------------------*/

int    get_path ()
{

   _rax = 0x4700;
   _rdx = 0;
   _rsi = &pathname[0];
   _rds = _showds();
   _doint ( DOS_INT );
   if ( _carryf )
       return ERR;

}



/*----- Give the DTA address to DOS ----------------------------------------*/

void   set_dta ()
{

   _rax = 0x1A00;
   _rdx = &dta;
   _rds = _showds();
   _doint ( DOS_INT );

}



/*----- Get the first matching directory entry -----------------------------*/

int    get_first ()
{

   strcat ( pathname,"\\*.*" );    /* look for all files */
   _rax = 0x4E00;
   _rdx = &pathname;
   _rcx = 0xFF;                    /* attribute = all */
   _doint ( DOS_INT );
   if ( _carryf )                  /* if there's an error */
       return  ( _rax );           /* send back the code */
   _move ( 22, &dta.attribute, &entry[entries++] );    /* else move data to array */
   return ( 0 );

}



/*----- Get next matching directory entry ----------------------------------*/

void   get_next ()
{

   _rax = 0x4F00;
   _doint ( DOS_INT );
   if ( _carryf )
       return  ( _rax );
   _move ( 22, &dta.attribute, &entry[entries++] );
   return ( 0 );

}


/*----- Get free space left on disk ----------------------------------------*/

long   get_free ()
{

   _rdx = 0;
   _rax = 0x3600;
   _doint ( DOS_INT );
   total_space = ((long) ( (long)_rax * (long)_rdx * (long)_rcx ));
   return ((long) ( (long)_rax * (long)_rbx * (long)_rcx ));

}



/*----- Comparison routine for qsort() -------------------------------------*/

int    compare ( first, second )
struct dir_entry *first, *second;
{

   return ( strcmp (first -> e_name, second -> e_name ) );

}



/*----- Sort the directory entries by name ---------------------------------*/

void   sort_files ()
{

   qsort ( entry, entries, 22, compare );

}



/*----- Get the cursor location --------------------------------------------*/


void   find_cursor()
{
   _rax = 0x0300;
   _rbx = 0;
   _doint(0x10);
   row = _rdx >> 8;
   column = _rdx & 0x00FF;

}


/*----- Get the current video mode -----------------------------------------*/

int    get_mode ()
{
   _rax = 0x0F00;
   _doint ( 0x10 );
   return ( _rax & 0x00FF );

}


/*----- Print the directory entries and free space -------------------------*/
/*                                             this code needs to be redone */

void   print_files ()
{
   int i, middle, count, pm, dir, hour;
   struct dir_entry *next;
   char    namebuf[20];
   char    extbuf [4];
   char    *buf, *dirln;

   pm = hour = count = 0;
   next = &entry;
   middle = (entries + 1)/2;
   puts ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
   puts ("Filename Ext   Size     Date     Time  ³ Filename Ext   Size     Date     Time \n");
   puts ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
   for ( count = 0; count < middle; count++ )
       {
       buf = &namebuf;
       dirln = &entry[count].e_name;
       *buf = '\0';
       dir = FALSE;
       if ( entry[count].e_attribute & 0x10 )
           dir = TRUE;
       while ( *dirln && (*dirln != '.'))
           *buf++ = *dirln++;
       *buf = '\0';
       buf = &extbuf;
       *buf = '\0';
       if ( *dirln )
           dirln++;
       strcat( extbuf, dirln );
       hour = ( entry[count].e_time >> 11 ) & 0x001F;
       pm = FALSE;
       if ( hour > 12 )
           {
           hour -= 12;
           pm = TRUE;
           }
       if ( hour == 0 )
           hour = 12;
       if ( dir )
           {
           if ( strlen ( namebuf ) == 0 )
               strcpy ( namebuf, "." );
           else
               ;
           printf ("%-8s %-3s  [Dir]   %2d-%2d-%2d  %2d:%02d%1c ³",
                   namebuf,
                   extbuf,
                   (entry[count].e_date >> 5) & 0x000F,
                   (entry[count].e_date & 0x001F),
                   (entry[count].e_date >> 9) + 80,
                   hour,
                   (entry[count].e_time >> 5) & 0x003F,
                   (pm) ? 'p' : 'a');
           }
       else
           printf ("%-8s %-3s  %6D  %2d-%2d-%2d  %2d:%02d%1c ³" ,
                   namebuf,
                   extbuf,
                   entry [count].e_file_size,
                   (entry[count].e_date >> 5) & 0x000F,
                   (entry[count].e_date & 0x001F),
                   (entry[count].e_date >> 9) + 80,
                   hour,
                   (entry[count].e_time >> 5) & 0x003F,
                   (pm) ? 'p' : 'a');
       if ( (count + middle) < entries )
           {
           buf = &namebuf;
           dirln = &entry[count + middle].e_name;
           *buf = '\0';
           dir = FALSE;
           if ( entry[count+middle].e_attribute & 0x10 )
               dir = TRUE;
           while ( *dirln && (*dirln != '.'))
               *buf++ = *dirln++;
           *buf = '\0';
           buf = &extbuf;
           *buf = '\0';
           if ( *dirln )
               dirln++;
           strcat( extbuf, dirln );
           hour = ( entry[count + middle].e_time >> 11 ) & 0x001F;
           pm = 0;
           if ( hour > 12 )
               {
               hour -= 12;
               pm = 1;
               }
           if ( hour == 0 )
               hour = 12;
           if ( dir )
               {
               if ( strlen ( namebuf ) == 0 )
                   strcpy ( namebuf, "." );
               else
                   ;
               printf (" %-8s %-3s  [Dir]   %2d-%2d-%2d  %2d:%02d%1c\n",
                       namebuf,
                       extbuf,
                       (entry[count+middle].e_date >> 5) & 0x000F,
                       (entry[count+middle].e_date & 0x001F),
                       (entry[count+middle].e_date >> 9) + 80,
                       hour,
                       (entry[count+middle].e_time >> 5) & 0x003F,
                       (pm) ? 'p' : 'a');
               }
           else
               printf (" %-8s %-3s  %6D  %2d-%2d-%2d  %2d:%02d%1c\n" ,
                       namebuf,
                       extbuf,
                       entry [count+middle].e_file_size,
                       (entry[count+middle].e_date >> 5) & 0x000F,
                       (entry[count+middle].e_date & 0x001F),
                       (entry[count+middle].e_date >> 9) + 80,
                       hour,
                       (entry[count+middle].e_time >> 5) & 0x003F,
                       (pm) ? 'p' : 'a');
           }
       else
           puts ("\n" );
       if ( ( (count + 1) % 17 ) == 0 )
           {
           puts ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Press any key to continue ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
           while ( !scr_csts () )
               ;
           }
       }
   puts ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
   printf (" %3d directory %s                 ³", entries, (entries > 1) ? "entries" : "entry  " );
   printf ("  %D bytes free         %3d%% used\n", free_space, 100 * (total_space - free_space) / total_space );
   puts ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");

}


