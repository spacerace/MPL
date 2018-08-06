/* DI : Directory listing utility.   By  John H. DuBois III.   v1.1.
   Last rev 11/29/87.   Full documentation in DI.DOC.
   Searches directories and prints matching files.
   Multiple file specifications can be given.
   Any filespec can include wildcards in both filename and path.
   Recursive search of directories can be specified.
   Attributes of files to search for are given in and-terms; multiple and-terms
    can be given and files which match any of them will be printed.
   Files found in each directory can be sorted according to various criteria
    before groups of files with non-differing criteria are sorted by filename.
   Listing can be printed in long form, wide form, or with full path of files.
   Pause after each page can be specified.
   Printing of free space on device can be toggled off.
   Printing of headers can be toggled off.
   Total space used by the files listed can be printed.
   Initial listing specifications and search attributes can be specified by an
    environment variable.   */

#include <stdio.h>
#include <dir.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include "di.h"

main(int argc,char *argv[])
{
NameNode *Files,	/* Linked list of directories to print */
*Temp;		/* Points to entry to be disposed of */
int Attribs[MAX_ATTS];
int Cnt;	/* Number of parameters given in environment var + 1 */
ListType ListSpec;
static char *Defaults[3] = {"","*.*","/LI5HSV"};	/* Default listing:
 list all files in current dir; list Long; sort by dIr attrib; when listing
 wide, print 5 entries per line; print all entries except those with attribs
 Hidden, System, or Volume label. */
EntryListType EntryList;	/* Matching files found in a dir */
char *Vars[80];		/* Value of env var */

   setvbuf(stdout, NULL, _IOLBF, 0x200);
   setcbrk(1);	/* Turn on cntrl-brk checking at all DOS interupts */
   More(-1);	/* Initialize More() to disabled */
   Files = NULL;	/* Initialize Files to null list */
   ListSpec.PrintFree = TRUE;   /* Make free space printing the default */
   ListSpec.PrintHeaders = TRUE;	/* Make header printing the default */
   ListSpec.RecSearch = FALSE;	/* Make no recursive listing the default */
   GetArgs(3,Defaults,Attribs,&ListSpec,&Files);	/* Set up internal
    defaults for search attribs, list specs, and files to list */
   if (GetEnvParams(&Cnt,Vars,"DI")) {	/* If "DI" env var exists, get
	new defaults from it */
      GetArgs(Cnt,Vars,Attribs,&ListSpec,&Files);
      free(Vars[0]);	/* Free copied env var */
   }
   GetArgs(argc,argv,Attribs,&ListSpec,&Files);	/* Convert command line args
   	to search attribs, list specs, and file specs */
   if (!isatty(1)) /* If stdout is redirected, turn of More if it was
   			enabled */
      More(-1);
   if (ListSpec.PrintHeaders)
      PrintHeader(Files,ListSpec.PrintFree);	/* Print volume labels and
		   possibly free space for all drives given in file specs */
   while (Files != NULL) {	/* For each filespec given */
      /* Get all matching fls */
      GetEntries(&EntryList,Attribs,Files -> Name,!ListSpec.RecSearch);
      if (EntryList.NumEnts > 1)	/* If at least two matching files were
					found, sort list as specified */
         SortList(&EntryList,ListSpec.Sort);
      PrintList(&EntryList,&ListSpec,Files -> Name); /* Print list */
      Temp = Files;	/* Discard list node */
      free(Files);
      Files = Temp -> Next;
   }
}

More(int Lines)
/* Pauses and waits for a key to be struck when MORE_ROWS lines have been
   printed.  A call with Lines = 0 resets line count to 0 and enables More.
   A call with Lines = -1 disables More.  A call with a positive number
   increments the line count by that amount. If More is disabled, it will
   return from all calls without pausing until it is called with 0.
   More() is called each time new lines are printed, with Lines set to the
   number of lines printed. If a number of lines is printed between calls that
   is larger than (screen height - MORE_ROWS), lines may be lost (scrolled out
   without pausing).  If, when More() pauses for a key to be hit, <space>
   is pressed, the line count will be set to 0 and More will return.  If
   any other key except q is hit, the line count will be set to MORE_ROWS - 1
   and More will return.  In this case, the next time it is called it will
   pause again (unless it is called with 0). If 'q' is pressed, the program
   will be terminated with exit status - 1.  All output is directly to
   screen.  More returns its enable status (-1 if enabled, else 0) which
   is the inverse of the codes used to enable/disable More.  A call
   with a negative number other than -1 for Lines is a null call and will
   have no effect; it will only return the enable status.  Such a call can
   therefore be used to return a number which will toggle the enable status
   of More if it is called with it */
{
static int Line,	/* Line count */
Enabled = FALSE;	/* Whether More is enabled */

   if (Lines > -2)
      switch (Lines) {
         case 0 :	Line = 0;
			Enabled = TRUE;
			break;
         case -1 :	Enabled = FALSE;
			break;
         default :
            if (Enabled && (Line += Lines) >= MORE_ROWS) { /* Print message
            						to screen & pause */
               fputs("--More--",stderr);
               switch (getch()) {
                  case ' ' :	Line = 0;
				break;
                  case 'q' :
                  case 'Q' :	fputs("\b\b\b\b\b\b\b\bQuit    \n",stderr);
				exit(1);
                  default  :	Line = MORE_ROWS - 1;
               }
               fputs("\b\b\b\b\b\b\b\b        \b\b\b\b\b\b\b\b",stderr);
							/* Erase message */
            }
   }
   return (Enabled);
}

char *Insert(char *Target,char *Source,int Pos)
/* Inserts string Source into string Target at position Pos. If the length of
   Target is less than Pos, Source is appended to Target. */
{
unsigned SLen,TLen;	/* Length of source and target strings */

   SLen = strlen(Source);
   TLen = strlen(Target);
   if (TLen < Pos)
      strcat(Target,Source);
   else {
      memcpy(Target + Pos + SLen,Target + Pos,TLen - Pos + 1);	/* Make room
					in target string for source string */
      memcpy(Target + Pos,Source,SLen); /* Copy source string to targ string */
   }
   return(Target);
}

char *AttribWtoS(char A)
/* Converts an attibute word (A) to a string of characters representing the
   word's active attributes, puts the string in a static buffer, and
   returns a pointer to the buffer */
{
static char Attribs[7];		/* Attribute character buffer */
static char Convert[] = "RHSVDA";	/* Ordered attribute list */
int I,	/* Which attribute is being tested */
J,	/* Index to buffer */
Mask;	/* Bit mask to get attribute flag */

   Mask = 1;	/* Start with bit 0 */
   J = 0;
   for (I = 0; I < 6; I++) {
      if (A & Mask)	/* Add attribute to buffer */
         Attribs[J++] = Convert[I];
      Mask <<= 1;	/* Check next attribute */
   }
   Attribs[J] = '\0';	/* Terminate string */
   return(Attribs);
}

void *MallocT(unsigned Size)
/* Calls malloc() to allocate a block of memory of size Size and checks
   whether there was enough memory. Exits if not, otherwise returns a
   pointer to the block */
{
void *Temp;

   Temp = (void *) malloc(Size);
   if (Temp == NULL) {
      fputs("Error: Not enough memory.",stderr);
      exit(2);
   }
   return(Temp);
}

void *CallocT(unsigned NElem,unsigned Size)
/* Calls calloc() to allocate a block of memory for NElem elements of size
   Size and checks whether there was enough memory. Exits if not, otherwise
   returns a pointer to the block */
{
void *Temp;

   Temp = (void *) calloc(NElem,Size);
   if (Temp == NULL) {
      fputs("Error: Not enough memory.",stderr);
      exit(2);
   }
   return(Temp);
}

void *ReallocT(void *Ptr,unsigned Size)
/* Calls Realloc() to reallocate the block pointed to by Ptr to size Size
   and checks whether there was enough memory. Exits if not, otherwise
   returns a pointer to the block */
{
void *Temp;

   Temp = (void *) realloc(Ptr,Size);
   if (Temp == NULL) {
      fputs("Error: Not enough memory.",stderr);
      exit(2);
   }
   return(Temp);
}

