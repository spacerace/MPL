
/****************************************************************
    STDDEFS.H
****************************************************************/    

#ifndef _STDDEFS_
#define _STDDEFS_	"STDDEFS.H Version 1.03 -- Aug 18 1987" 


#ifndef VOID
#define VOID void
#endif

#ifndef WM_CREATE /*tells us windows.h*/
typedef short 	WORD;
typedef unsigned long  	DWORD;
typedef long	LONG;
typedef long	LWORD;
typedef WORD	HANDLE;
typedef HANDLE	HWND;

typedef int  		BOOL;
typedef unsigned char	BYTE;
#endif

#ifdef MAC
#define WCALLBACK
#define NEARPASCAL
#else
#define WCALLBACK	far pascal
#define NEARPASCAL	near pascal
#endif

typedef char *		MEMADDR;
typedef char *		STRING;
typedef unsigned int 	UWORD;
typedef int		ENUM;
typedef char		CHAR;

#ifdef RV /*10-10-88*/
typedef int		INT;
typedef int		INDEX;
#define INVALID_INDEX  ((INDEX) -1)
#endif

#define LOOP    for(;;)
#define LOCAL   static

#ifdef MAC
#define LOCALF /**/
#else
#define LOCALF static
#endif

#define PUBLIC /**/

#define IMPORT  extern
#define EXPORT /**/
#define EXTERN extern
#define PTR     	*


#define REG register

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
#ifndef NULL
#define NULL 0
#endif

#define BLOCK		/**/

/****************************************************************/

#define CTL(c)		((c)-'@')

#define LOW_BYTE(n)	(n & 0x00FF)
#define HIGH_BYTE(n)	((n & 0xFF00) >> 8)
#define HIGH_BIT	(0x80)


		/*********  TTY DEFS ************************************/
#define NULL_PTR 	((char *)0)
#define NUL 		((BYTE)'\0')
#define ASCII_NULL	((BYTE)'\0')
#define BELL  		((BYTE)  7)
#ifndef BS
#define BS  		((BYTE)  8)
#endif
#define TAB_CH		((BYTE)  9)
#define LF		((BYTE) 10)
#ifndef CR
#define CR		((BYTE) 13)
#endif
#define ESC		((BYTE) 27)
#define SPACE_CH 	((BYTE) ' ')
#define UNDERLINE_CH 	((BYTE) '_')
#define DEL 		((BYTE)'\177')
#define BACKSLASH_CH	((BYTE)'\\')
#define NEWLINE_CH		(13)


		/************ MISC TEXT ORIENTED MACROS **************/

#define is_digit(c)	('0'<=(c) && (c)<='9')
#define is_upper(c)	('A'<=(c) && (c)<='Z')
#define is_lower(c)	('a'<=(c) && (c)<='z')
#define is_alpha(c)	(is_upper(c) || is_lower(c))
#define to_lower(c)	(is_upper(c) ? (c)+SPACE_CH : (c))
#define to_upper(c)	(is_lower(c) ? (c)-SPACE_CH : (c))


		/************ MISC COMMON MACROS **************/
#ifndef MIN
#define MIN(a,b)		( ((a) > (b)) ? (b) : (a))		
#define MAX(a,b)		( ((a) < (b)) ? (b) : (a))		
#endif
#ifndef min
#define min(a,b)		( ((a) > (b)) ? (b) : (a))		
#define max(a,b)		( ((a) < (b)) ? (b) : (a))		
#endif

#ifndef ABS
#define ABS(a)			( (a) > 0 ? (a) : -(a))
#endif


#ifndef MAKELONG
#define MAKELONG(a, b)  ((long)(((unsigned)a) | ((unsigned long)((unsigned)b)) << 16))
#define LoWord(x)		((unsigned short)x)
#define HiWord(x)		((unsigned short)(((unsigned long)x) >> 16))
#define SetLoWord(x,val)	(x = MAKELONG(val,HiWord(x)))
#define SetHiWord(x,val)	(x = MAKELONG(LoWord(x),val))
#endif

#ifndef MAC
#ifndef _MACTYPES_

typedef struct 
{
    int	v,h;
} Point;
	
typedef struct
{
    int	top,left,bottom,right ;
} Rect;
#endif
#endif


#endif /*_STD_DEFS_*/


