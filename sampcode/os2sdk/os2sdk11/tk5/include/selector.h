#define	WM_QUERYTASKFOCUS	0x0058

typedef struct _WHOISINFO {
    USHORT	segNum;
    USHORT	mte;
    char	names[256];
} WHOISINFO;
typedef WHOISINFO far *PWHOISINFO;

int EXPENTRY IdentifyCodeSelector(USHORT, PWHOISINFO);
