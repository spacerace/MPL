/*
 *	driver.h ->	Device Driver Header File
 */


/*
 *	Status Word Bits
 */

#define	Error	0x8000
#define	Busy	0x0100
#define	Done	0x0080


/*
 *	Media Descriptor Byte Bits
 */

#define	TwoSided	1
#define	EightSector	2
#define	Removable	4


/*
 *	Error Return Codes
 */

#define	WriteProtect	0
#define	UnknownUnit	1
#define	DeviceNotReady	2
#define	UnknownCommand	3
#define	crcError	4
#define	BadLength	5
#define	SeekError	6
#define	UnknownMedia	7
#define	SectorNotFound	8
#define	NoPaper		9
#define	WriteFault	10
#define	ReadFault	11
#define	GeneralFailure	12


/*
 *	structures
 */

typedef struct
	{
	char	Length;		/*  Request Header length	*/
	char	Unit;		/*  Unit Code			*/
	char	Command;	/*  Command Code		*/
	int	Status;		/*  Status			*/
	char	reserved[8];	/*  DOS Reserved Area		*/
	}
	Request;

typedef struct
	{
	Request	InitReq;	/*  Request Header		*/
	char	nUnits;		/*  number of units		*/
	long	EndAddr;	/*  Ending Address		*/
	long	BPBarray;	/*  ptr to BPB array		*/
	}
	InitParms;

typedef struct
	{
	Request	MediaReq;	/*  Request Header		*/
	char	MediaDesc;	/*  Media Descriptor		*/
	char	ReturnCode;	/*  Return Code			*/
	}
	MediaParms;

typedef struct
	{
	Request	BPBReq;		/*  Request Header		*/
	char	MediaDesc;	/*  Media Descriptor		*/
	long	Transfer;	/*  Transfer Address		*/
	long	BPBTable;	/*  ptr to BPB table		*/
	}
	BPBParms;

typedef struct
	{
	Request	InOutReq;	/*  Request Header		*/
	char	MediaDesc;	/*  Media Descriptor		*/
	long	Transfer;	/*  Transfer Address		*/
	int	Count;		/*  Byte/Sector Count		*/
	int	Start;		/*  Starting Sector Number	*/
	}
	InOutParms;

typedef struct
	{
	Request	ndInputReq;	/*  Request Header		*/
	char	Byte;		/*  Byte Read From Device	*/
	}
	ndInputParms;

typedef struct
	{
	Request	StatusReq;	/*  Request Header		*/
	}
	StatusParms;

typedef struct
	{
	Request	FlushReq;	/*  Request Header		*/
	}
	FlushParms;

typedef struct
	{
	int	BytesPerSector;
	char	SecsPerAllocUnit;
	int	ReservedSectors;
	char	FATCount;
	int	RootDirEntries;
	int	SectorsPerLogical;
	char	MediaDesc;
	int	SecsPerFAT;
	}
	BPB;

typedef struct
	{
	char	BootJump[3];
	char	Name[8];
	BPB	BootBPB;
	int	SecsPerTrack;
	int	HeadCount;
	int	HiddenCount;
	}
	BootSector;


/*
 *	externals
 */

extern Request	ReqHdr;		/*  current Request Header	*/

extern char	FarPeek();	/*  return value from long ptr	*/
extern int	FarPoke();	/*  store value at long ptr	*/

extern long	EndAddr();	/*  returns Ending Address	*/
extern long	Dword();	/*  converts ptr to DWORD	*/


/*
 *	END of driver.h
 */
