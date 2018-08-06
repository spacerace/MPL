/*
    global.h -- global type definitions and abbreviations
    Created by Microsoft Corporation, 1989
*/
typedef struct _COM { 			/* com */
	char	szPort[5];
	USHORT	usBaud;
	BYTE	bParity, bData, bStop;
	BOOL	fWrap;
	BOOL	fSoftware;
	BOOL	fHardware;
} COM; 

#define MAXLINELEN		80
typedef struct _LINEINFO	{	/* li */
	char szText[MAXLINELEN];
	USHORT cch;
	BOOL fComplete;
	BOOL fDrawn;
} LineInfo;
typedef LineInfo	far	*Line;

#define		Min(x,y)	(((x) < (y)) ? (x) : (y))
#define		Max(x,y)	(((x) > (y)) ? (x) : (y))
