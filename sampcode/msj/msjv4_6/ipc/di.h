/* di.h RHS 7/15/89
 *
 */

#define	DIRINFOQNAME	"\\QUEUES\\DIRINFO.QUE"
#define	MAXSEGSIZE	(USHORT)65536

#if	defined(DICODE)

typedef struct _dirinforesult
	{
	USHORT					attributes;		/* attributes this request		*/
	PCH						filespec;		/* filespec to use				*/
	PCH						currentdir;		/* currentdir this request		*/
	PFILEFINDBUF			firstfile;		/* first result					*/
	PFILEFINDBUF			nextfile;		/* next result						*/
	USHORT					numfound;		/* number of files found		*/
	USHORT					errorval;		/* error value returned			*/
	struct _dirinforesult	*next;		/* related structure if found	*/
	} DIRINFORESULT;
typedef DIRINFORESULT FAR *PDIRINFORESULT;

typedef struct _requestheader
	{
	ULONG				RAMsem;					/* RAM semaphore for client	*/
	SEL					rselector;			/* client selector to results	*/
	HQUEUE				qhandle;				/* handle to server's queue	*/
	PID					qowner;				/* PID of server (queue owner)*/
	VOID FAR			*resultptr;				/* server pointer to work area*/
	SEL					serverhsel;			/* server selector to header	*/
	SEL					serverwsel;			/* server selector to results	*/
	PCH					currentdir;			/* client's dir in work area	*/
	PCH					requestspec;		/* next part of work area		*/
	USHORT				size;					/* curr. size header	segment	*/
	USHORT				resultsize;			/* new size of result segment	*/
	USHORT				totalresults;		/* total results found			*/
	USHORT				numRequests;		/* # of requests being made	*/
	DIRINFORESULT		resultArray[1];	/* request structures			*/
	} REQUESTHEADER;
typedef REQUESTHEADER FAR *PREQUESTHEADER;


void DiInit(PID *qowner, HQUEUE *qhandle);
void DiMakeRequest(PREQUESTHEADER *hptr, PCH filespec, USHORT att);
void DiSendRequest(PREQUESTHEADER header);
void DiGetNumResults(PREQUESTHEADER header,USHORT *numresults,USHORT *numrequests);
void DiDestroyRequest(PREQUESTHEADER *header);
char *DiGetResultFspec(PDIRINFORESULT result);
char *DiGetResultDir(PDIRINFORESULT result);
void DiGetResultHdl(PREQUESTHEADER header,USHORT requestnum,USHORT *num, PDIRINFORESULT *resulthdl);
void DiGetFirstResult(PDIRINFORESULT result, char *buffer);
void DiGetNextResult(PDIRINFORESULT result, char *buffer);
void DiGetFirstResultPtr(PDIRINFORESULT result, PFILEFINDBUF *ptr);
void DiGetNextResultPtr(PDIRINFORESULT result, PFILEFINDBUF *ptr);

#else

void DiInit(PID *qowner, HQUEUE *qhandle);
void DiMakeRequest(PVOID *hptr, PCH filespec, USHORT att);
void DiSendRequest(PVOID header);
void DiGetNumResults(PVOID header,USHORT *numresults,USHORT *numrequests);
void DiDestroyRequest(PVOID *header);
char *DiGetResultFspec(PVOID result);
char *DiGetResultDir(PVOID result);
void DiGetResultHdl(PVOID header,USHORT requestnum,USHORT *num,
		PVOID *resulthdl);
void DiGetFirstResult(PVOID result, char *buffer);
void DiGetNextResult(PVOID result, char *buffer);
void DiGetFirstResultPtr(PVOID result, PFILEFINDBUF *ptr);
void DiGetNextResultPtr(PVOID result, PFILEFINDBUF *ptr);

#endif


