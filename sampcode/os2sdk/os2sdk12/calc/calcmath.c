/****************************** Module Header *********************************/
/*									      */
/* Module Name:  calcmath.c - Calc application				      */
/*									      */
/* OS/2 Presentation Manager version of Calc, ported from Windows version     */
/*									      */
/* Created by Microsoft Corporation, 1987				      */
/*									      */
/******************************************************************************/

#define INCL_WINCLIPBOARD
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern BOOL fValueInMemory;
extern CHAR chLastKey;
extern CHAR szreg1[20], szreg2[20], szmem[20];
extern HWND hwndCalc;
extern CHAR szregx[];
extern HAB  hab;
BOOL   fReadNumber;
CHAR   PendingOperation;
BOOL   fFirstOperand, fError;
CHAR   szresult[20];
SEL    sel;

#define tolower(x)   (((x) >= 'A') && ((x)<='Z')) ? (x) - 'A' + 'a' : (x)
#define MAXINT	(double)999999999
#define MININT (double)-999999999
#define ABS(x)		(((x) >= (double)0) ? (x) : (-(x)))


/******************************************************************************/
extern VOID UpdateDisplay( VOID);
extern BOOL InterpretChar( CHAR);

VOID AppendNumber( BYTE);
VOID BinaryOperator( CHAR);
VOID Clear( VOID);
VOID DataXCopy( VOID);
VOID DataXPaste( VOID);
VOID Equilibrate( VOID);
VOID Evaluate( BYTE);
VOID FarStrcpy( PSZ, PSZ);
NPCH ftoa( double);
VOID InitCalc( VOID);
VOID MClear( VOID);
VOID MMinus( VOID);
VOID MPlus( VOID);
VOID Negate( VOID);
VOID Number( CHAR);
VOID Percent( VOID);
VOID reverse( NPCH);
VOID Simplify( VOID);
VOID SquareRoot( VOID);


/******************************************************************************/
VOID FarStrcpy( pszDest, pszSrc)
PSZ  pszDest, pszSrc;
{
    while( *pszDest++ = *pszSrc++);
}

/******************************************************************************/
VOID
reverse( s)

NPCH s;
{
    CHAR ch;
    register INT iHead, iTail;

    for (iHead = 0, iTail = strlen(s) - 1; iHead<iTail; iHead++, iTail-- ) {
	ch = s[iHead];
	s[iHead] = s[iTail];
	s[iTail] = ch;
    }
}

/******************************************************************************/
NPCH
ftoa( dblNum)

double dblNum;
{
    sprintf( szresult, "%.8f", dblNum );
    return (szresult);
}


/******************************************************************************/
VOID
Negate()
{
    CHAR sztemp[ 20 ];

    if (szreg1[0] ==  '-')
	strcpy(szreg1, (&szreg1[1]));		     /* get rid of minus sign */
    else if (szreg1[0] != '0' || (strlen(szreg1) > 2)) { /* can't negate zero */
	     sztemp[0] = '-';
	     strcpy(&sztemp[1], szreg1);
	     strcpy(szreg1, sztemp);
	 }
}

/******************************************************************************/
VOID
Number( ch)

CHAR ch;
{
    register INT iLen, iSize;

    iSize = 9;
    if (szreg1[0] == '-') iSize++;
    if (strchr(szreg1, '.')) iSize++;
    iLen  = strlen(szreg1 );
    if (iLen == iSize) return;
    if (iLen == 1 && szreg1[0] == '0') iLen--;
    szreg1[ iLen ] = ch;
    szreg1[min(iLen + 1, 11)] = 0;
}

/******************************************************************************/
VOID
AppendNumber ( b)

BYTE b;
{
    if (b == '.') {		    /*	if no decimal, add one at end */
        if (!strchr(szreg1, '.'))   
            strcat(szreg1, ".");
    }
    else if ( b == 0xb1 )
	     Negate();
	 else
	     Number(b);
}

/******************************************************************************/
VOID
Equilibrate()
{
    double dblResult;
    double dblX1, dblX2;

    if (chLastKey == '=') return;
    dblResult = (double)atof(szreg1);
    dblX1 = (double)atof(szreg1);
    dblX2 = (double)atof(szreg2);

    switch (PendingOperation) {
        case '+':
	    if (dblX2>(double)0) {	    /* check for overflow */
		if (dblX1>(double)0) {
		    if (dblX1 > (MAXINT - dblX2))
                        fError = TRUE;
		}
	    }
	    else if (dblX2 < (double)0) {
		     if (dblX1 < (double)0) {
			 if ( dblX1 < (MININT - dblX2))
			     fError = TRUE;
		     }
		 }
	    if (!fError)
		dblResult = dblX2 + dblX1;
            break;
        case '-':
	    if (dblX2 < (double)0) {
		if (dblX1 > (double)0) {
		    if (dblX1 > (dblX2 - MININT))
                        fError = TRUE;
		}
	    }
	    else if (dblX2 > (double)0) {
		    if (dblX1 < (double)0) {
			if (dblX1 < (dblX2 - MAXINT))
                            fError = TRUE;
		    }
		 }
            if (!fError) 
		dblResult = dblX2 - dblX1;
            break;
        case '/':
	    if (dblX1 == (double)0.0)
                fError = TRUE;
	    else if (dblX2 > (double)0) {
		     if (dblX1 > (double)0) {
			 if (dblX1 < (dblX2 / MAXINT))
			     fError = TRUE;
		     }
		     else {  /* dblX1 < 0 here */
			if (dblX1 > (dblX2 / MININT))
			     fError = TRUE;
		     }
		 }
		 else {  /* dblX2 < 0 here */
		     if (dblX1 < (double)0) {
			 if (dblX1 > (dblX2 / MAXINT))
			     fError = TRUE;
		     }
		     else { /* dblX1 > 0 here */
			 if (dblX1 < (dblX2 / MININT))
			     fError = TRUE;
		     }
		 }
	    if (!fError)
		dblResult = dblX2 / dblX1;
            break;
        case '*':
	    if (dblX1 == (double)0) return;
	    if (ABS(dblX2) > (double)1) {
		if (ABS(dblX1) > (double)1) {
		    if (ABS(dblX1) > (MAXINT / ABS(dblX2)))
                        fError = TRUE;
                    } 
                }                    
	    if (!fError) dblResult = dblX2 * dblX1;
            break;
        }
    if (!fError) {
	strcpy(szreg1, ftoa((double)dblResult));
        strcpy( szreg2, szreg1 );
        }
    Simplify();
}

/******************************************************************************/
VOID
SquareRoot()
{
    double dblResult;

    dblResult = (double)atof(szreg1);
    if (dblResult < 0.0) {
        fError = TRUE;
        return;
    }
    if ((dblResult == 0.0) || ((chLastKey == 'q') && (dblResult == 1.0)))
        return;
    if ((dblResult < (double) 1.00000002) && (dblResult > (double) 1.0))
	dblResult = (double)1.0;
    else
	dblResult = sqrt(dblResult);
    strcpy( szreg1, ftoa((double)dblResult));
    if (atof( szreg1 ) == 0.0)
        strcpy(szreg1, "0.");
    Simplify();
}

/******************************************************************************/
VOID
BinaryOperator( ch)

CHAR ch;
{
    if (fFirstOperand) {
        fFirstOperand = FALSE;
        strcpy(szreg2, szreg1);
    }
    else {
        Equilibrate();
    }
    PendingOperation = ch;
}

/******************************************************************************/
VOID
Clear()
{
    fReadNumber = FALSE;
    fFirstOperand = TRUE;
    strcpy(szreg1, "0.");
    if (fError || chLastKey == 'c'){
        strcpy(szreg2, "0.");
        PendingOperation = NULL;
    }
    fError = FALSE;
}

/******************************************************************************/
/* trash out trailing zeros, if a '.' is in the number			      */
/* and leading zeros in all cases.					      */
/******************************************************************************/
VOID
Simplify()
{
    register INT iLen, iCount;
    CHAR	 achLocal[20];

    iCount = 0;
    strcpy(achLocal, szreg1);
    if (atof(achLocal) != 0.0) {
	while (achLocal[iCount++] == '0');
	strcpy(szreg1, &achLocal[iCount-1] );
    }
    if (strchr(szreg1, '.')) {
	iLen = strlen(szreg1);
	while (szreg1[--iLen] == '0');
	szreg1[min( iLen + 1, 11)] = 0; /* null terminate */
    }
}


/******************************************************************************/
VOID
DataXPaste()
{
    PSZ 	  psz;
    ULONG	  ulText;
    register CHAR ch;

    if (WinOpenClipbrd( hab))
    {
	ulText = WinQueryClipbrdData( hab, CF_TEXT);
	if (ulText)
	{
	    psz = MAKEP( (SEL)ulText, 0);
	    while (*psz)
            {
		ch = (CHAR) (tolower(*psz));
		if (ch == 'm')
                {
		    psz++;
		    switch (tolower(*psz))
                    {
			case '-':
			    ch = '\271';
			    break;
			case '+':
			    ch = '\272';
			    break;
			case 'r':
			    ch = '\273';
			    break;
			case 'c':
			    ch = '\274';
			    break;
			default:
			    ch = ' ';
			    break;
                    }
                }
		psz++;
		InterpretChar(ch);
                UpdateDisplay();
            }
        }
    }
    WinCloseClipbrd( hab);
    InterpretChar('=');
    UpdateDisplay();
}


/******************************************************************************/
VOID
DataXCopy()
{
    PSZ  pszText;

    if (WinOpenClipbrd( hab))
    {
	WinEmptyClipbrd( hab);
	DosAllocSeg( 20, (SEL FAR *)&sel, SEG_GIVEABLE);
        if (sel == NULL) return;
	pszText = MAKEP(sel, 0);
	FarStrcpy( pszText, (PSZ)szreg1);
	WinSetClipbrdData( hab, (ULONG)sel, CF_TEXT, CFI_SELECTOR);
	WinCloseClipbrd( hab);
    }
}


/******************************************************************************/
VOID
MPlus()
{
    double dblX1, dblX2, dblResult;

    dblX2 = atof(szmem);
    dblX1 = atof(szreg1);

    if (dblX2>(double)0) {	    /* check for overflow */
	if (dblX1>(double)0) {
	    if (dblX1 > (MAXINT - dblX2))
                fError = TRUE;
	}
    }
    else if (dblX2 < (double)0) {
	     if (dblX1 < (double)0) {
		 if ( dblX1 < (MININT - dblX2))
		     fError = TRUE;
	     }
	 }
    if (!fError) {
	dblResult = dblX2 + dblX1;
	strcpy( szmem, ftoa((double)dblResult));
    }
    if (dblResult == (double)0.0)
         fValueInMemory = FALSE; 
    else fValueInMemory = TRUE;
}

/******************************************************************************/
VOID
MClear()
{
     strcpy(szmem, "0.");   
     fValueInMemory = FALSE; 
}


/******************************************************************************/
VOID
MMinus()
{
    double dblX1, dblX2, dblResult;

    dblX2 = atof(szmem);
    dblX1 = atof(szreg1);
    if (dblX2 < (double)0) {
	if (dblX1 > (double)0) {
	    if (dblX1 > (dblX2 - MININT))
                fError = TRUE;
	}
    }
    else if (dblX2 > (double)0) {
	    if (dblX1 < (double)0) {
		if (dblX1 < (dblX2 - MAXINT))
                    fError = TRUE;
	    }
	 }
    if (!fError) {
	dblResult = dblX2 - dblX1;
	strcpy( szmem, ftoa((double)dblResult));
    }
    if (dblResult == (double)0.0)
         fValueInMemory = FALSE; 
    else fValueInMemory = TRUE;
}

/******************************************************************************/
VOID
Evaluate( bCommand)

BYTE bCommand;
{
    switch( bCommand ) {
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '.': case 0xb1:
        case 'n': /* n = 'negate'  from keyboard */
	    if ( fReadNumber )
		AppendNumber( bCommand );
            else {
                      /* if starting a new number */
		if (bCommand != 0xb1)
                    strcpy(szreg1, "0");
		AppendNumber( bCommand );
	    }
	    if (bCommand != 0xb1)
                fReadNumber = TRUE;
            break;
        case '+': case '-': case '/': case '*': case 'p':
	    BinaryOperator(bCommand);
            fReadNumber = FALSE;
            break;
        case '=':
            fReadNumber = FALSE;
            Equilibrate();
            PendingOperation = NULL;
            break;
        case 'q':
            SquareRoot();
            fReadNumber = FALSE;
            break;
        case 0xBB:   /* MR */
            strcpy(szreg1, szmem);
            fReadNumber = FALSE;
            Simplify();
            break;
        case 0xBA: /* M+ */
            MPlus();
            fReadNumber = FALSE;
            Simplify();
            break;
        case 0xB9: /* M- */
            MMinus();
            fReadNumber = FALSE;
            Simplify();
            break;
        case 0xBC:
            MClear(); /* MC */
            break;
        case '%':
            Percent();
            fReadNumber = FALSE;
            break;
        case 'c':
            Clear();
            break;
        }
}

/******************************************************************************/
VOID
Percent()
{
    double dblX1, dblX2, dblResult;

    dblX1 = atof(szreg1) / 100.0;
    dblX2 = atof(szreg2);
    if (ABS(dblX2) > (double)1) {
	if (ABS(dblX1) > (double)1) {
	    if (dblX1 > (MAXINT / dblX2))
                fError = TRUE;
	}
    }
    if (!fError) {
	dblResult = dblX2 * dblX1;
	strcpy( szreg1, ftoa((double)dblResult));
    }
    Simplify();
}

/******************************************************************************/
VOID
InitCalc()
{
    fReadNumber = FALSE;
    fError = FALSE;
    fFirstOperand = TRUE;
    PendingOperation = 0;
    strcpy(szreg1, "0.");
    strcpy(szmem,  "0.");
}
