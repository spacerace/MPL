/*
 * WINDOWS COLOR PALETTE UTILITY - SOURCE
 *
 * LANGUAGE : Microsoft C 5.0
 * TOOLKIT  : Windows 2.03 SDK
 * MODEL    : Small
 * STATUS   : Operational
 *
 * 03/20/88 - Kevin P. Welch - initial creation.
 *
 */
 
#include <windows.h>
#include <math.h>
#include "spectrum.h"
#include "palette.h"

/* spectrum values */
WORD		wModel;
RGB		rgbColor;
CMY		cmyColor;
HSV		hsvColor;

/* default RGB colors */
LONG		rgbRed[32] = {				
					0x00000000,0x00000011,0x00000022,0x00000033,
					0x00000044,0x00000055,0x00000066,0x00000077,
					0x00000088,0x00000099,0x000000aa,0x000000bb,
					0x000000cc,0x000000dd,0x000000ee,0x000000ff,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};

LONG		rgbGreen[32] = {	
					0x00000000,0x00001100,0x00002200,0x00003300,
					0x00004400,0x00005500,0x00006600,0x00007700,
					0x00008800,0x00009900,0x0000aa00,0x0000bb00,
					0x0000cc00,0x0000dd00,0x0000ee00,0x0000ff00,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};
				
LONG		rgbBlue[32] = {
					0x00000000,0x00110000,0x00220000,0x00330000,
					0x00440000,0x00550000,0x00660000,0x00770000,
					0x00880000,0x00990000,0x00aa0000,0x00bb0000,
					0x00cc0000,0x00dd0000,0x00ee0000,0x00ff0000,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};

/* default CMY colors */
LONG		cmyCyan[32] = {
					0x00ffffff,0x00ffffee,0x00ffffdd,0x00ffffcc,
					0x00ffffbb,0x00ffffa9,0x00ffff99,0x00ffff87,
					0x00ffff76,0x00ffff65,0x00ffff54,0x00ffff43,
					0x00ffff32,0x00ffff21,0x00ffff10,0x00ffff00,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};
				
LONG		cmyMagenta[32] = {
					0x00ffffff,0x00ffeeff,0x00ffddff,0x00ffccff,
					0x00ffbbff,0x00ffa9ff,0x00ff99ff,0x00ff87ff,
					0x00ff76ff,0x00ff65ff,0x00ff54ff,0x00ff43ff,
					0x00ff32ff,0x00ff21ff,0x00ff10ff,0x00ff00ff,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};
				
LONG		cmyYellow[32] = {
					0x00ffffff,0x00eeffff,0x00ddffff,0x00ccffff,
					0x00bbffff,0x00a9ffff,0x0099ffff,0x0087ffff,
					0x0076ffff,0x0065ffff,0x0054ffff,0x0043ffff,
					0x0032ffff,0x0021ffff,0x0010ffff,0x0000ffff,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};

/* default HSV colors */
LONG		hsvHue[32] = {
					0x000000ff,0x000055ff,0x0000aaff,0x0000ffff,
					0x0000ffa9,0x0000ff55,0x0000ff00,0x0054ff00,
					0x00aaff00,0x00ffff00,0x00ffaa00,0x00ff5400,
					0x00ff0000,0x00ff0055,0x00ff00ff,0x005500ff,
					0x00000000,0x41a00000,0x42200000,0x42700000,
					0x42a00000,0x42c80000,0x42f00000,0x430c0000,
					0x43200000,0x43340000,0x43480000,0x435c0000,
					0x43700000,0x43820000,0x43960000,0x43aa0000
				};
				
LONG		hsvSaturation[32] = {
					0x00ffffff,0x00ffeeee,0x00ffdddd,0x00ffcccc,
					0x00ffbbbb,0x00ffa9a9,0x00ff9999,0x00ff8787,
					0x00ff7676,0x00ff6565,0x00ff5454,0x00ff4343,
					0x00ff3232,0x00ff2121,0x00ff1010,0x00ff0000,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};
				
LONG		hsvValue[32] = {
					0x00000000,0x00111111,0x00222222,0x00333333,
					0x00444444,0x00555555,0x00666666,0x00777777,
					0x00888888,0x00999999,0x00aaaaaa,0x00bbbbbb,
					0x00cccccc,0x00dddddd,0x00eeeeee,0x00ffffff,
					0x00000000,0x3d888889,0x3e088889,0x3e4ccccd,
					0x3e888889,0x3eaaaaab,0x3ecccccd,0x3eeeeeef,
					0x3f088889,0x3f19999a,0x3f2aaaab,0x3f3bbbbc,
					0x3f4ccccd,0x3f5dddde,0x3f6eeeef,0x3f800000
				};

/**/

/*
 * WinMain( hInst, hPrevInst, lpszCmdLine, wCmdShow ) : int
 *
 *		hInst				handle of current instance
 *		hPrevInst		handle to previous instance (if any)
 *		lpszCmdLine		pointer to command line arguments
 *		wCmdShow			initial ShowWindow command
 *
 *	This function is the system entry point for the application
 * and is responsible for defining the appropriate window 
 * classes and for processing all the messages.  Note how
 * the dialog box manager is responsible for the operation of
 * the palette window.
 *
 */
 
int PASCAL WinMain( hInst, hPrevInst, lpszCmdLine, wCmdShow )
	HANDLE			hInst;
	HANDLE			hPrevInst;
	LPSTR				lpszCmdLine;
	WORD				wCmdShow;
{
	/* local variables */
	FARPROC			lpProc;						/* temporary function */

	/* register window if first instance */
	if ( hPrevInst || RegisterSpectrum(hInst) ) {
			
		/* display palette dialog box */
		lpProc = MakeProcInstance( (FARPROC)PaletteDlgFn, hInst );
 		DialogBox( hInst, "Palette", NULL, lpProc );
 		FreeProcInstance( lpProc );
		
	} 
			
	/* end program */
	return( FALSE );

}

/**/

/*
 * PaletteDlgFn( hWnd, wMsg, wParam, lParam ) : BOOL 
 *
 *		hWnd					handle to palette window
 *		wMsg					message number
 *		wParam				single word parameter
 *		lParam				double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the color palette dialog box.  This mainly
 * involves the definition and retrieval of the various
 * colors selected by the user.
 *
 */

BOOL FAR PASCAL PaletteDlgFn( hDlg, wMsg, wParam, lParam )
	HWND 			hDlg;
	WORD		 	wMsg;
	WORD 			wParam;
	LONG 			lParam;
{
 	/* local variables */
 	BOOL			bResult;							/* result of function */
 
   /* initialization */
   bResult = TRUE;
   
   /* process messages */
   switch( wMsg )
   	{
   case WM_INITDIALOG : /* initialize dialog box */
	
		/* select crt mixing model */
		wModel = ID_RGB;
		SelectMixingModel( hDlg, wModel );
		
		/* define icon for dialog box */
	  	SetClassWord( 
	  		hDlg, 
	  		GCW_HICON, 
	  		LoadIcon( INSTANCE, (LPSTR)"PaletteIcon" )
	  	);
	  				
	  	break;
   case WM_SYSCOMMAND : /* system command */
		
		/* process sub-messages */
		switch( wParam )
			{
		case SC_CLOSE : /* destroy dialog box */
 		  	EndDialog( hDlg, TRUE );
			break;
		default :
			bResult = FALSE;
			break;
		}
		
		break;
	case WM_COMMAND : /* window command */
   	
   	/* process sub-message */
   	switch( wParam )
   		{
   	case ID_RGB : /* RGB mixing model */
   	case ID_CMY : /* CMY mixing model */
   	case ID_HSV : /* HSV mixing model */
			SelectMixingModel( hDlg, wParam );
	  		break;
   	case ID_SPECTRUM1 : /* 1st spectrum */

   		switch( wModel )
   			{
   		case ID_RGB :
   			rgbColor.fRed = *((float*)&lParam);
   			RGBtoCMY( &rgbColor, &cmyColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE1, "%.3f", rgbColor.fRed );
   			break;
   		case ID_CMY :
   			cmyColor.fCyan = *((float*)&lParam);
   			CMYtoRGB( &cmyColor, &rgbColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE1, "%.3f", cmyColor.fCyan );
   			break;
   		case ID_HSV :
   			hsvColor.fHue = *((float*)&lParam);
   			HSVtoRGB( &hsvColor, &rgbColor );
   			RGBtoCMY( &rgbColor, &cmyColor ); 
				DlgPrintf( hDlg, ID_VALUE1, "%.0f", hsvColor.fHue );
   			break;
   		}
   		InvalidateRect( GetDlgItem(hDlg,ID_SAMPLE), NULL, NULL );

   		break;
   	case ID_SPECTRUM2 : /* 2nd spectrum */

   		switch( wModel )
   			{
   		case ID_RGB :
   			rgbColor.fGreen = *((float*)&lParam);
   			RGBtoCMY( &rgbColor, &cmyColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE2, "%.3f", rgbColor.fGreen );
   			break;
   		case ID_CMY :
   			cmyColor.fMagenta = *((float*)&lParam);
   			CMYtoRGB( &cmyColor, &rgbColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE2, "%.3f", cmyColor.fMagenta );
   			break;
   		case ID_HSV :
   			hsvColor.fSaturation = *((float*)&lParam);
   			HSVtoRGB( &hsvColor, &rgbColor );
   			RGBtoCMY( &rgbColor, &cmyColor ); 
				DlgPrintf( hDlg, ID_VALUE2, "%.3f", hsvColor.fSaturation );
   			break;
   		}
   		InvalidateRect( GetDlgItem(hDlg,ID_SAMPLE), NULL, NULL );

   		break;
   	case ID_SPECTRUM3 : /* 3rd spectrum */

   		switch( wModel )
   			{
   		case ID_RGB :
   			rgbColor.fBlue = *((float*)&lParam);
   			RGBtoCMY( &rgbColor, &cmyColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE3, "%.3f", rgbColor.fBlue );
   			break;
   		case ID_CMY :
   			cmyColor.fYellow = *((float*)&lParam);
   			CMYtoRGB( &cmyColor, &rgbColor );
   			RGBtoHSV( &rgbColor, &hsvColor );
				DlgPrintf( hDlg, ID_VALUE3, "%.3f", cmyColor.fYellow );
   			break;
   		case ID_HSV :
   			hsvColor.fValue = *((float*)&lParam);
   			HSVtoRGB( &hsvColor, &rgbColor );
   			RGBtoCMY( &rgbColor, &cmyColor ); 
				DlgPrintf( hDlg, ID_VALUE3, "%.3f", hsvColor.fValue );
   			break;
   		}
   		InvalidateRect( GetDlgItem(hDlg,ID_SAMPLE), NULL, NULL );

   		break;
   	case ID_SAMPLE : /* sample color patch */
		
			/* update sample only if necessary */
  			if ( HIWORD(lParam) == BN_PAINT ) {
										
				/* local variables */
				HDC			hDC;
				CRT			crtValue;
				HANDLE		hOldBrush;
				RECT			rectClient;
															
				/* display crt equivalent numerical values */
				RGBtoCRT( &rgbColor, &crtValue );
				DlgPrintf( hDlg, ID_RED, "RED   %u", crtValue.cRed );
				DlgPrintf( hDlg, ID_GREEN, "GREEN %u", crtValue.cGreen );
				DlgPrintf( hDlg, ID_BLUE, "BLUE  %u", crtValue.cBlue );
					
				/* paint color sample */
   			hDC = GetDC( LOWORD(lParam) );
   			GetClientRect( LOWORD(lParam), &rectClient );

				/* setup display context */
   			hOldBrush = SelectObject(
   					hDC,
   					CreateSolidBrush( *((DWORD*)&crtValue) )
   				);
   				
   			/* draw color sample */
   			Rectangle(
   				hDC,
   				rectClient.left,
   			   rectClient.top,
   				rectClient.right,
   				rectClient.bottom
   			);

				/* cleanup & release display context */
   			DeleteObject( SelectObject(hDC,hOldBrush) );
   			ReleaseDC( LOWORD(lParam), hDC );

   		}

   		break;
   	default : /* something else */
   		break;
   	}
   	
   	break;
	case WM_NCLBUTTONDBLCLK : /* non-client double click */
		EndDialog( hDlg, TRUE );
		break;
   default :
   	bResult = FALSE;
   	break;
   }	
   
   /* return result */
   return( bResult );
 
}

/**/

/*
 * SelectMixingModel( hDlg, wNewModel )
 *
 *		hDlg			handle to palette dialog box
 *		wNewModel	id of new mixing model
 *
 * This function is responsible for changing the displayed 
 * color mixing model to a new one.  In doing so the color
 * spectrum are changed and an attempt is made to find a
 * match for the currently defined color under the new
 * mixing model.
 *
 */
 
void SelectMixingModel( hDlg, wNewModel )
	HANDLE		hDlg;
	WORD			wNewModel;
{
	/* local variables */
	WORD			wRange;
	WORD			wEntry;
		
	/* define new range & check button */
	wRange = 16;
	wModel = wNewModel;
	CheckRadioButton( hDlg, ID_RGB, ID_HSV, wModel );
			
	/* perform initialization depending on model */
	switch( wNewModel )
		{
	case ID_RGB :

		SetDlgItemText( hDlg, ID_TITLE1, "Red.......:" );
		SetDlgItemText( hDlg, ID_TITLE2, "Green.....:" );
		SetDlgItemText( hDlg, ID_TITLE3, "Blue......:" );

		wEntry = MatchColor(rgbRed,rgbColor.fRed);
	  	SetSpectrumColors( SPECTRUM1, &wRange, rgbRed );
		SetSpectrum( SPECTRUM1, &wEntry  );

		wEntry = MatchColor(rgbGreen,rgbColor.fGreen);
	  	SetSpectrumColors( SPECTRUM2, &wRange, rgbGreen );
		SetSpectrum( SPECTRUM2, &wEntry );

		wEntry = MatchColor(rgbBlue,rgbColor.fBlue);
	  	SetSpectrumColors( SPECTRUM3, &wRange, rgbBlue );
		SetSpectrum( SPECTRUM3, &wEntry  );
		
		break;	
	case ID_CMY :

		SetDlgItemText( hDlg, ID_TITLE1, "Cyan......:" );
		SetDlgItemText( hDlg, ID_TITLE2, "Magenta...:" );
		SetDlgItemText( hDlg, ID_TITLE3, "Yellow....:" );

		wEntry = MatchColor(cmyCyan,cmyColor.fCyan);
	  	SetSpectrumColors( SPECTRUM1, &wRange, cmyCyan );
		SetSpectrum( SPECTRUM1, &wEntry );

		wEntry = MatchColor(cmyMagenta,cmyColor.fMagenta);
	  	SetSpectrumColors( SPECTRUM2, &wRange, cmyMagenta );
		SetSpectrum( SPECTRUM2, &wEntry );

		wEntry = MatchColor(cmyYellow,cmyColor.fYellow);
	  	SetSpectrumColors( SPECTRUM3, &wRange, cmyYellow );
		SetSpectrum( SPECTRUM3, &wEntry );
		
		break;	
	case ID_HSV :
	
		SetDlgItemText( hDlg, ID_TITLE1, "Hue.......:" );
		SetDlgItemText( hDlg, ID_TITLE2, "Saturation:" );
		SetDlgItemText( hDlg, ID_TITLE3, "Value.....:" );

		wEntry = MatchColor(hsvHue,hsvColor.fHue);
	  	SetSpectrumColors( SPECTRUM1, &wRange, hsvHue );
		SetSpectrum( SPECTRUM1, &wEntry );

		wEntry = MatchColor(hsvSaturation,hsvColor.fSaturation);
	  	SetSpectrumColors( SPECTRUM2, &wRange, hsvSaturation );
		SetSpectrum( SPECTRUM2, &wEntry );

		wEntry = MatchColor(hsvValue,hsvColor.fValue);
	  	SetSpectrumColors( SPECTRUM3, &wRange, hsvValue );
		SetSpectrum( SPECTRUM3, &wEntry );
		
		break;
	}
	
}

/**/

/*
 * MatchColor( plTable, fValue ) : iEntry
 *
 *		plTable			table of color values
 *		fValue			value to search for
 *
 * This utility function searches a spectrum table for
 * a particular entry using the key value provided.  An
 * index to the closest match is returned for subsequent
 * use when switching color mixing modes.
 *
 * Note that an exact match may not be present (due to the
 * discrete nature of the spectrum table).  As a result 
 * some variations in the resulting color may by visible.
 *
 */
 
static int MatchColor( plTable, fValue )
	LONG *		plTable;
	float			fValue;
{
	int			iEntry;
	int			iMinimum;
	float			fMinimum;
	
	/* intialization */
	iMinimum = 0;
	fMinimum = ABS(*((float*)&plTable[16])-fValue);

	/* search for best fit */
	for ( iEntry=16; iEntry<32; iEntry++ ) {
		if ( ABS(*((float*)&plTable[iEntry])-fValue) < fMinimum ) {
			iMinimum = iEntry - 16;
			fMinimum = ABS(*((float*)&plTable[iEntry])-fValue);
		}
	}
	
	/* return best entry */
	return( iMinimum );
	
}

/**/

/*
 * DlgPrintf( hDlg, wId, szParmList ) : int
 *
 *		hDlg				handle to dialog box
 *		wId				child window id number
 *		szParmList		printf parameter list
 *
 * This function performs a standard printf operation
 * to a text field in a dialog box.  The parameter list
 * is a standard printf one.  The value returned is the
 * number of characters output to the text field.
 *
 */
 
static int DlgPrintf( hDlg, wId, szParmList )
	HANDLE		hDlg;
	WORD			wId;
	PSTR			szParmList;
{
	/* local variables */
	int			iResult;						
	PSTR			pArguments;					
	char			szString[132];				
	
	/* define string from parameter list & display in text field */
	pArguments = (char *)&szParmList + sizeof(szParmList);		
	iResult = vsprintf( szString, szParmList, pArguments );
	SetDlgItemText( hDlg, wId, szString );
	
	/* normal return */
	return( iResult );
}

/**/

/*
 * RGBtoCRT( prgbValue, pcrtValue ) : BOOL
 *
 *		prgbValue		pointer to red-green-blue value
 *		pcrtValue		pointer to CRT red-green-blue color
 * 
 * This function converts the given RGB color value to the
 * corresponding rgb CRT value.  Normal ranges are assumed.
 * A value of TRUE is returned if successful.
 *
 */
 
BOOL RGBtoCRT( prgbValue, pcrtValue )
	RGB *		prgbValue;
	CRT *		pcrtValue;
{
	
	/* convert RGB to CRT */
	pcrtValue->cUnused = 0;
	pcrtValue->cRed = prgbValue->fRed*255.00;
	pcrtValue->cGreen = prgbValue->fGreen*255.00;
	pcrtValue->cBlue = prgbValue->fBlue*255.00;

	/* normal return */
	return( TRUE );
	
}

/**/

/*
 * CRTtoRGB( pcrtValue, prgbValue ) : BOOL
 *
 *		pcrtValue		pointer to CRT red-green-blue color
 *		prgbValue		pointer to red-green-blue value
 * 
 * This function converts the given rgb CRT color value to a
 * corresponding RGB value.  Normal ranges are assumed.  A
 * value of TRUE is returned if successful.
 *
 */
 
BOOL CRTtoRGB( pcrtValue, prgbValue )
	CRT *		pcrtValue;
	RGB *		prgbValue;
{
	
	/* convert CRT to RGB */
	prgbValue->fRed = pcrtValue->cRed / 255.00;
	prgbValue->fGreen = pcrtValue->cGreen / 255.00;
	prgbValue->fBlue = pcrtValue->cBlue / 255.00;

	/* normal return */
	return( TRUE );
	
}

/**/

/*
 * RGBtoCMY( prgbValue, pcmyValue ) : BOOL
 *
 *		prgbValue		pointer to red-green-blue value
 *		pcmyValue		pointer to cyan-magenta-yellow color
 * 
 * This function converts the given RGB color value to the
 * corresponding CMY value.  Normal ranges are assumed.  A
 * value of TRUE is returned if successful.
 *
 */
 
BOOL RGBtoCMY( prgbValue, pcmyValue )
	RGB *		prgbValue;
	CMY *		pcmyValue;
{
	
	/* convert RGB to CMY */
	pcmyValue->fCyan = 1.00 - prgbValue->fRed;
	pcmyValue->fMagenta = 1.00 - prgbValue->fGreen;
	pcmyValue->fYellow = 1.00 - prgbValue->fBlue;

	/* normal return */
	return( TRUE );
	
}

/**/

/*
 * CMYtoRGB( pcmyValue, prgbValue ) : BOOL
 *
 *		pcmyValue		pointer to cyan-magenta-yellow color
 *		prgbValue		pointer to red-green-blue value
 * 
 * This function converts the given CMY color value to a
 * corresponding RGB value.  Normal ranges are assumed.  A
 * value of TRUE is returned if successful.
 *
 */
 
BOOL CMYtoRGB( pcmyValue, prgbValue )
	CMY *		pcmyValue;
	RGB *		prgbValue;
{
	
	/* convert CMY to RGB */
	prgbValue->fRed = 1.00 - pcmyValue->fCyan;
	prgbValue->fGreen = 1.00 - pcmyValue->fMagenta;
	prgbValue->fBlue = 1.00 - pcmyValue->fYellow;

	/* normal return */
	return( TRUE );
	
}

/**/

/*
 * RGBtoHSV( prgbValue, phsvValue ) : BOOL
 *
 *		prgbValue		pointer to red-green-blue value
 *		phsvValue		pointer to hue-saturation-value color
 * 
 * This function converts the given RGB color value to the
 * corresponding HSV value.  Normal ranges are assumed.  A
 * value of TRUE is returned if successful.
 *
 */
 
BOOL RGBtoHSV( prgbValue, phsvValue )
	RGB *			prgbValue;
	HSV *			phsvValue;
{
	/* local variables */
	float			fMin;
	float			fMax;
	float			fRedDist;
	float			fBlueDist;
	float			fGreenDist;
	
	/* calculate minimum & maximum values */
	fMin = MINIMUM( prgbValue->fRed, prgbValue->fGreen, prgbValue->fBlue );
	fMax = MAXIMUM( prgbValue->fRed, prgbValue->fGreen, prgbValue->fBlue );
	
	/* define value & saturation */
	phsvValue->fValue = fMax;
	phsvValue->fSaturation = (fMax) ? (fMax-fMin)/fMax : 0.00;
	
	/* define hue */
	if ( phsvValue->fSaturation > 0.00 ) {
		
		/* calculate related color distances */
		fRedDist = (fMax-prgbValue->fRed)/(fMax-fMin);
		fGreenDist = (fMax-prgbValue->fGreen)/(fMax-fMin);
		fBlueDist = (fMax-prgbValue->fBlue)/(fMax-fMin);
	
		/* compute hue based on distances */
		if ( prgbValue->fRed == fMax )
			phsvValue->fHue = fBlueDist - fGreenDist;
		else
			if ( prgbValue->fGreen == fMax )
				phsvValue->fHue = 2 + fRedDist - fBlueDist;
			else
				phsvValue->fHue = 4 + fGreenDist - fRedDist;
		
		/* convert hue to degrees and make non-negative */
		phsvValue->fHue *= 60.00;
		if ( phsvValue->fHue < 0.00 )
			phsvValue->fHue += 360.00;
	
	} else
		phsvValue->fHue = 0.00;

	/* normal return */
	return( TRUE );

}

/**/

/*
 * HSVtoRGB( phsvValue, prgbValue ) : BOOL
 *
 *		phsvValue		pointer to hue-saturation-value color
 *		prgbValue		pointer to red-green-blue value
 * 
 * This function converts the given HSV color value to the
 * corresponding RGB value.  Normal ranges are assumed.  A
 * value of TRUE is returned if successful.
 *
 */
 
BOOL HSVtoRGB( phsvValue, prgbValue )
	HSV *			phsvValue;
	RGB *			prgbValue;
{
	/* local variables */
	float			fP;
	float			fQ;
	float			fT;
	float			fHue;
	float			fFract;
	
	/* branch if chromatic */
	if ( phsvValue->fSaturation > 0.00 ) {

		/* define adjusted hue */
		fHue = ( phsvValue->fHue >= 360.00 ) ?
			0.00 :
			phsvValue->fHue/60.00;
		
		/* calculate triplet */
		fFract = fHue - floor(fHue);
		
		fP=phsvValue->fValue*(1.0-phsvValue->fSaturation);
		fQ=phsvValue->fValue*(1.0-(phsvValue->fSaturation*fFract));
		fT=phsvValue->fValue*(1.0-(phsvValue->fSaturation*(1.0-fFract)));		
		
		/* triplet assignment */
		switch( (unsigned)floor(fHue) )
			{
		case 0 :
			prgbValue->fRed = phsvValue->fValue;
			prgbValue->fGreen = fT;
			prgbValue->fBlue = fP;
			break;
		case 1 :
			prgbValue->fRed = fQ;
			prgbValue->fGreen = phsvValue->fValue;
			prgbValue->fBlue = fP;
			break;
		case 2 :
			prgbValue->fRed = fP;
			prgbValue->fGreen = phsvValue->fValue;
			prgbValue->fBlue = fT;
			break;
		case 3 :
			prgbValue->fRed = fP;
			prgbValue->fGreen = fQ;
			prgbValue->fBlue = phsvValue->fValue;
			break;
		case 4 :
			prgbValue->fRed = fT;
			prgbValue->fGreen = fP;
			prgbValue->fBlue = phsvValue->fValue;
			break;
		default :
			prgbValue->fRed = phsvValue->fValue;
			prgbValue->fGreen = fP;
			prgbValue->fBlue = fQ;
			break;
		}
	
	} else {	
		prgbValue->fRed = phsvValue->fValue;
		prgbValue->fGreen = phsvValue->fValue;
		prgbValue->fBlue = phsvValue->fValue;
	}

	/* normal return */
	return( TRUE );

}	
