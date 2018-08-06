
/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  TCXLPRN.H - TCXL (c) 1987, 1988 by Mike Smedley.                        ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  printer functions.                                                      ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


#if __STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif


/*---------------------------[ Function Prototypes ]-------------------------*/

void     _Cdecl lprintc(int ch);
int      _Cdecl lprintf(const char *format,...);
void     _Cdecl lprintns(char *str,int count);
void     _Cdecl lprints(char *str);
void     _Cdecl lprintsb(char *str,int reps);
void     _Cdecl lprintsu(char *str);
void     _Cdecl scrndump(void);


/*-----------[ printer escape codes for Epson compatible printers ]----------*/

    /*  Example:  lprints(L_BFON"This is bold printing"L_BFOFF);    */

#define L_BFOFF     "F"                /*  turns bold faced printing off   */
#define L_BFON      "E"                /*  turns bold faced printing on    */
#define L_DWOFF     "W0"               /*  turns double wide printing off  */
#define L_DWON      "W1"               /*  turns double wide printing on   */
#define L_ELITE     "M"                /*  sets printer in 12 CPI mode     */
#define L_INIT      "@"                /*  initializes printer             */
#define L_ITALOFF   "5"                /*  turns italicized printing off   */
#define L_ITALON    "4"                /*  turns italicized printing on    */
#define L_PICA      "P"                /*  sets printer in 10 CPI mode     */
#define L_ULOFF     "-0"               /*  turns underlined printing off   */
#define L_ULON      "-1"               /*  turns underlined printing on    */


/*-----------------------[ Macro-Function Definitions ]----------------------*/

#define lcrlf()             lprintc(LF)

