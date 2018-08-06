;* SHOW.INC - Include file used by DOS and OS/2 versions of SHOW.
;* Contains constants, prototypes and global variables. Items used
;* by ignoreed by OS/2 version)

; Constants for real and protect
LINE_POS        EQU     6       ; Position of line number string
FILE_POS        EQU     6       ; Position of file name string
X_MAX           EQU     80      ; Number of columns
STAT_CLR        EQU     030h    ; Status line color - black on cyan
STAT_BW         EQU     070h    ; Status line B&W - black on white
SCRN_CLR        EQU     017h    ; Screen color - white on blue
SCRN_BW         EQU     007h    ; Screen B&W - white on black

IFDEF OS2_INCLUDED
NAME_MAX        EQU     255     ; OS/2 maximum file specification
ELSE
NAME_MAX        EQU     66      ; DOS maximum file specification
ENDIF

; DOS only
IFNDEF OS2_INCLUDED
SEG_CLR         EQU     0B800h  ; Color address
SEG_MONO        EQU     0B000h  ; Monochrome address
ENDIF

; Prototypes
Show            PROTO
GetNamePos      PROTO pchSpec:PTR BYTE
GetVid          PROTO
Pager           PROTO cLines:SWORD
GoBack          PROTO
GoForeward      PROTO
EndCount        PROTO
BinToStr        PROTO i:WORD, pch:PTR BYTE
Quit            PROTO

;OS/2 Only
IFDEF OS2_INCLUDED
ShowLine        PROTO pchIn:PBYTE, y:WORD, cbMax:WORD, pcelAtrib:PBYTE
ELSE
; DOS only
ShowLine        PROTO fpBuffer:FAR PTR BYTE, y:WORD, attr:BYTE
IsEGA           PROTO
CellFill        PROTO yStart:WORD, cbCell:WORD, celFill:WORD
FillTab         PROTO
ENDIF


; Global variables

        .DATA
        EXTERNDEF       atSta:BYTE, atScr:BYTE, segBuf:WORD, offBuf:WORD
        EXTERNDEF       cbBuf:WORD, stLine:BYTE, yMax:WORD, yCur:WORD

        ; DOS Only
        EXTERNDEF       segVid:WORD, fCGA:BYTE, celCur:WORD, atCur:BYTE

        ; OS/2 Only
        EXTERNDEF       celScr:WORD
@CurSeg ENDS            ; End current
