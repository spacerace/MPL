/********************************* MAKEBOX.H ******************\
*
*       HEADER FILE for VIO DBCS SAMPLE program
*
\**************************************************************/

#define CBMAXDBCS    10     // Max size of DosGetDBCSEv buffer
#define VIOHANDLE     0     // default handle for full screen VIO

extern  int main(SHORT argc,char * *argv);
extern  VOID MakeBox( USHORT yTop,USHORT xLeft, USHORT yBottom,
                      USHORT xRight );
extern  VOID SaveArea(USHORT yTop,USHORT xLeft, USHORT yBottom,
                      USHORT xRight, BOOL fSave);
extern  VOID ParseFrameMsg(PBYTE pchMsg, USHORT cbMsg);


/******************** for OS2 1.2 US ********************
*       These defines are needed to run under 1.2 US
*********************************************************/



typedef struct t_VIOSCROLL { /* vioscroll */
    USHORT  cb;
    USHORT  type;
    USHORT  cnscrl;
} VIOSCROLL, FAR * PVIOSCROLL;

#define VS_GETSCROLL                  6


#ifndef MSG_APPL_SINGLEFRAMECHAR
#define MSG_APPL_SINGLEFRAMECHAR    130
#define MSG_APPL_DOUBLEFRAMECHAR    131
#define MSG_APPL_ARROWCHAR          132
#endif

#ifndef VCC_SBCSCHAR

#define VCC_SBCSCHAR      0   // Cell contains SBCS character
#define VCC_DBCSFULLCHAR  1   // Cell contains Full DBCS character
#define VCC_DBCS1STHALF   2   // Cell contains leading byte of DBCS
#define VCC_DBCS2NDHALF   3   // Cell contains trailing byte of DBCS

USHORT APIENTRY VioCheckCharType (PUSHORT pType, USHORT usRow,
         USHORT usColumn, HVIO hvio);
#endif
