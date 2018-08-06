/*
    circleq.h -- Circular Queue module entry points
    Created by Microsoft Corporation, 1989
*/

/*
    Remember to include global.h first
*/
/*
    Define queue specifications
*/
#define	QUEUESIZE	150
/*
    Define queue element type -- in global.h
*/
/*
    Queue manipulation macros and functions
*/
void QueInit(void);
void QueAdvance(int n);
Line QueQuery(int LineNum);
BOOL QueInsertLine(Line pli);
BOOL QueCompleteLine(void);
int  QueUpdateHead(int nRows, BOOL bPage, BOOL bPaging);
Line QueLastLine(void);
int  QuePageUp(int nRows);
