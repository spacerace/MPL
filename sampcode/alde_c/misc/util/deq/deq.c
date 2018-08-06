/*--------------------------------------------------------------------------*/
/*   deq.c- selective delete; queries for each file to delete
 *          Uses C86 filedir() subroutine to return matching directory
 *          entries, queries to delete each file separately: Yes or No;
 *          Entry of G ends polling, begins automatic deletion (abort with
 *                    any keystroke).  Quit ends program.
 *   Copyright 1985 Walter Borys. No liability or guarantee for usage.
 *   Distributed as Freeware for utility or educational purposes
 *              only; not to be sold under any circumstances. 
 *   Name and copyright notice may not be altered or removed.
 */
/*--------------------------------------------------------------------------*/
#include "stdio.h"
main(ac,av)
    int ac;
    char *av[];
    {
    extern char *filedir();         /* pointer to directory strings */
    char *filptr, *next;            /* pointer to current, next file*/
    char delfile[20];               /* filename with drive          */
    char *driveptr;                 /* pointer to drive with files  */
    int n,killall,reply,attrib;
    int bdos();
    static *hlptxt[]={
"DEQ Selective Delete Program Usage:  dq [d:]filnam.ext\n\n",
"  Program will poll to delete each matching file: /Y/N/G/Q\n",
"    'Y'ES or 'N'O to delete, 'G'O to delete without polling, or 'Q'UIT\n",
"    `G' process may be halted by hitting any key.\n",
"    * and ? wildcards are supported, pathes are not.\n",
"\nDEQ Version 1.1  Copyright Walt Borys 1985. Distributed as Freeware\n"};
/*--------------------------------------------------------------------------*/

    if (ac != 2)    /* print help message if no files specified */
        {
        for (n=0;n<6;n++)
            printf(hlptxt[n]);
        exit(0);
        }

    strncpy(delfile,av[1],20);              /* get file spec */
    attrib = 0;                             /* normal file attributes */
    for (n=0;n<strlen(delfile);n++)         /* check for pathes       */
        if (delfile[n] == 0134)
            abort("Sorry- don't do pathes \n");

    if (delfile[1]==':')                    /* another disk */
        driveptr=&delfile[2];
    else
        driveptr=&delfile[0];

    filptr = filedir(delfile,attrib);       /* directory string returned */

    if (filptr == NULL)
        {
        printf("No match for files %s found \n",delfile);
        exit(1);
        }

    killall = reply = 0;
    for (next = filptr; *next != NULL; next = next + strlen(next) + 1)
        {
        strcpy(driveptr,next);                   /* next file to delete */

        if (!killall)   /* query next delete, unless in delete all mode  */
            {
            while (bdos(11)&01)     /* flush typeahead */
                bdos(7);
            do  {             /* main query iteration- loop thru files  */
                printf(" Delete '%-15.15s'? (Y/N/G/Q) *",delfile);putchar(-1);
                reply = toupper(bdos(7) &0177);     /* keystroke reply  */
                if (reply=='\03')                   /* convert ^C to Q  */
                    reply = 'Q';
                printf("%c\n",reply);               /* echo reply       */
                } while (reply!='Y'&&reply!='N'&&reply!='G'&&reply!='Q');
            }

        if (reply == 'G') /* full delete, toggle reply to permanent Y  */
            killall = reply = 'Y';

        if (reply == 'Y')                        /* yes, delete file */
           unlink(delfile);

        if (reply == 'Q')                        /* quit */
            break;

        if (killall)         /* tell what is automatically being deleted */
            printf("   Deleting '%-15.15s'\n",next);

        if (killall && bdos(11)&01)         /* interrupt the full delete */
            killall = 0;                           /* go back to polling */
        }        
    free(filptr);                           /* de allocate memory  */
    }