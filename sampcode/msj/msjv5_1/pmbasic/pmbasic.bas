
'+----------------------------------------------------------
-------------
'| Program Name: PMBasic.BAS
'|
'| Description:  This program gives a simple demo of a
Presentation
'|               Manager program written with BASIC Compiler
Version
'|               6.00 and the BASIC OS/2 Presentation
Manager Toolkit
'|               Supplement.  This supplement is available
from
'|               Microsoft Product Support at (206) 454-
2030.
'|
'|               This program draws a figure with
GpiBegin/EndArea and
'|               GpiPolyFillet.  It allows the user to
choose -- with
'|               a menu defined in the resource script file,
'|               PMBasic.RC -- the number of random points
used in the
'|               area drawn.
'+----------------------------------------------------------
-------------
'***** Type definitions
TYPE POINTL
     x AS LONG
     y AS LONG
END TYPE

TYPE QMSG
     hwnd AS LONG
     msg  AS INTEGER
     mp1  AS LONG
     mp2  AS LONG
     time AS LONG
     ptl  AS LONG
END TYPE

'***** CONSTant definitions
CONST FCFTITLEBAR = &H00000001 : CONST FCFSYSMENU       =
&H00000002
CONST FCFMENU     = &H00000004 : CONST FCFSIZEBORDER    =
&H00000008
CONST FCFMINMAX   = &H00000030 : CONST FCFSHELLPOSITION =
&H00000400
CONST FCFTASKLIST = &H00000800 : CONST CSSIZEREDRAW     =
&H00000004
CONST HWNDDESKTOP = &H00000001 : CONST WSVISIBLE =
&H80000000
CONST WMSIZE  = &H0007 : CONST WMPAINT = &H0023 : CONST
WMCOMMAND = &H0020
CONST BANOBOUNDARY = 0 : CONST BAALTERNATE  = 0

'***** FUNCTION declarations
DECLARE FUNCTION WinInitialize&   (BYVAL ushort AS INTEGER)
DECLARE FUNCTION WinCreateMsgQueue&(BYVAL hab AS LONG,_
                                    BYVAL cmsg AS INTEGER)
DECLARE FUNCTION WinRegisterClass%(BYVAL hab AS LONG, BYVAL
pszCN AS LONG,_
              BYVAL pfnWP AS LONG, BYVAL flSty  AS LONG,_
              BYVAL cbWD AS INTEGER)
DECLARE FUNCTION MakeLong&(BYVAL hiword AS INTEGER,_
                           BYVAL loword AS INTEGER)
DECLARE FUNCTION RegBas&
DECLARE FUNCTION WinCreateStdWindow&(BYVAL hwndP AS LONG,_
             BYVAL flS AS LONG, BYVAL pflCF AS LONG, BYVAL
pszC AS LONG,_
             BYVAL pszT AS LONG, BYVAL styC AS LONG,_
             BYVAL hmod AS INTEGER, BYVAL idRes AS INTEGER,_
             BYVAL phwnd AS LONG)
DECLARE FUNCTION WinGetMsg%(BYVAL hab AS LONG, BYVAL pqmsg
AS LONG,_
            BYVAL hwndF AS LONG, BYVAL msgFF AS INTEGER,_
            BYVAL msgFL AS INTEGER)
DECLARE FUNCTION WinDispatchMsg&(BYVAL hab AS LONG, BYVAL
pqmsg AS LONG)
DECLARE FUNCTION WinDestroyWindow%(BYVAL hwnd AS LONG)
DECLARE FUNCTION WinDestroyMsgQueue%(BYVAL hmq AS LONG)
DECLARE FUNCTION WinTerminate%(BYVAL hab AS LONG)
DECLARE SUB BreakLong(BYVAL along AS LONG, hiword AS
INTEGER,_
                      loword AS INTEGER)
DECLARE FUNCTION WinInvalidateRect%(BYVAL hwnd AS LONG,_
                                    BYVAL pwrc AS LONG,_
                                    BYVAL fIC AS INTEGER)
DECLARE FUNCTION WinBeginPaint&(BYVAL hwnd AS LONG, BYVAL
hps AS LONG,_
                                BYVAL prcl AS LONG)
DECLARE FUNCTION GpiErase%(BYVAL HPS AS LONG)
DECLARE FUNCTION GpiBeginArea%(BYVAL HPS AS LONG, BYVAL
ULONG AS LONG)
DECLARE FUNCTION GpiMove%(BYVAL HPS AS LONG, BYVAL PPTL AS
LONG)
DECLARE FUNCTION GpiPolyFillet&(BYVAL HPS AS LONG, BYVAL
ALONG AS LONG,_
                                BYVAL PPTL AS LONG)
DECLARE FUNCTION GpiEndArea&(BYVAL HPS AS LONG)
DECLARE FUNCTION WinEndPaint%(BYVAL hps AS LONG)
DECLARE FUNCTION WinSendMsg&(BYVAL hwnd AS LONG, BYVAL msg
AS INTEGER,_
                             BYVAL mp1 AS LONG, BYVAL mp2 AS
LONG)
DECLARE FUNCTION WinDefWindowProc&(BYVAL hwnd AS LONG,_
                                   BYVAL msg AS INTEGER,_
                                   BYVAL mp1 AS LONG, BYVAL
mp2 AS LONG)

'*********         Initialization section        ***********

DIM aqmsg AS QMSG

flFrameFlags& = FCFTITLEBAR OR FCFSYSMENU OR FCFSIZEBORDER
OR FCFMENU OR_
                FCFMINMAX OR FCFTASKLIST OR FCFSHELLPOSITION

Class$ = "ClassName" + CHR$(0)

hab& = WinInitialize(0)
hmq& = WinCreateMsgQueue(hab&, 0)

bool% = WinRegisterClass(hab&,_
        MakeLong(VARSEG(Class$), SADD(Class$)), RegBas,
CSSIZEREDRAW, 0)

hwndFrame& = WinCreateStdWindow(HWNDDESKTOP, WSVISIBLE,_
             MakeLong(VARSEG(flFrameFlags&),
VARPTR(flFrameFlags&)),_
             MakeLong(VARSEG(Class$), SADD(Class$)), 0, 0,
0, 1,_
             MakeLong(VARSEG(hwndClient&),
VARPTR(hwndClient&)))

'*************         Message loop         ***************

WHILE WinGetMsg(hab&, MakeLong(VARSEG(aqmsg),
VARPTR(aqmsg)), 0, 0, 0)
   bool% = WinDispatchMsg(hab&, MakeLong(VARSEG(aqmsg),
VARPTR(aqmsg)))
WEND

'***********         Finalize section        ***************

bool% = WinDestroyWindow(hwndFrame&)
bool% = WinDestroyMsgQueue(hmq&)
bool% = WinTerminate(hab&)
END

'***********         Window procedure        ***************

FUNCTION ClientWndProc&(hwnd&, msg%, mp1&, mp2&) STATIC
  ClientWndProc& = 0

  SELECT CASE msg%
  CASE WMSIZE           'Store size to make area
proportional to window
    CALL BreakLong(mp2&, cyClient%, cxClient%)

  CASE WMPAINT          'Paint window with PolyFillet with
(pts%) rand pts
    ' Invalidate to paint whole window
    bool% = WinInvalidateRect(hwnd&, 0, 0)
    hps&  = WinBeginPaint(hwnd&, 0, 0)   'Begin painting
    bool% = GpiErase(hps&)               'Erase window

    '*** Set up array of random points. Number of points is
set with menu.
    IF pts% = 0 THEN pts% = 50
    REDIM aptl(pts%) AS POINTL
    FOR I% = 0 to pts%
      aptl(I%).x = cxClient% * RND : aptl(I%).y = cyClient%
* RND
    NEXT I%

    '*** Start at last pt and draw PolyFillet through all
pts
    '    alternating fill
    bool% = GpiMove(hps&, MakeLong(VARSEG(aptl(pts%)),
VARPTR(aptl(pts%))))
    bool% = GpiBeginArea (hps&, BAALTERNATE OR BANOBOUNDARY)
    bool% = GpiPolyFillet(hps&, pts% + 1,_
                          MakeLong(VARSEG(aptl(0)),
VARPTR(aptl(0))))
    bool% = GpiEndArea(hps&)
    bool% = WinEndPaint(hps&)

  CASE WMCOMMAND          'Menu item sets number of pts to
use in drawing.
    CALL BreakLong(mp1&, hiword%, pts%)
    bool% = WinSendMsg(hwnd&, WMPAINT, 0, 0)      'Send
WMPAINT to draw

  CASE ELSE                 'Pass control to system for
other messages
    ClientWndProc& = WinDefWindowProc(hwnd&, msg%, mp1&,
mp2&)
  END SELECT

END FUNCTION

