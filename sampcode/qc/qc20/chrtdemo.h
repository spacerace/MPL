/* File: CHRTDEMO.H
 *
 * Common definitions for major modules of CHRTDEMO.
 */


/* --- For graphics adaptors that are color capable: --- */
#define C_INPUTCOLOR  11      /* Color for data input                 */
#define C_HILITECOLOR 10      /* Color for first-letter highlights    */
#define C_FORMCOLOR   15      /* Color for screen form lines and help */
#define C_TITLECOLOR  15      /* Color for QuickCHART title           */
#define C_ERRORCOLOR  14      /* Color for error lines                */
#define C_INFOCOLOR    7      /* Color non-input data on screen       */

/* --- For graphics adaptors that are not color capable: --- */
#define M_INPUTCOLOR   7      /* Color for data input                 */
#define M_HILITECOLOR 15      /* Color for first-letter highlights    */
#define M_FORMCOLOR    7      /* Color for screen form lines and help */
#define M_TITLECOLOR  15      /* Color for QuickCHART title           */
#define M_ERRORCOLOR  15      /* Color for error lines                */
#define M_INFOCOLOR    7      /* Color non-input data on screen       */

/* Define macros to determine whether the graphics adaptor is color-capable. */
#define ismono(m) ( ((m) == _MRESNOCOLOR) || ((m) == _HRESBW)      || \
                    ((m) == _HERCMONO)    || ((m) == _ERESNOCOLOR) || \
                    ((m) == _VRES2COLOR) )
#define iscolor(m) (!ismono(m))

/* ASCII codes for commonly used control functions. */
#define BEEP        7
#define ESCAPE     27

typedef enum tagBool { FALSE, TRUE } BOOL;

/* Declarations of functions.  */
void Axes( void );
void Axis( axistype *pat );
void AxisRange( axistype *pat );
void AxisScale( axistype *pat );
void AxisTics( axistype *pat );
void Border( windowtype *pwt );
int  BlankMenu( char *pchTitle, char *pchChoice1, char *pchChoice2 );
void ChangeTypeface( void );
void ChartOptions( void );
void ChartType( void );
void ChartWindow( void );
void ChooseFont( int WhichFont, int Height );
void ClearData( BOOL fConfirm );
void ClrForm( void );
void ClrHelp( void );
void DataWindow( void );
void DefaultData( short iType, short iStyle, BOOL fClear );
void ErrorMsg( char *pchMsg );
void FontOptions( void );
void Help( char *pchMsg, short sColor );
void Initialize( void );
int  InputInt( char *pchPrompt, int iOld, int iMin, int iMax );
float InputFloat( char *pchPrompt, float fOld );
char *InputStr( char *pchPrompt, char *pchOld );
char InputCh( char *pchPrompt, char *pchAccept );
BOOL InRange( int Value, int iMin, int iMax );
void Justify( titletype *ptt );
void Legend( void );
void LegendPlace( void );
int  main( void );
void MainMenu( void );
int  Menu( char *pszMenuList[] );
void PopTitle( void );
void PrintAt(int row, int column, char far * lpszString, short sColor);
void PrintChar(int row, int column, char cChar, short sColor);
void PushTitle( char *pchOldTitle );
void PushTitle( char *pchOldTitle );
void ResetOptions( void );
void Demo( void );
void ScreenMode( void );
void SetDisplayColors( void );
BOOL SetGraphMode( int mode );
void ShowError( int iErr );
void ShowChartData( void );
int  ShowAxisType( int iRow, int iCol, axistype theAxis );
int  ShowLegendType( int iRow, int iCol, legendtype theLegend );
void ShowSampleData( void );
int  ShowTitleType( int iRow, int iCol, titletype theTitle );
int  ShowWindowType( int iRow, int iCol, windowtype theWindow );
void SprintAt( int iRow, int iCol, char * szFmt, ... );
void TitleOpt( titletype *ptt );
void Titles( void );
int  ViewChart( void );
void WrtForm( int yBot );
void Windows( void );
void WindowSize( windowtype *pwt );

/* Constant limits. */
#define MAXVALUES  12
#define MAXSERIES  4
