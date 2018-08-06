#include <stdio.h>
#include <dir.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "di.h"
char *Tokenize(char S[],char SepChar);
char *NextMatchFile(char FileSpec[],int Attributes);
NameNode *LastElem(NameNode *List);
NameNode *GetMatchFiles(char *Path,int Attribs);
NameNode *ExpandPath(char Path[]);
NameNode *CreateNameList(void);
NameNode *CatNameLists(NameNode *Target,NameNode *Source);
void AddFileWC(char File[]);
void AddNode(NameNode *Node);
void InsertList(NameNode *Target,NameNode *Source);
void AddCDirs(NameNode *List);
void AddAllWC(char Path[]);
void FirstWild(char Path[],char *Head,char *WildSpec,char *Tail);
void AddString(NameNode *List,char *S);
void DeleteNext(NameNode *List);
void FreeList(NameNode *List);
void MakePathsFull(NameNode **List);
void AddFlag(char *Flag,int *Attribute,ListType *ListSpec);
FlagErr(int ErrWord);
GetFullPath(char *Path);
NullPath(char *Path);
IsRoot(char *Dir);
GetFullDir(char *Path);
IsDir(char *Path);
FindInt(int Arr[],int Targ);

void GetArgs(int argc,char *argv[],int Attribs[],ListType *ListSpec,
 NameNode **Files)
/* Converts an argument list to an array of search attribs (Attribs),
   a list of filespecs (Files), and a set of listing type specifications
   (ListSpec).	If any attrib flags are given, the previous values in
   Attribs are erased.	If any filespecs are given, the previous Files
   list is replaced & freed. */
{
int I,
Attrib, 	/* Search attributes given by one argument */
NumAtts,	/* Number of attribute groups found */
FileSpecGiven,	/* Whether any argument was a filespec */
OldNum; 	/* Number of attributes previously given in array */
NameNode *PathList,	/* List of paths produced by expanding the wildcards
			   in a filespec arg */
*List,		/* List of paths produced from filespec args */
*CurFile;	/* Current end of the path list */

   List = CurFile = CreateNameList();	/* Initially set List and CurFile to
					   point to an empty node */
   OldNum = FindInt(Attribs,NO_AT);	/* Find number of attribs previously
					   in Attribs */
   NumAtts = 0; 	/* Initialize current number of attribs to 0 */
   FileSpecGiven = FALSE;
   for (I = 1; I < argc; I++) { 	/* For each argument */
      if (strchr(FLAGSPECS,argv[I][0]) != NULL) { /* If arg is a flag group */
	 AddFlag(argv[I],&Attrib,ListSpec);	/* Interpret flag
						group and set variables */
	 if (Attrib != NO_AT) { 	/* If group contained attributes */
	    ExTest(NumAtts == MAX_ATTS - 1,"Too many attribute groups.");
	    Attribs[NumAtts++] = Attrib; /* Add attrib and-term to Attribs */
	 }
      }
      else {
	 FileSpecGiven = TRUE;
	 PathList = ExpandPath(argv[I]);	/* Expand wildcards in arg */
	 if (PathList == NULL)
	    fprintf(stderr,"Invalid directory in path \"%s\".\n",argv[I]);
	 else
	    CurFile = CatNameLists(CurFile,PathList);	/* Add all paths
			indicated by the arg to the total paths list */
      }
   }
   CurFile = List;		/* Get rid of initial empty node */
   List = List -> Next;
   free(CurFile);
   if (FileSpecGiven) { 	/* If filespecs found, replace original path
				   list with new list */
      FreeList(*Files);
      *Files = List;
   }
   if (ListSpec -> RecSearch)	/* If recursive search flag was given, find
    all subdirs of the lowest-level dir given in each path and add them to
    the list, with each added path having the file name of the path it was
    derived from */
      AddCDirs(*Files);
   if (NumAtts == 0)	/* If no attrib flags were given, attrib array is
			   unchanged; set num of attribs to original num */
      NumAtts = OldNum;
   Attribs[NumAtts] = NO_AT;	/* Mark end of attrib array */
}

void AddFlag(char *Flag,int *Attribute,ListType *ListSpec)
/* Sets Attribute, ListSpec, and More() according to the flags in
   Flag.  If Flag contains any attribute flags, they are combined to produce
   attribute and-term Attribute.  Lower case attrib flags are interpreted
   as 'true' (a file must have that attribute to match) and the corresponding
   bit (as indicated by the FA_* constants) in the low byte of Attribute is
   set.  Upper case flags are interpreted as 'false' (a file can not have
   that attrib to match) and the corresponding bit in the high byte of
   Attribute is set.  The 'e' flag (everything) sets Attribute to 0. An error
   message is printed if both a true and a false flag are given in the same
   and-term.
   The value of RecSearch and the status of More() are toggled if their
   corresponding flags are found.  The list specs ListType, Sort, and
   LineEnts take on the value of the last flag that applies to them.
   No error message is printed if more than one flag for one of them is
   found while the env var and command line args are being read, so that
   a command line flag can override an env var flag without a message.
   If an illegal flag is given, an error message is printed. */
{
static char FlagDesc[] =	/* Printed if any illegal flags were given */
"     DI: Directory Listing Utility   by  John H. DuBois III	v1.1\n"
"Flags are:   Pause during listing;  list Long, Wide, or with Full path;\n"
"print 1-6 entries / line if listing wide;  search directories reCursively;\n"
"sort first by dIr attrib, eXtension, Time and date, siZe, or Name only;\n"
"Kill headers;	print Unused (free) space on device in main header;\n"
"print files with attribs: Everything (print all files), or\n"
"Read only, Hidden, System, Directory, Volume label, Archive.\n";

int I,
Attrib, 	/* Attrib that one flag translated to */
AttGiven,	/* Whether any attributes were given in the flag list */
BadFlag;	/* Whether any illegal flags were found in the flag list */
char F; 	/* Upper case version of flag being interpreted */

   *Attribute = 0;	/* Initialize to no attributes */
   AttGiven = BadFlag = FALSE;
   if (Flag[1] == '\0') /* If arg started with a flag specifier but no flags
			   were given */
      fputs("Empty flag.",stderr);
   else
      for (I = 1; Flag[I] != '\0'; I++) {	/* For each flag */
	 F = UpCase(Flag[I]);
	 Attrib = NO_AT;	/* Initially set Attrib to a value that no
	  attribute term can have, so that it can be determined if Attrib was
	  set by a flag */
	 switch(F) {
	    case 'F' :
	    case 'L' :
	    case 'W' :	ListSpec -> ListType = F; break;
	    case 'X' :
	    case 'Z' :
	    case 'N' :
	    case 'I' :
	    case 'T' :	ListSpec -> Sort = F; break;
	    case '1' :
	    case '2' :
	    case '3' :
	    case '4' :
	    case '5' :
	    case '6' :	ListSpec -> LineEnts = F - '0'; break;
	    case 'P' :	More(More(-2)); break;
	    case 'U' :	ListSpec -> PrintFree = !ListSpec -> PrintFree; break;
	    case 'K' :	ListSpec -> PrintHeaders = !ListSpec -> PrintHeaders;
			   break;
	    case 'C' :	ListSpec -> RecSearch = !ListSpec -> RecSearch; break;
	    case 'R' :	Attrib = FA_RDONLY;  break;
	    case 'H' :	Attrib = FA_HIDDEN;  break;
	    case 'S' :	Attrib = FA_SYSTEM;  break;
	    case 'V' :	Attrib = FA_LABEL;   break;
	    case 'D' :	Attrib = FA_DIREC;   break;
	    case 'A' :	Attrib = FA_ARCH;    break;
	    case 'E' :	Attrib = 0;	     break;
	    default :	fprintf(stderr,"Bad flag: %c\n",Flag[I]);
			BadFlag = TRUE;
	 }
	 if (Attrib != NO_AT) { /* Flag list contained attribute flags */
	    AttGiven = TRUE;
	    if (islower(Flag[I])) {	/* 'true' flag was given. Check
	     whether the corresponding 'false' flag has already been set */
	       if (!FlagErr((*Attribute >> 8) & Attrib))
		  *Attribute |= Attrib; /* Set the correct bit in lo byte */
	    }
	    else	/* 'false' flag was given. Check whether the
	     corresponding 'true' flag has already been set */
	       if (!FlagErr(*Attribute & Attrib))
		  *Attribute |= Attrib << 8;	/* Set the bit in hi byte */
	 }
      }
   if (!AttGiven)	/* If no attrib flags found, indicate this */
      *Attribute = NO_AT;
   if (BadFlag) /* If any bad flags were found, print list of valid flags */
      fputs(FlagDesc,stderr);
}

FlagErr(int ErrWord)
/* ErrWord is produced by ANDing the one-byte version of an attrib with
   a word containing previously given attribs, with the inverse byte
   shifted to the low byte.  If an attribute flag with the opposite
   value has already been read, the corresponding bit in ErrWord will
   be set.  If any bits in ErrWord are set, an error message and the flag
   are printed and TRUE is returned;  otherwise FALSE is returned */
{

   if (ErrWord != 0) {
      fprintf(stderr,"Conflicting attribute flag: %s\n",
       AttribWtoS((char) ErrWord));
      return(TRUE);
   }
   else
      return(FALSE);
}

GetEnvParams(int *Cnt,char *Vars[],char *Name)
/* Convert the value of environment var Name to a list of parameters with
   the same format as argv and argc.  Parameters in the var are separated
   by spaces and tabs. A copy of the var value is made with
   strdup(); the space after each param is converted to a null char; and a
   pointer to the start of each param is put in Vars. The first element in
   Vars is set to point to the start of the copied string so it can be freed.
   Cnt is set to the number of args found + 1.	If the var is not found in
   the environment, Cnt is set to 0 and FALSE is returned; otherwise TRUE is
   returned.
*/
{
char *Envar,	/* Found environment var */
*Value; 	/* Copied env var */
int Num;	/* Number of args found so far + 1 */

   *Cnt = Num = 0;
   if ((Envar = getenv(Name)) == NULL)	/* Get env var */
      return(FALSE);	/* If not found return FALSE */
   else {
      Value = strdup(Envar);	/* Copy env var value */
      Vars[0] = Value;	/* Make first pointer point to string */
      Num = 1;	/* Start with 1 to account for pointer set to Value */
      Vars[1] = strtok(Value," \t");	/* Tokenize Value and put
					   pointers in Vars */
      while (Vars[Num] != NULL)
	 Vars[++Num] = strtok(NULL," ");
      *Cnt = Num;
      return(TRUE);
   }
}

NameNode *ExpandPath(char Path[])
{
NameNode *List, /* Start of the filespec list */
 *Temp, 	/* A node that is to be freed */
 *Last, 	/* End of the filespec list */
 *NewDirs;	/* The directory list that a wildcard dir spec expanded to */
char Head[MAXPATH],	/* The portion of the path before the first component
			   containing wildcards */
Wild[MAXPATH],	/* The first component of the path containing wildcards */
Tail[MAXPATH],	/* The portion of the path after the first component containing
		   wildcards */
Dir[MAXPATH];	/* The current wildcard dir spec being expanded */

   Last = List = CreateNameList();	/* Initialize List with an empty node
					   and make Last be the same node */
   strupr(Path);	/* Convert Path to all upper case */
   strcpy(List -> Name,Path);	/* Copy Path to the first node in the list */
   AddAllWC(List -> Name);	/* Expand all implicit wildcard in the path */
   FirstWild(List -> Name,Head,Wild,Tail);	/* Find the first wildcard
    specification in the path that must be expanded to a list of directories */
   while (Tail[0] != '\0') {	/* As long as Tail is not a null string, Tail
    will contain the last component of the path (the file name).  Path
    expansion should stop when the file name is reached since only the dir
    wildcards are to be expanded.  Because all final paths will have the same
    number of dirs expanded from wildcards in the path, as soon as one
    completely expanded path is found all paths will have been completely
    expanded */
      strcpy(Dir,Head); /* Make Dir consist of the Head (which will contain no
       wildcards) as dir path and Wild as the filename */
      strcat(Dir,Wild);
      NewDirs = GetMatchFiles(Dir,FA_REALDIR);	/* Get the full paths of all
						   dir names that match Dir */
      AddString(NewDirs,Tail);	/* Add the unexpanded portion of the path to
				   all dirs found */
      Last = CatNameLists(Last,NewDirs);	/* Add the new dirs found to
						   the end of the dir list */
      Temp = List;	/* Get rid of the first element of the list (the dir
			   that has been expanded */
      List = List -> Next;
      free(Temp);
      FirstWild(List -> Name,Head,Wild,Tail);	/* Split up the next path in
						   the list */
   }
   MakePathsFull(&List);	/* Convert relative paths to full path names
				   and get rid of invalid paths */
   return(List);
}

void AddCDirs(NameNode *List)
/* Adds child dirs to List with the same file name.  The name of each path in
   List is split into directory and filename.  All subdirs of each dir are
   found and inserted in the list immediately after the parent dir, with
   the file name from the original path appended to them.  The same proceedure
   is performed on each new dir found, so that at the end all decendant dirs
   of the dir in the original path will have been added to the list with the
   original file name appended. */
{
char Dir[MAXPATH],	/* The path being searched for subdirs, with the
			   original filename replaced by "*.*" */
*NameStrt,		/* Start of the filename of the path */
Name[13];		/* The original filename of the path */
NameNode *SubList;	/* List of subdirs found */

   Name[13] = '\0';	/* If a file name was more than 12 chars, it will be
			truncated.  Ensures that it will be null terninated */
   while (List != NULL) {
      strcpy(Dir,List -> Name); 	/* Copy path so it can be altered */
      NameStrt = strrchr(Dir,'\\');	/* Find start of file name */
      strncpy(Name,NameStrt,12);	/* Save original file name */
      strcpy(NameStrt + 1,"*.*");	/* Replace it in path with "*.*"
					   so all subdirs will be found */
      SubList = GetMatchFiles(Dir,FA_REALDIR);	/* Find subdirs, but not
						   "." and ".." entries */
      AddString(SubList,Name);	/* Add orig file name to all subdirs found */
      InsertList(List,SubList); /* Insert list of child dirs immediately after
	parent dir, so they will be the next to be searched for child dirs */
      List = List -> Next;
   }
}

NameNode *GetMatchFiles(char *Path,int Attribs)
/* Finds & makes a list of all files that match Path and Attribs. Returns a
   pointer to the list */
{
NameNode *Temp, /* End of the list of matching files */
*List;	/* List of matching files */
char *File;	/* The name of a matching file */

   Temp = List = CreateNameList();	/* Initialize Temp and List to an
					   empty node */
   File = NextMatchFile(Path,Attribs);	/* Get the first matching file */
   while (File != NULL) {	/* Do until no more matching files */
      AddNode(Temp);		/* Add a new empty node to the list */
      Temp = Temp -> Next;	/* Move Temp to point to it */
      strcpy(Temp -> Name,File);	/* Copy the name of the matching file
					   to the new node */
      File = NextMatchFile(NULL,0);	/* Get next matching file */
   }
   Temp = List; 	/* Get rid of the first empty node */
   List = List -> Next;
   free(Temp);
   return(List);
}

NameNode *CreateNameList()
/* Creates a new NameNode, sets its Next pointer to NULL, and returns
   a pointer to it */
{
NameNode *Temp;

   Temp = (NameNode *) MallocT(sizeof(NameNode));
   Temp -> Next = NULL;
   return(Temp);
}

FindInt(int Arr[],int Targ)
/* Searches array Arr for Targ & returns its index */
{
int I;

   for (I = 0; Arr[I] != Targ; I++) ;
   return(I);
}

void FreeList(NameNode *List)
/* Frees all of the nodes in List */
{
NameNode *Temp;

   while (List != NULL) {
      Temp = List;
      List = List -> Next;
      free(Temp);
   }
}

void AddAllWC(char Path[])
/* Converts all implicit wildcards in Path, except for those in the last
   file name given, to explicit wildcards.  A filename
   is assumed to exist between every two backslashes and after the last
   backslash.  If Path contains no backslashes, it is assumed to be one
   filename.  */
{
char NewPath[MAXPATH],	/* The new path as it is being created from Path */
*FSpec, 	/* The file name being converted */
Temp[12];
int NameStart;

   strcpy(NewPath,Path);	/* Copy Path to NewPath so that possible
			drive spec and leading backslash will be in NewPath */
   if (Path[0] != '\0' && Path[1] == ':')	/* Find the first file name
    in Path (at index 0 or 2, depending on whether a drive was specified */
      NameStart = 2;
   else
      NameStart = 0;
   FSpec = Tokenize(Path + NameStart,'\\');	/* Find the first filename in
						   Path */
   if (NewPath[NameStart] == '\\') {	/* If Path began with a backslash, the
    first name returned by Tokenize will by the null string that preceded the
    backslash, which should not be expanded.  Call Tokenize again, and make
    NewPath end immediately after the first backslash */
      FSpec = Tokenize(NULL,'\\');
      NewPath[NameStart + 1] = '\0';
   }
   else
      NewPath[NameStart] = '\0';	/* Remove everything except the drive
					   spec, if given, from NewPath */
   while (FSpec != NULL) { /* While there are filenames left to be converted */
      strcpy(Temp,FSpec);	/* Copy the file name to Temp so it can be
				   converted */
      FSpec = Tokenize(NULL,'\\');	/* Find next file name */
      if (FSpec != NULL)	/* Convert filename only if it is not the last
				   file name */
	 AddFileWC(Temp);
      strcat(NewPath,Temp);	/* Add name to new path */
      strcat(NewPath,"\\");	/* Add a backslash */
   }
   NewPath[strlen(NewPath) - 1] = '\0'; /* Get rid of the last bckslsh added */
   strcpy(Path,NewPath);	/* Copy converted path back to Path */
}

void AddFileWC(char File[])
/* Converts the implicit wildcards in File to explicit wildcards.  An implicit
   file name of "*" exists if File has no file name, and an implicit file
   extension of ".*" exists if File has no file ext. If File is "." or "..",
   it is not altered. */
{
char *Ext;	/* Start of the file ext of File */

   Ext = strchr(File,'.');
   if (Ext == File && !CPDir(File) || File[0] == '\0')	/* If File starts with
    a dot and is not the current or parent dir ("." or ".."), then it has no
    file name; make the file name "*". */
      Insert(File,"*",0);
   if (Ext == NULL)	/* If there was no dot in File, it has no file ext;
			   make the file ext ".*" */
      strcat(File,".*");
}

char *Tokenize(char S[],char SepChar)
/* Converts S to tokens. On the first call, S should be the string to tokenize;
   on subsequent calls it should be null.  S is a string of tokens separated
   by single instances of SepChar.  If two SepChar's occur adjacent to each
   other, a null token ("") is assumed to exist between them.  If S has a
   leading or trailing SepChar, a null token is assumed to exist before or
   after the string, respectively.  SepChar can be different between calls.
   After each call to Tokenize, a null is written immediately after the token
   found and a pointer to its start is returned.  Null tokens are returned as
   empty strings.  When no more tokens are found, a NULL pointer is returned.
   */
{
static char *List;	/* Start of list of tokens */
char *Pos,	/* Position of end of current token */
*Token; /* Start of token */

   if (S != NULL)  /* If first call to function, set List to string given */
      List = S;
   Token = List;   /* Current token starts at start of List */
   if (List != NULL) {			/* If some tokens are left */
      Pos = strchr(List,SepChar);	/* Find end of token */
      if (Pos == NULL)	/* If no more tokens, the next call will return NULL */
	 List = NULL;
      else {
	 *Pos = '\0';	/* Otherwise, add null termination to current token
			   and make List point to start of next token */
	 List = Pos + 1;
      }
   }
   return(Token);
}

void FirstWild(char Path[],char *Head,char *WildSpec,char *Tail)
/* Finds the first wildcard filename in Path.  The portion of the path
   preceeding that filename (if any), including trailing backslash, is
   copied to Head.  The wildcard filename (without leading or trailing
   backslashes) is copied to WildSpec.	The portion of the path after
   the wildcard filename (including leading backslash) is copied to Tail */
{
char *PostSep,
*PreSep,
*FirstWC;

   strcpy(Head,Path);	/* Copy Path to Head so it can be altered */
   FirstWC = Head + strcspn(Head,"*?"); /* Make FirstWC point to the first
					   wildcard in Head */
   if (*FirstWC == '\0') {	/* If no wildcards found, leave the entire
		string in Head and make WildSpec and Tail null strings */
      WildSpec[0] = '\0';
      Tail[0] = '\0';
   }
   else {
      PostSep = strchr(FirstWC,'\\');	/* Find the first backslash after the
					   first wildcard */
      if (PostSep != NULL) {	/* If there was one, then it is the start of
       the tail and the end of the first wildcard spec */
	 strcpy(Tail,PostSep);
	 *PostSep = '\0';
      }
      else
	 Tail[0] = '\0';	/* If not, make Tail a null string */
      PreSep = strrchr(Head,'\\');	/* Find the first backslash before the
					   the first wildcard */
      if (PreSep != NULL) {	/* If there was one, the char after it is the
      start of the the first wildcard spec and the end of the Head */
	 strcpy(WildSpec,PreSep + 1);
	 *(PreSep + 1) = '\0';
      }
      else {
	 strcpy(WildSpec,Head); /* If not, everything in Head is the first
				   wildcard spec */
	 Head[0] = '\0';
      }
   }
}

void AddString(NameNode *List,char *S)
/* Adds string S to every name in List */
{
   while (List != NULL) {
      strcat(List -> Name,S);
      List = List -> Next;
   }
}

void InsertList(NameNode *Target,NameNode *Source)
/* Inserts list Source into the list in which Target is a node, immediately
   after target. */
{
NameNode *Next;

   if (Source != NULL) {
      Next = Target -> Next;
      Target -> Next = Source;
      LastElem(Source) -> Next = Next;
   }
}

NameNode *CatNameLists(NameNode *Target,NameNode *Source)
/* Target points to the last node in a list.  Catenates Source to Target
   and returns a pointer to the last node of the new list */
{

   ExTest(Target == NULL,"Error: Null Target in CatNameLists().");
   Target -> Next = Source;
   return(LastElem(Target));
}

NameNode *LastElem(NameNode *List)
/* Returns a pointer to the last element of List */
{
   ExTest(List == NULL,"Error: Null pointer passed to LastElem()");
   while(List -> Next != NULL)
      List = List -> Next;
   return(List);
}

void AddNode(NameNode *Node)
/* Creates a new node; links Node to it; and makes its Next pointer null */
{
   ExTest(Node == NULL,"NULL pointer passed to AddNode");
   Node -> Next = (NameNode *) MallocT(sizeof(NameNode));
   Node -> Next -> Next = NULL;
}

char *NextMatchFile(char FileSpec[],int Attributes)
/* Finds files that match FileSpec and have all attributes indicated by
   Attributes, and returns the full path of the file,
   including the drive and directory given in FileSpec. On the first call,
   FileSpec and Attribs are given; on subsequent calls FileSpec should be
   NULL.  The found file is put in a static buffer and a pointer to the
   buffer is returned. If no matching file is found, a null pointer is
   returned. */
{
static char File[MAXPATH],	/* Full file name buffer */
Drive[MAXDRIVE],Dir[MAXDIR];	/* Drive and dir given in the filespec, to
				   be added to each file found */
static struct ffblk FFBlk;	/* Structure for directory searching routine */
char Name[MAXFILE],Ext[MAXEXT]; /* Name and extension of found file */
int Done;	/* Whether dir searching routine found a matching file */
static int Attribs,	/* Saves Attributes between calls */
RealDir;	/* Search for real dirs only */

   if (FileSpec != NULL) {	/* First call to function */
      fnsplit(FileSpec,Drive,Dir,NULL,NULL); /* Get drive & dir of filespec */
      if (Attributes & FA_REALDIR) {	/* If one of the attributes given is
       the psuedo-attrib FA_REALDIR (match real dirs only, not "." or "..")
       then set the variable that will save the search attribs between calls
       to Attributes, with the psuedo-attrib replaced with the dir attrib,
       and set RealDir */
	 Attribs = (Attributes & ~FA_REALDIR) | FA_DIREC;
	 RealDir = TRUE;
      }
      else {	/* Save attribs */
	 Attribs = Attributes;
	 RealDir = FALSE;
      }
      Done = findfirst(FileSpec,&FFBlk,Attribs);  /* Find first match file */
   }
   else 	/* Use FFBlk set up by previous call */
      Done = findnext(&FFBlk);
   while (!Done && ((FFBlk.ff_attrib & Attribs) != Attribs ||
    RealDir && CPDir(FFBlk.ff_name)))	/* Find next file that has all of the
     attribs specified in Attribs and, if RealDir is set, is not the current
     or parent dir entry ("." or "..") */
      Done = findnext(&FFBlk);
   if (Done)	/* If no more matching files return NULL */
      return(NULL);
   else {
      fnsplit(FFBlk.ff_name,NULL,NULL,Name,Ext); /* Split matching file name
						    into Name & Extension */
      fnmerge(File,Drive,Dir,Name,Ext); /* Merge Name & Ext with previously
					   given Drive & Dir */
      return(File);
   }
}

void MakePathsFull(NameNode **List)
/* Converts the relative paths given in List to full path names, and deletes
   invalid paths from the list. If paths are removed from the front of the
   list, List is changed to point to the new start of the list */
{
NameNode  *Last,	/* The last path converted */
 *Temp; 	/* Pointer to list as it is stepped through */

   while (*List != NULL && !GetFullPath((*List) -> Name)) {	/* Find the first valid
    path in List (deleting invalid ones) and convert it to a full path */
      Temp = *List;
      *List = (*List) -> Next;
      free(Temp);
   }
   if (*List != NULL) { /* If any valid dirs were given */
      Temp = (*List) -> Next;	/* Start with the next node, since the current
				   one has already been converted */
      Last = *List;	/* The previous loop insured that there is a 'last'
       node that can have its Next pointer moved beyond the current node, in
       order to delete that node */
      while (Temp != NULL) { /* For all of the rest of the paths in the list */
	 if (GetFullPath(Temp -> Name)) {	/* If path is valid */
	    Last = Temp;		/* Make Last point to the current
					   path, then move to the next path */
	    Temp = Temp -> Next;
	 }
	 else { /* If the path is not valid */
	    DeleteNext(Last);	/* Delete the current path from the list
	     by making the Next pointer of the previous node point the next
	     node and freeing the current node */
	    Temp = Last -> Next;	/* Move to the next path */
	 }
      }
   }
}

void DeleteNext(NameNode *List)
/* Deletes the node that follows List by making List's Next pointer point to
   the node beyond the next node, and freeing the deleted node */
{
NameNode *Temp; /* Node to be freed */

   ExTest(List == NULL || List -> Next == NULL,"Null pointer to DeleteNode");
   Temp = List -> Next;
   List -> Next = Temp -> Next;
   free(Temp);
}

GetFullPath(char *Path)
/*
   Given a relative path to a file or directory, finds the full path name
   of the file or directory and replaces the original string with it.
   If a drive is not specified in the path, the current drive is used.
   Returns TRUE if the file is found, otherwise FALSE.
*/
{
char Drive[MAXDRIVE],Dir[MAXDIR],FileName[MAXFILE],Ext[MAXEXT], /* Used to
					split up and reassemble path */
FindPath[MAXPATH];	/* New path */
int Comps;	/* Components found in path */

   strcpy(FindPath,Path);	/* Make working copy of Path */
   if (FindPath[0] == '\0' || FindPath[1] == ':' && strlen(FindPath) == 2)
	/* If no path or only a drive is given, make path be the current
	    directory on the default or given drive. */
      strcat(FindPath,".\\");
   else if (IsDir(FindPath) && FindPath[strlen(FindPath) - 1] != '\\')
     /* If path is a directory, append a backslash to it to indicate this */
      strcat(FindPath,"\\");
   Comps = fnsplit(FindPath,Drive,Dir,FileName,Ext);	/* Split path into its
							   components */
   strcpy(FindPath,Drive);	/* Generate path pointing to given directory
				   or parent dirctory of given file */
   strcat(FindPath,Dir);
   strcat(FindPath,".");
   if (strcmp(FileName,".") == 0 || !GetFullDir(FindPath))	/* Convert
    directory path to full dir name; if dir specified in path was invalid, or
    specified a parent directory beyond the root directory, return FALSE */
      return(FALSE);
   if (!(Comps & DRIVE)) {	/* If a drive was not specified, make path
				   drive be the current drive */
      Drive[0] = getdisk() + 'A';
      Drive[1] = ':';
   }
   if (!(Comps & FILENAME))	/* If no file name given, make file name "*" */
      strcpy(FileName,"*");
   if (!(Comps & EXTENSION))	/* if no file ext given, make file ext ".*" */
      strcpy(Ext,".*");
   fnmerge(Path,Drive,FindPath,FileName,Ext); /* Create full path from comps */
   return(TRUE);
}

GetFullDir(char *Path)
/* Given a path to a directory, converts it to a full directory name. If
   given path did not point to a dir, returns FALSE; else returns TRUE */
{
struct ffblk FFBlk;	/* Structure for directory searching routine */
char Dir[MAXPATH];	/* Temp storage of full path */

   if (!IsDir(Path))	/* If invalid directory, return FALSE */
      return(FALSE);
   Dir[0] = '\0';		/* Clear directory string */
   while (!findfirst(Path,&FFBlk,0x3f)) {	/* Get name of directory
     pointed to by Path until no more directories can be found */
      if (!IsRoot(Path)) {	/* Check whether directory name found is the
	name of the dir that is at the root of the drive given in the path,
	which it could be if the drive is a substituded drive. If not,
	insert the name followed by a backslash at the begining of Dir */
	 Insert(Dir,"\\",0);
	 Insert(Dir,FFBlk.ff_name,0);
      }
      strcat(Path,"\\..");	/* Append "\.." to Path so it will point
       to the parent directory (if it exists) of the dir just found */
   }
   Insert(Dir,"\\",0);	/* Make Dir have a leading backslash */
   strcpy(Path,Dir);	/* Move full dir name to Path */
   return(TRUE);
}

IsRoot(char *Dir)
/* Returns TRUE if Dir is a path to the root of a real or substituted drive,
   or is an invalid directory. Otherwise returns FALSE. */
{
struct ffblk FFBlk;	/* For dir searching routine */
char Temp[MAXPATH];	/* Path to parent of Dir, if it exists */

   strcpy(Temp,Dir);
   strcat(Temp,"\\..\\*.*");	/* Make Temp point to parent of Dir */
   return(findfirst(Temp,&FFBlk,0x3f)); /* Return TRUE if Dir has no parent. */
}

IsDir(char *Path)
/* Returns true if Path is a directory, otherwise false. */
{
char Temp[MAXPATH];	/* Test string */
struct ffblk FFBlk;	/* For directory searching routine */

   strcpy(Temp,Path);
   if (Temp[0] != '\0' && Temp[strlen(Temp) - 1] != '\\')	/* If string is
    not null and does not end in a backslash, append a backslash to it so it
    will specify a directory */
      strcat(Temp,"\\");
   strcat(Temp,"*.*");	/* Make temp specify any file in the given directory */
   return(!findfirst(Temp,&FFBlk,0x3f) || NullPath(Path));	/* If any files
     are found, or it is a null path (contains only single .'s or backslashes)
     it must be a directory. The only directory that does not have
     any files is a root dir without ordinary files, directories, system files
     or volume label. A path to such a directory must be a null path. */
}

NullPath(char *Path)
/* Checks whether Path is a null path (contains only single '.s and
   backslashes). Returns TRUE if so, else returns FALSE. */
{
char Temp[MAXPATH];


   if (DriveSpeced(Path))
      strcpy(Temp,Path + 2);
   else
      strcpy(Temp,Path);
   strcat(Temp,"\\");	/* Pad string with '\' so strstr will work despite
			   its bug */
   return(strspn(Temp,"\\.") == strlen(Temp) && strstr(Temp,"..") == NULL);
     /* Check that temp contains only \'s and .'s, and no two .'s are
	adjacent */
}

