/*
 * WINDOWS SPECTRUM CONTROL - HEADER FILE
 *
 * LANGUAGE : Microsoft C 5.0
 * TOOLKIT  : Windows 2.03 SDK
 * MODEL    : Small or Medium
 * STATUS   : Operational
 *
 * 03/20/88 - Kevin P. Welch - initial creation.
 *
 */
 
/* spectrum function definitions */
BOOL FAR PASCAL RegisterSpectrum( HANDLE );
BOOL FAR PASCAL SetSpectrum( HWND, WORD* );
BOOL FAR PASCAL GetSpectrum( HWND, WORD* );
BOOL FAR PASCAL SetSpectrumColors( HWND, WORD*, LONG* );
BOOL FAR PASCAL GetSpectrumColors( HWND, WORD*, LONG* );
LONG FAR PASCAL SpectrumWndFn( HWND, WORD, WORD, LONG );



