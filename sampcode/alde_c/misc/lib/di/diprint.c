#include <stdio.h>
#include <dir.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include "di.h"
long FreeSpace(int Drive);
long SpaceUsed(Entry *Entries,int Num,int Drive);
char *Label(int Drive);
char *AddCommasL(long N);
char *FileDate(int PDate);
char *FileTime(int PTime);
void PrintFull(Entry *Entries,int Num,char *Dir);
void PrintLong(Entry *Entries,int Num);
void PrintShort(Entry *Entries,int Num,int LineEnts);
void FiltAttribs(int *OldAtts,int *NewAtts,int Att);
AUSize(int Drive);
MatchingAtts(int Attribs[],char Attrib);

void PrintHeader(NameNode *Files,int PrintFree)
/* Prints the volume label and, if PrintFree is true, the free space for all
   drives specified in the filespecs in Files, including the default drive if
   indicated by any filespec. */
{
int Drives[26],	/* One entry for each drive, indicating whether it should be
		   printed */
Drive,	/* Specific or default drive specified in current filespec */
CurDrive,	/* Default drive */
I;	
char *Volume;	/* Volume label of a drive */

   CurDrive = getdisk();	/* Get default drive in case it is specified */
   for (I = 0; I <= 25; I++)	/* Initialize array to no drives speced */
      Drives[I] = FALSE;
   while (Files != NULL) {	/* For each filespec */
      if (strchr(Files -> Name,':') == NULL)	/* If no drive speced, use
      						   default drive */
         Drives[CurDrive] = TRUE;
      else {
         Drive = Files -> Name[0] - 'A';	/* Get drive number */
         if (0 <= Drive && Drive <= 25)	/* Make sure it is a letter */
            Drives[Drive] = TRUE;
      }
      Files = Files -> Next;
   }
   putchar('\n');	/* Put a blank line between program invokation and
   			   first line of listing */
   for (I = 0; I <= 25; I++)	/* For each possible drive */
      if (Drives[I]) {	/* If drive was specified in any filespec */
         Volume = Label(I);	/* Get volume label */
         if (Volume == NULL)	
            printf("Volume in drive %c has no label",I + 'A');
         else
            printf("Volume in drive %c is %s",I + 'A',Volume);
         if (PrintFree)	/* If PrintFree is true, print free space */
            printf("  : %s bytes free",AddCommasL(FreeSpace(I)));
         puts(".");
         More(1);	
      }
}

int AUSize(int Drive)
/* Returns the size of the allocation units on drive Drive, in bytes. */
{
   struct fatinfo FatInfo;	/* Struct used by getfat() */
   getfat(Drive + 1,&FatInfo);
   /* Size of allocation units = # of sectors per cluster * bytes per sector */
   return(FatInfo.fi_sclus * FatInfo.fi_bysec);
}

long SpaceUsed(Entry *Entries,int Num,int Drive)
/* Returns the actual amount of space used by all of the files in Entries,
   assuming that they are on drive Drive */
{
   int AU;	/* Size of allocation units on drive Drive */
   int I;
   long TotalSpace = 0L;	/* Total space used by the files */

   AU = AUSize(Drive);	/* Find size of alloc units */
   for (I = 0; I < Num; I++)	/* For each file, add the space it uses */
      TotalSpace += (Entries[I] -> ff_fsize + AU - 1) / AU * AU;
   return(TotalSpace);
}

char *AddCommasL(long N)
/* Converts long integer N to a string, stores it in a static buffer,
   inserts commas where appropriate, and returns a pointer to the bufffer */
{
static char Buf[15];	/* Buffer */
int Pos,	/* Current position that comma is being inserted at */
Num,		/* Number of commas to insert */
I;

   Pos = sprintf(Buf,"%ld",N);	/* Convert N to a string and put the length of
   				   the string in Pos */
   Num = ((Buf[0] == '-' ? Pos - 1 : Pos) - 1) / 3;	/* Find num of commas
   							   to insert */
   for (I = 0; I < Num; I++) {
      Pos -= 3;		/* Find next location at which to insert comma */
      Insert(Buf,",",Pos);
   }
   return(Buf);
}

long FreeSpace(int Drive)
/* Returns the free space on drive Drive */
{
struct dfree DFree;	/* Structure used by getdfree */

   getdfree(Drive + 1,&DFree);	/* Get available clusters, sectors/cluster,
   				   and bytes/sector */
   ExTest(DFree.df_sclus == -1,"Disk error.");	/* Exit if error */
   return((long) DFree.df_avail * DFree.df_bsec * (long) DFree.df_sclus);
}

char *Label(int Drive)
/* if drive Drive (0 = A, 1 = B, etc) has a label, it is put in a static
   buffer and a pointer to the buffer is returned. Otherwise a null
   pointer is returned */
{
static char *FileSpec = " :\\*.*",	/* Filespec that will by used to
				find label, with room for drive letter */
Volume[13];	/* Buffer */
struct ffblk FFBlk;	/* Structure used by findfirst() */
char *Dot;	/* Start of file ext in volume label */

   FileSpec[0] = Drive + 'A';	/* Add drive letter to filespec */
   if (findfirst(FileSpec,&FFBlk,FA_LABEL))	/* Get label */
      return(NULL);	/* if no label, return null */
   else {
      strcpy(Volume,FFBlk.ff_name);	/* Copy label to volume */
      Dot = strchr(Volume,'.');	
      if (Dot != NULL)	/* If volume had a dot in it, remove it */
         strcpy(Dot,Dot - Volume + FFBlk.ff_name + 1);
      return(Volume);
   }
}

void GetEntries(EntryListType *EntryList,int Attribs[],char Name[],int PrintCP)
/* calloc() is used to make an array of pointers to the structure returned by
   findfirst/next.  The array is expanded with realloc() as neccessary.
   Elements of the array are made to point to matching directory entries.
   A dir entry will match if its name matches Name and its attribs match at
   least one of the attrib and-terms in Attribs.  "." and ".." dir entries will
   match only attrib specs that specifically include dirs, and only if PrintCP
   is true.  The entry list and number of matching entries found are returned
   in EntryList. */
{
int Done,	/* Whether all matching files have been found */
NumEnts,	/* Number of matching entries found */
DirAttribs[MAX_ATTS];	/* Attribute list containing those attrib terms in
			   Attribs that specifically include directories */
unsigned Length;	/* Current length that the entry array has been
			   c/realloc()ed to */
Entry *Entries;		/* Entry array */

   if (PrintCP)
      FiltAttribs(Attribs,DirAttribs,FA_DIREC);	/* Copy all entries in Attribs
			   that specifically include dirs into DirAttribs */
   else
      *DirAttribs = NO_AT;
   Length = ENTRY_INC;	/* Do initial allocation for Entries */
   Entries = (Entry *) CallocT(Length,sizeof(Entry));
   NumEnts = 0;		/* No entries found yet */
   Entries[0] = (Entry) MallocT(sizeof(struct ffblk));	/* Start with one
							   empty entry */
   Done = findfirst(Name,Entries[0],0x3f);
   while (!Done) {  /* For each entry in specified dir that matches Name */
     if (MatchingAtts(CPDir(Entries[NumEnts] -> ff_name) ?
      DirAttribs : Attribs,Entries[NumEnts] -> ff_attrib)) { /* Check whether
       the attribs of the found entry match any of those in Attribs if the
       found entry is not the current or parent dir ("." or ".."), or in
       DirAttribs if it is one of them.  The current or parent dir will
       therefore only be printed if directories have specifically been
       requested */
        if (NumEnts == Length - 1) {  /* Reallocate Entries with more space */
           Length += ENTRY_INC;
           Entries = (Entry *) ReallocT(Entries,Length * sizeof(Entry));
        }
        Entries[++NumEnts] = (Entry) MallocT(sizeof(struct ffblk));  /* Add
							new empty node */
        *(Entries[NumEnts]) = *(Entries[NumEnts - 1]); /* copy last matching
		        entry to new node for next call to findnext() */
      }
      Done = findnext(Entries[NumEnts]); 	/* Get next matching entry */
   }
   free(Entries[NumEnts]);	/* Get rid of last node added, since it is the
			   	   one that findnext() failed on */
   EntryList -> NumEnts = NumEnts;	/* Copy array & num of entries to
   					   EntryList */
   EntryList -> Entries = Entries;
}

void FiltAttribs(int OldAtts[],int NewAtts[],int Att)
/* Copies all attributes in OldAtts that have at least one of the attribs
   indicated in Att, or that have no attribs (= 0) to NewAtts */
{
int I,
NumAtts;	/* Number of matching attrib terms found */

   NumAtts = I = 0;
   while (OldAtts[I] != NO_AT) {
      if (OldAtts[I] & Att || OldAtts[I] == 0)
         NewAtts[NumAtts++] = OldAtts[I];
      I++;
   }
   NewAtts[NumAtts] = NO_AT;	/* Mark end of array */
}

void PrintList(EntryListType *EntryList, /* Entry list and length of list */
 ListType *ListSpec, /* Specifies what type of listing to do */
 char *Dir /* Name of path that list was produced from */)
/* Print list of dir entries according to ListSpec */
{
int J,
NumEnts;	/* Number of entries in list */
Entry *Entries;	/* Entry list */

   Entries = EntryList -> Entries;
   NumEnts = EntryList -> NumEnts;
   if (ListSpec -> ListType != 'F' && ListSpec -> PrintHeaders) {
      /* If list type is not "full path" and headers are to be printed...*/
      printf("Directory of %s  : %d matching file(s)",Dir,NumEnts);
      if (ListSpec -> PrintFree)	/* Print total space used by files */
         printf(" using %ldk",
          (SpaceUsed(Entries,NumEnts,(int) *Dir - 'A') + 1023) >> 10);
      puts(".\n");
      More(2);
   }
   if (NumEnts) {	/* If at least one matching entry for dir */
      switch (ListSpec -> ListType) {
         case 'W' : PrintShort(Entries,NumEnts,ListSpec -> LineEnts);
                    break;
         case 'L' : PrintLong(Entries,NumEnts);
                    break;
         case 'F' : PrintFull(Entries,NumEnts,Dir);
      }
      for (J = 0; J < NumEnts; J++) /* Free entry nodes pointed to by array */
         free(Entries[J]);
   }
   free(Entries);	/* Free array */
}

MatchingAtts(int Attribs[],char Attrib)
/* Returns true if Attrib matches any of the attrib and-terms in Attribs;
   otherwise returns false.  Attrib will match a term if it has all of the
   attributes specified in the low byte of the term and none of the attribs
   specified in the high byte of the term */
{
int I,
FullAtt;	/* Full attribute */

   FullAtt = Attrib | (~((int) Attrib) << 8);	/* Produce full attrib by
    copying Attrib in true form into the low byte and complemented form
    into the high byte. */
   I = 0;
   while (Attribs[I] != NO_AT && (Attribs[I] & FullAtt) != Attribs[I])
    /* Check whether Attrib matches any of the attribute terms given in
       Attribs. To check for match, FullAtt is first ANDed with an attrib
       term.  Since Attrib is true in the low byte of FullAtt, all attrib
       bits that Attrib has and are specified in lo byte of the term (attribs
       that Attrib must have to match) will be on in the low byte of the
       result.  Since Attrib is complemented in the high
       byte of FullAtt, all attrib bits that Attrib does not have and are
       specified in the high byte of the term (attribs that Attrib cannot
       have if it is to match) will be on in the high byte of the result.
       In order to match, the result must therefor be equal to the term. */
      I++;
   return(Attribs[I] != NO_AT);
}

void PrintShort(Entry *Entries,int Num,int LineEnts)
/* Prints a 'short' listing of the entries in Entries.  Prints the file names
   only, with LineEnts names on each line.  A backslash is appended to
   directory entries. Num is the number of entries in Entries */
{
int I,
Width,		/* Width to print each name with (padding with blanks) */
NotEven;	/* Whether the entries will exactly fit on a line */
char *Name,	/* Points to the name being printed (either the original
 		   list entry or a copy of it with a backslash appended) */
Dir[14];	/* Copy of the file name, with a backslash appended to
		   indicate that it is a directory */

   Width = 80 / LineEnts;	/* Find width to print each name with */
   NotEven = 80 % LineEnts;	/* Determine whether LineEnts names will
				   exactly fit on a line */
   for (I = 0; I < Num; I++) {	/* For each entry */
      if (Entries[I] -> ff_attrib & FA_DIREC) {	/* If the entry is a dir, make
       a copy of it, append a backslash to it, & make Name point to the copy */
         strcpy(Dir,Entries[I] -> ff_name);
         strcat(Dir,"\\");
         Name = Dir;
      }
      else	/* Otherwise make Name point to the original name */
         Name = Entries[I] -> ff_name;
      printf("%-*s",Width,Name);	/* Print the name */
      if ((I + 1) % LineEnts == 0) {	/* If it was the last name on a line */
         if (NotEven)	/* If LineEnts entries do not exactly fit on a line,
          the cursor will not have automatically wrapped around to the next
          line; print a newline */
            putchar('\n');
         More(1);
      }
   }
   if (I % LineEnts) {	/* If the last name printed was not the last one on
		   	a line and therefore did not cause a CR, print one */
      putchar('\n');
      More(1);
   }
}

void PrintLong(Entry *Entries,int Num)
/* Print a 'long' dir listing. Prints each file's name, ext, size, time, date,
   and attributes.  Dirs and the volume label report a size of 0, so they are
   printed with a special label in the size column.  The list is modified. */
{
int I;
Entry File;	/* Entry being printed */
char Size[12],	/* File size converted to a string, or a special label for
		   dirs and the volume label */
*Ext;		/* Start of the extension of the file name */

   for (I = 0; I < Num; I++) {
      File = Entries[I];
      if (File -> ff_attrib & FA_DIREC)	/* If entry is a dir or the vol label,
    				    give it a label instead of a file size */
         strcpy(Size,"<DIR>  ");
      else if (File -> ff_attrib & FA_LABEL)
         strcpy(Size,"<LAB>  ");
      else
         sprintf(Size,"%ld",File -> ff_fsize);
      Ext = strchr(File -> ff_name,'.');	/* Find the start of the ext
      						   of the file name */
      if (Ext == NULL || CPDir(File -> ff_name))	/* If there is no ext,
       or the first dot in the entry for the current or parent dir was found,
       make Ext point to a null string */
         Ext = "";
      else {
         *Ext = '\0';	/* Otherwise make Ext point to the start of the file
          ext and replace the dot with a null char so that when the file name
          is printed it will not include the file ext */
         Ext++;
      }
      printf("%-9s%-3s%9s%10s%8s   %-6s\n",File -> ff_name,Ext,Size,
       FileDate(File -> ff_fdate),FileTime(File -> ff_ftime),
       AttribWtoS(File -> ff_attrib));
      More(1);
   }
}

char *FileDate(int PDate)
/* Converts the file date word PDate into a string of the form mm-dd-yy,
   puts it in a static buffer and returns a pointer to the buffer */
{
static char Date[9];

   sprintf(Date,"%2u-%02u-%02u",(PDate >> 5) & 0xf,PDate & 0x1f,
    ((PDate >> 9) & 0x7f) + 80);
   return(Date);
}

char *FileTime(int PTime)
/* Converts the file time word PTime to a string of the form HH:MMm, where m
   is 'a' for AM and 'p' for PM, puts it in a static buffer, and returns a
   pointer to the buffer */
{
static char Time[7];
int NHour;	/* File hour in 12-hour format */

   NHour = ((PTime >> 11) & 0x1f) % 12;
   if (NHour == 0)
      NHour = 12;
   sprintf(Time,"%2u:%02u%c",NHour,(PTime >> 5) & 0x3f,
    ((PTime >> 11) & 0x1f) > 11 ? 'p' : 'a');
   return(Time);
}

void PrintFull(Entry *Entries,int Num,char *Dir)
/* Print 'Full Path' listing of the file names in Entries.  The entries in
   Entries are assumed to have been produced from the path given in Dir.
   Dir is copied, and the filename is removed from it.  For each file name in
   Entries, prints the file's size, time, date, and attributes; then Dir is
   printed followed by the filename.  If the entry is a dir, a backslash is
   printed at the end.
   Dirs and the volume label report a size of 0, so they are printed with a
   special label in the size column.
   The list is modified. Num is the # of entries in Entries. */
{
char Name[MAXPATH];	/* Full path name of the file name */
int I;
Entry File;	/* Entry being printed */
char Size[12];	/* File size converted to a string, or a special label for
		   dirs and the volume label */

   strcpy(Name,Dir);
   *(strrchr(Name,'\\') + 1) = '\0';	/* Remove the file name from Dir */
   for (I = 0; I < Num; I++) {
      File = Entries[I];
      if (File -> ff_attrib & FA_DIREC)	/* If entry is a dir or the vol label,
    				    give it a label instead of a file size */
         strcpy(Size,"<DIR>  ");
      else if (File -> ff_attrib & FA_LABEL)
         strcpy(Size,"<LAB>  ");
      else
         sprintf(Size,"%ld",File -> ff_fsize);
      /* If the entry is a directory, follow it with a backslash */
      printf("%9s%10s%8s  %-6s%s%s%s\n",Size,FileDate(File -> ff_fdate),
       FileTime(File -> ff_ftime),AttribWtoS(File -> ff_attrib),Name,
       File -> ff_name,File -> ff_attrib & FA_DIREC ? "\\" : "");
      More(1);
   }
}

void SortList(EntryListType *EntryList,char Sort)
/* Sorts EntryList according the sort specification Sort.  Any groups of
   entries that are not differentiated by the first sort are then sorted
   by name. The sort specs are:
     X = alpha by file eXtension; T = by Time & date, oldest to newest;
     Z = by file siZe, smallest to largest; I = by directory attribute,
     dirs first, then other files; N = sort by Name only;   */
{
int Num,	/* Number of entries */
Start,		/* Start of a group of undifferentiated entries */
I;
Entry *List;	/* List to sort */
int (*CompFunc)(Entry *E1,Entry *E2), /* Comparison func to be used in sort */
 ExtComp(Entry *E1,Entry *E2),TDComp(Entry *E1,Entry *E2),
 SizeComp(Entry *E1,Entry *E2),DAComp(Entry *E1,Entry *E2),
 NameComp(Entry *E1,Entry *E2);  /* Comparison functions */

   Num = EntryList -> NumEnts;
   List = EntryList -> Entries;
   if (Sort != 'N') {	/* Do a first sort */
      switch (Sort) {	/* Select sort function */
         case 'X' :	CompFunc = ExtComp;
          		break;
         case 'T' :	CompFunc = TDComp;
          		break;
         case 'Z' :	CompFunc = SizeComp;
          		break;
         case 'I' :	CompFunc = DAComp;
      }
      qsort(List,Num,sizeof(Entry),CompFunc);	/* Do first sort */
   }
   Start = 0;	/* Intialize undiffentiated entries start to 0 */
   for (I = 1; I < Num; I++)
      if ((*CompFunc)(&List[I - 1],&List[I])) {	/* If comp func differentiates
    					current entry from the previous one */
         if (I - Start > 1) /* If more than one entry was passed before
          differentiation was found, sort undifferentiated entries by name */
            qsort(List + Start,I - Start,sizeof(Entry),NameComp);
         Start = I;	/* Make start of undiff ents be current ent */
      }
   if (I - Start > 1)	/* If the last entry was the last of a group of
 			   undiffed entries, sort them */
      qsort(List + Start,I - Start,sizeof(Entry),NameComp);
}

DAComp(Entry *E1,Entry *E2)
/* Compares file dir attribs.  Returns a negative int if E1 is a dir and
   E2 is not; 0 if they both are or are not, and a positive int if E2 is
   and E1 is not */
{
   return(((*E2) -> ff_attrib & FA_DIREC) - ((*E1) -> ff_attrib & FA_DIREC));
}

NameComp(Entry *E1,Entry *E2)
/* Compares the names of E1 and E2.  Returns a neg int, zero, or a pos int if
   the name of E1 is lexicographically less than, equal to, or greater than
   the name of E2.  Since strings are terminated with a null (lex value 0),
   a file name will be less than the same initial name with extra characters,
   including an ext. Because '.' has a value less than any letter, a file name
   with an extension will be less than a file name with the same initial name
   but other extra chars instead of an ext. */
{
   return(strcmp((*E1) -> ff_name,(*E2) -> ff_name));
}

ExtComp(Entry *E1,Entry *E2)
/* Compares file names by their extension.  Returns a neg int, zero, or a
   pos int if the ext of E1 is lexicographically less than, equal to, or
   greater than the ext of E2.  A file name without an ext will be less than
   a file name with one. */
{
char *Ext1,*Ext2;	/* Exts of the files, or null strings if they did
			   not have exts */

   Ext1 = strchr((*E1) -> ff_name,'.');
   if (Ext1 == NULL)
      Ext1 = "";
   Ext2 = strchr((*E2) -> ff_name,'.');
   if (Ext2 == NULL)
      Ext2 = "";
   return(strcmp(Ext1,Ext2));
}

TDComp(Entry *E1,Entry *E2)
/* Compares the time and date of E1 and E2.  Returns -1, 0, or +1 if the
   file date if E1 is earlier than, the same as, or later than the file
   date of E2 */
{
   return(Sign((((long) ((*E1) -> ff_fdate) << 16) + (*E1) -> ff_ftime) -
          (((long) ((*E2) -> ff_fdate) << 16) + (*E2) -> ff_ftime)));
}

SizeComp(Entry *E1,Entry *E2)
/* Compares the size fo E1 and E2.  Returns -1, 0, or +1 if E1 is smaller
   than, the same size as, or larger than E2 */
{
   return(Sign((*E1) -> ff_fsize - (*E2) -> ff_fsize));
}

