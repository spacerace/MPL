// list.h RHS header for programs using list routines

void far *ListInit(unsigned members, char *name);
unsigned ListAdd(void far *plist, void far *object, unsigned size,
    char *name);
unsigned ListFind(void far *plist, char *name);
void far *ListDelete(void far *plist, char *name);
static void far *_ListDelete(void far *plist, unsigned bplitem);
void ListDestroy(void far *plist);
void ListSetObject(void far *plist, void far *object, unsigned size,
    char *name);
void far *ListGetObject(void far *plist, char *name);
void ListDeleteObject(void far *plist, char *name);
void ListSetName(void far *plist, char *name);
static void far *_ListInit(unsigned realsize);
unsigned ListGetNumItems(void far *plist);
unsigned ListGetItem(void far *plist);
unsigned ListItemGetNextItem(void far *plist, unsigned bplitem);
void far *ListItemGetObject(void far *plist, unsigned bplitem);
unsigned ListSave(void far *plist, char *filename);
void far *ListRestore(char *filename);

void error_exit(int err, char *msg);
void ListDump(void far *plist);
void ListDumpItem(unsigned temp);


