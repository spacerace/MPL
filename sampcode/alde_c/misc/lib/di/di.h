#define TRUE -1
#define FALSE 0
#define FLAGSPECS "-/"	/* Characters that designate flags */
#define MAX_ATTS 21	/* Maximum number of attribute words + 1 */
#define ENTRY_INC 100	/* Increment that entry array is realloc()ed in */
#define NO_AT 0xff	/* Value that an attribute word can not have */
#define FA_REALDIR 0x80	/* Specifies psuedo-attrib of all dir entries except
			   "." and ".." */
#define ExTest(Test,Mess) if (Test) {fputs(Mess,stderr); exit(2);}
#define MORE_ROWS 23	/* Number of lines printed before pause */
#define CPDir(S) (S[0] == '.' && (S[1] == '\0' || S[1] == '.'))	/* Current
				or Parent Dir: True if S = "." or ".." */
#define UpCase(C) ('a' <= (C) && (C) <= 'z' ? (C) + 'A' - 'a' : (C))	/* If
  C is a lower case letter, returns it in upper case; otherwise returns C */
#define DriveSpeced(P) (P[0] != '\0' && P[1] == ':')	/* True if P has at
	least two letters and the second one is ':'.  If P is a path, this
	means that a drive is specified in the path */
#define Sign(x) ((x) > 0 ? 1 : (x) < 0 ? -1 : 0)  /* Signum function of x */
typedef struct ffblk * Entry;	/* Ptr to struct used by findfirst/next() */
typedef struct LinkedList {	/* List of directories/files to print */
   char Name[MAXPATH];
   struct LinkedList *Next;
} NameNode;
typedef struct {	/* Directory listing specs */
       LineEnts;	/* Number of entries/line to print in wide mode */
   char ListType,	/* Type of listing to print */
        Sort;	/* Type of sort to be done before sorting by name */
   int PrintFree;	/* Whether to print free space in heading */
   int PrintHeaders;	/* Whether to print headers */
   int RecSearch;	/* Whether to do a recursive search of dirs */
} ListType;
typedef struct {	/* List of directory entries */
   Entry *Entries;	/* Array of pointers to ffblk */
   NumEnts;		/* Number of entries */
} EntryListType;
void *MallocT(unsigned Size);	/* Memory allocation routines that test */
void *CallocT(unsigned NElem, unsigned Size);	/* whether there was */
void *ReallocT(void *Ptr, unsigned Size);	/*enough memory */
char *AttribWtoS(char A);
char *Insert(char *Target,char *Source,int Pos);
void SortList(EntryListType *EntryList,char Sort);
void GetArgs(int argc,char *argv[],int Attribs[],ListType *ListSpec,
 NameNode **Files);
void GetEntries(EntryListType *EntryList,int Attribs[],char Name[],int PrntCW);
void PrintHeader(NameNode *Files,int PrintFree);
void PrintList(EntryListType *EntryList,ListType *ListSpec,char *Dir);
GetEnvParams(int *Cnt,char *Vars[],char *Name);
More(int Lines);
