/*
    Entry points for COM port manipulation routines
    Created by Microsoft Corporation, 1989
*/
/*
    Must include global.h before this file
*/
int ComFlush(void);
int ComInit(COM comTerm);
USHORT ComRead(Line pli);
int ComWrite(char ch);
int ComClose(void);
int ComBreak(void);
int ComUnbreak(void);
int ComError(void);
