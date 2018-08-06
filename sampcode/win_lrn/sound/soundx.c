/***********************************************************************
 *** Functions demonstrated: (all of them...)
 ***	    CloseSound, OpenSound,				   
 ***	    StartSound, StopSound,		 
 ***	    CountVoiceNotes, SetVoiceNote,
 ***	    SetVoiceSound, SetSoundNoise, 
 ***	    SetVoiceQueueSize, SetVoiceAccent,
 ***	    SetVoiceEnvelope, SetVoiceThreshold,
 ***	    GetThresholdEvent, GetThresholdStatus,
 ***	    SyncAllVoices, WaitSoundState.			
 ***								
 *** Note that the files are not named SOUND.  This is because	
 *** Windows has an internal driver named sound and will not accept 
 *** sound as an application name.				    
 ************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "soundx.h"

long FAR PASCAL SoundWndProc (HWND, unsigned, WORD, LONG);

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
    HANDLE	hInstance, hPrevInstance;
    LPSTR	lpszCmdLine;
    int 	nCmdShow;
    {
    HWND	hWnd;
    MSG 	msg;
    WNDCLASS	wndclass;

    if (!hPrevInstance)
	{
	wndclass.style		= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= SoundWndProc;
	wndclass.cbClsExtra	= 0;
	wndclass.cbWndExtra	= 0;
	wndclass.hInstance	= hInstance;
	wndclass.hIcon		= LoadIcon(NULL, IDI_ASTERISK);
	wndclass.hCursor	= LoadCursor(NULL,  IDC_ARROW);
	wndclass.hbrBackground	= GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName	= "SoundMenu";
	wndclass.lpszClassName	= "Sound";

	if (!RegisterClass(&wndclass) )
	    return FALSE;
	}

    hWnd = CreateWindow("Sound", "Sound Window",
		WS_MINIMIZEBOX | WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU,
		270, 130, 40, 54, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0,0 ))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}

    return msg.wParam;
    }

long FAR PASCAL SoundWndProc (hWnd, iMessage, wParam, lParam)
    HWND	  hWnd;
    unsigned	  iMessage;
    WORD	  wParam;
    LONG	  lParam;
    {
    char	 szJunk[40];	  /* Buffer fo MessageBoxes		*/
    unsigned int nIndex;	  /* Generic index counter		*/
    static int	 nNoise=0,	  /* Noise parameter			*/
		 nCount=0;	  /* Number returned from GetVoiceNotes */
    static BOOL  bThresh=FALSE,   /* SetVoiceThreshold flag		*/
		 bEnv=FALSE;	  /* SetVoiceEnvelope flag		*/

    /* Notes from The Appassionata					*/
    static int	 nNotes[60]={45,42,38,45,38,47,43,38,45,42,38,45,
			     38,45,40,47,45,40,38,40,37,45,40,37,
			     45,42,38,45,42,50,45,42,50,45,42,50,
			     43,50,47,43,50,45,42,50,42,50,44,50,
			     40,49,45,40,49,45,40,49,50,45,40,33};

    switch (iMessage)
	{
	case WM_DESTROY:
	    PostQuitMessage(0);
	    /* fall through */

	/* Release resources if focus is lost or window is iconized	*/
	/* or ended.  Remove these and a constant sound will continue	*/
	/* for a time even if the window is no longer around.  The bad	*/
	/* part is that sound resources are not released and that can	*/
	/* screw up DOS as well as Windows.				*/
	case WM_KILLFOCUS:
	case WM_SETVISIBLE:
	    CloseSound();
	    StopSound();
	    break;

	/* Open sound resources if window is opened or obtains the focus*/
	case WM_SETFOCUS:
	    OpenSound();
	    break;

	case WM_COMMAND:
	    /* Set number of notes in queue.  Windows only has multiple */
	    /* voice capability if the computer has sound hardware.	*/
	    SetVoiceQueueSize(1, 600);

	    switch (wParam)
		{
		case ID_NOTES:
		    /* Set temp, volume, and style of notes.  For the	*/
		    /* most part, the tempo parameter (2nd) is the only */
		    /* effective value on machines without additional	*/
		    /* sound hardware (volume is ineffective).		*/
		    SetVoiceAccent(1, 220, 20, S_NORMAL, 0);

		    /* Set the number of notes in threshold to 30.  This*/
		    /* causes the threshold flag to be set when the	*/
		    /* number of remaining notes falls below 30.	*/
		    if (bThresh)
			SetVoiceThreshold(1, 30);

		    /* Set an envelope					*/
		    SetVoiceEnvelope(1, bEnv, 3);

		    /* Set notes in queue.  Note 0 is a rest, note 1 is */
		    /* two octaves below middle C.  Concert A (440Hz) is*/
		    /* 34.  To prove this, do SetVoiceNote(1,34,1,0)	*/
		    /* and SetVoiceSound(1,440L*65536L, 300).  The tones*/
		    /* are equivalent.					*/
		    for (nIndex=84; nIndex > 0; nIndex -=1 )
			SetVoiceNote(1, nIndex, 250, 0);

		    /* Play the sound					*/
		    StartSound();

		    /* Retain the count of notes			*/
		    nCount=CountVoiceNotes(1);
		    break;

		case ID_SOUND:
		    /* Set sounds in queue.  Second parameter to	*/
		    /* SetVoiceSound is a LONG, not an int as stated in */
		    /* the documentation. Also, the documentation states*/
		    /* that the high-order word of the second parameter */
		    /* is the frequency in Kilohertz.  However, it is	*/
		    /* actually in Hertz.  Otherwise, this routine would*/
		    /* produce very minimal audible sound.		*/

		    for (nIndex=750; nIndex >=1; nIndex -=3)
			SetVoiceSound(1, (unsigned long)(nIndex*65536L), 3);

		    StartSound();
		    nCount=0;

		    /* Wait until all sounds have been played		*/
		    WaitSoundState(S_QUEUEEMPTY);
		    break;

		case ID_NOISE:
		    /* Start noise. Must StartSound after SetSoundNoise.*/
		    /* Full use of this function requires other sound	*/
		    /* drivers not available on standard machines.	*/
		    StopSound();
		    SetSoundNoise(nNoise++, 100);
		    StartSound();
		    nCount=0;
		    for (nIndex=0; nIndex < 65535; nIndex++); /* pause	*/
		    StopSound();
		    nNoise &=7;
		    break;

		case LUDWIG:
		    /* Play beginning of second movement to the Andante */
		    /* in Beethoven's Appassionata.                     */
		    StopSound();
		    SetVoiceAccent(1, 70, 120, S_LEGATO, 0);
		    SetVoiceEnvelope(1, bEnv, 3);
		    if (bThresh)
			SetVoiceThreshold(1, 30);

		    /* Set notes from the data array.			*/
		    for (nIndex=0; nIndex<60; nIndex++)
			SetVoiceNote(1, nNotes[nIndex], 16, 0);

		    SetVoiceNote(1,50,4,0); /* Longer final note.	*/

		    /* Start sound.  Continues playing until it done,	*/
		    /* the window is detroyed, loses the input focus,	*/
		    /* or iconized.  WaitSoundState can be used to wait */
		    /* until the sound is finished.			*/
		    StartSound();
		    nCount=CountVoiceNotes(1);
		    break;

		case ID_COUNT:
		    /* Show the current value of nCount. GetVoiceNotes	*/
		    /* only counts notes, not sounds, envelopes, etc.	*/
		    sprintf(szJunk, "Notes=%d", nCount);
		    MessageBox(hWnd, szJunk, "CountVoiceNotes", MB_OK);
		    break;

		case ID_SYNC:
		    /* SyncAllVoices is really only effective with	*/
		    /* multivoice support. On standard machines this	*/
		    /* function is not successful.			*/
		    if (SyncAllVoices())
			MessageBox(hWnd, "SyncAllVoices successful.",
				   "SyncAllVoices", MB_OK);
		    else
			MessageBox(hWnd, "SyncAllVoices unsuccessful.",
				   "SyncAllVoices", MB_OK);
		    break;

		case ID_ENV:
		    bEnv=!bEnv;
		    if(bEnv)
			MessageBox(hWnd, "Envelope ON",
				   "SetVoiceThreshold", MB_OK);
		    else
			MessageBox(hWnd, "Envelope OFF",
				   "SetVoiceThreshold", MB_OK);

		    break;

		case ID_EVENT:
		    /* Returns a pointer to an int			*/
		    sprintf(szJunk, "returns: %d", *(GetThresholdEvent()));
		    MessageBox(hWnd, szJunk, "GetThresholdEvent", MB_OK);
		    break;

		case ID_STATUS:
		    /* GetThresholdStatus retrieves an int.  Each bit	*/
		    /* has the status for each voice.			*/
		    /* If a bit is set, the the threshold level for	*/
		    /* that voice has been reached.			*/
		    if (GetThresholdStatus())
			MessageBox(hWnd, "Threshold reached.",
				   "GetThresholdStatus", MB_OK);
		    else
			MessageBox(hWnd, "Threshold not reached.",
				   "GetThresholdStatus", MB_OK);

		    break;

		case ID_SETVT:
		    /* Here a flag is toggled for NOTES and LUDWIG.	*/
		    bThresh=!bThresh;
		    if(bThresh)
			MessageBox(hWnd, "Threshold ON",
				   "SetVoiceThreshold", MB_OK);
		    else
			MessageBox(hWnd, "Threshold OFF",
				   "SetVoiceThreshold", MB_OK);
		    break;
		}
	default:
	    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
	}
    return 0L;
    }
