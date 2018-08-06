;* SHOWP.ASM - Text file displayer for OS/2 (protect mode).

        TITLE   Show
        .MODEL  small, pascal, os_os2
        .DOSSEG
        .286

INCL_NOCOMMON   EQU 1         ; Enable call groups
INCL_DOSFILEMGR EQU 1
INCL_DOSMEMMGR  EQU 1
INCL_KBD        EQU 1
INCL_VIO        EQU 1

        INCLUDE os2.inc
        INCLUDE show.inc
        INCLUDELIB os2.lib

        .STACK

        .DATA

; Status line

stLine  BYTE    "Line: 12345 "
stFile  BYTE    "File: 12345678.123  "
        BYTE    "Quit: Q  Next: ESC  Move:   PGUP PGDN HOME END"

; Variables for screen and cursor handling

yCur    WORD    1               ; Current line number
yMax    WORD    ?               ; Lines per screen
vmiMode VIOMODEINFO < SIZE VIOMODEINFO > ; Structure for video data
                                ; First field initialized to size
vciCsr  VIOCURSORINFO <>        ; Structure for cursor data
atCsr   WORD    -1              ; Cursor attribute (initized to hidden)
bCsrSta BYTE    0               ; 0 = cursor visible, position unchanged
                                ; 1 = cursor invisible, position unchanged
                                ; 2 = cursor invisible, position changed

atSta   BYTE    STAT_CLR        ; Status line color
celScr  LABEL   WORD            ; Cell (character and attribute)
chScr   BYTE    " "             ; Initialize to space
atScr   BYTE    SCRN_CLR        ; Screen color
chInit  BYTE    0               ; Cell to restore when finished
atInit  BYTE    0

; Buffer variables

fpBuf   LABEL   PBYTE
offBuf  WORD    0               ; Position in buffer (offset)
segBuf  SEL     ?               ; Base of buffer (segment selector)
cbBuf   WORD    ?               ; Count in bytes of buffer

; File information

hFileIn HFILE   ?               ; Holds file handle on open
usAct   WORD    ?               ; Result of open
usMode  WORD    OPEN_ACCESS_READONLY OR OPEN_SHARE_DENYNONE
cbRead  WORD    ?               ; Bytes read from file

; Directory information for file name search

stFiles BYTE    NAME_MAX DUP ("w")
hFiles  WORD    HDIR_CREATE     ; Directory handle
fiFiles FILEFINDBUF <>          ; Structure for results
usCount WORD    1               ; Find one file at a time

; Buffer for file name

kkiChar KBDKEYINFO <>           ; Structure for character input
sibStr  STRINGINBUF < NAME_MAX >; Structure for string input

; Messages

stMsg1  BYTE    13, 10, "Enter filename: "
stMsg2  BYTE    13, 10, "File problem. Try again? "
stMsg3  BYTE    13, 10, "File too large: "
stMsg4  BYTE    13, 10, "Memory problem.",13,10

; Call table

achKeys BYTE    71, 72, 73, 79, 80, 81, 'q', 'Q'; Key table
afnKeys WORD    HomeKey                         ; Corresponding procedures
        WORD    UpKey
        WORD    PgUpKey
        WORD    EndKey
        WORD    DownKey
        WORD    PgDnKey
        WORD    Quit
        WORD    Quit
        WORD    UnknownKey

        .CODE
        .STARTUP

; Load environment segment

        mov     es, ax                  ; AX points to environment segment
        mov     di, bx                  ; BX points to command line offset

; Throw away .EXE name

        sub     ax, ax                  ; Find null at end of program name
        repne   scasb
        cmp     BYTE PTR es:[di], 0     ; If double zero, there's no name
        je      Prompter                ;   so get from prompt

        .IF     BYTE PTR es:[di] == ' '
        inc     di                      ; Skip leading space
        .ENDIF

; Copy command line to file name buffer

        mov     si, di                  ; Filename source
        mov     di, OFFSET stFiles      ; Name buffer destination
        mov     bx, ds                  ; Save segment registers
        mov     dx, es
        mov     ds, dx                  ; DS = ES
        mov     es, bx                  ; ES = DS
        mov     cx, NAME_MAX            ; Count = max file name allowed

        .REPEAT
        lodsb                           ; Copy characters
        .BREAK .IF (al == ' ') || (al == 0) ; Stop at space or null
        stosb
        .UNTILCXZ                       ; Until name exceeds max

        mov     ds, bx                  ; Restore DS
        mov     BYTE PTR [di], 0
        jmp     FindFile

; Prompt for file

NoFile:
        INVOKE  VioWrtTTy,              ; Write message
                ADDR stMsg2,
                LENGTHOF stMsg2,
                0

        INVOKE  KbdCharIn,
                ADDR kkiChar,
                IO_WAIT,
                0

        and     kkiChar.chChar_, 11011111y ; Convert to uppercase
        cmp     kkiChar.chChar_, "Y"

        mov     hFiles, -1
        mov     usCount, 1
        .IF     !zero?
        jmp     Quit                    ; Quit if not yes
        .ENDIF
Prompter:
        INVOKE  VioWrtTTy,              ; Else prompt for file name
                ADDR stMsg1,
                LENGTHOF stMsg1,
                0

        INVOKE  KbdStringIn,
                ADDR stFiles,
                ADDR sibStr,
                IO_WAIT,
                0

        mov     di, sibStr.cchIn_       ; Null terminate
        mov     stFiles[di], 0

; Find first (or only) file in filespec

FindFile:
        INVOKE  DosFindFirst,
                ADDR stFiles,
                ADDR hFiles,
                0,
                ADDR fiFiles,
                SIZE fiFiles,
                ADDR usCount,
                0

        or      ax, ax
        jnz     NoFile

        INVOKE  GetVid                  ; Adjust for current mode and
                                        ;  video adapter and hide cursor

; Main program loop to process files

        .REPEAT

; Copy file name to file spec

        mov     bCsrSta, 2              ; Cursor hidden, position unchanged
        INVOKE  GetNamePos,             ; Get file name position in file spec
                ADDR stFiles

        mov     si, OFFSET fiFiles.achName_; Load source name
        mov     di, ax                  ; Load adjusted destination address
                                        ;   from return value
        sub     cx, cx                  ; Load file name length
        mov     cl, fiFiles.cchName_
        rep     movsb                   ; Copy to spec
        mov     BYTE PTR es:[di], 0     ; Null terminate

; Copy file name to status line

        sub     cx, cx                  ; Load file length
        mov     cl, fiFiles.cchName_
        mov     bx, 12                  ; Calculate blank spaces to fill
        sub     bx, cx
        push    ds                      ; ES=DS
        pop     es
        mov     si, OFFSET fiFiles.achName_; File name as source
        mov     di, OFFSET stFile[FILE_POS]; Status line as destination
        rep     movsb
        mov     al, " "                 ; Fill rest of name space with blanks
        mov     cx, bx
        rep     stosb

; Skip any file that is larger than 64K

        .IF     WORD PTR fiFiles.cbFile_[2] != 0

        INVOKE  VioWrtTTy,
                ADDR stMsg3,
                LENGTHOF stMsg3,
                0

        INVOKE  VioWrtTTy,
                ADDR fiFiles.achName_,
                fiFiles.cchName_,
                0

        .IF     usCount <= 0            ; Get key if there's another file
        INVOKE  KbdCharIn,
                ADDR kkiChar,
                IO_WAIT,
                0
        .ENDIF
        .ENDIF

; Allocate file Buffer

        mov     ax, WORD PTR fiFiles.cbFile_[0] ; Save size
        mov     cbBuf, ax
        mov     offBuf, 0
        INVOKE  DosAllocSeg,
                ax,
                ADDR segBuf,
                0

        .IF     ax != 0
        mov     bCsrSta, 1              ; Cursor hidden, position unchanged
        INVOKE  VioWrtTTy,
                ADDR stMsg4,
                LENGTHOF stMsg4,
                0

        jmp     Quit
        .ENDIF

; Open file and read contents into buffer

        INVOKE  DosOpen,
                ADDR stFiles,
                ADDR hFileIn,
                ADDR usAct,
                0,
                FILE_NORMAL,
                FILE_OPEN,
                usMode,
                0

        .IF     ax != 0
        jmp     NoFile
        .ENDIF

        INVOKE  DosRead,
                hFileIn,
                fpBuf,
                cbBuf,
                ADDR cbRead

        .IF     ax != 0
        jmp     NoFile
        .ENDIF

; Search back for EOF marker and adjust if necessary

        mov     di, cbRead              ; Load file length
        dec     di                      ;   and adjust
        mov     es, segBuf              ; Save ES and load buffer segment
        std                             ; Look backward for 255 characters
        mov     cx, 0FFh
        .IF     cx >= di
        mov     cx, di
        .ENDIF

        mov     al, 1Ah                 ; Search for EOF marker
        repne   scasb
        cld
        .IF     cx != 0                 ; If found:
        inc     di                      ; Adjust and save file size
        mov     cbBuf, di
        .ENDIF

; Show a screen of text and allow commands

        INVOKE  Show

        INVOKE  DosClose,               ; Close file
                hFileIn

        INVOKE  DosFreeSeg,             ; Free memofy
                segBuf

        INVOKE  DosFindNext,            ; Get next file
                hFiles,
                ADDR fiFiles,
                SIZE fiFiles,
                ADDR usCount

        .UNTIL  ax != 0                 ; Fall through to Quit if
                                        ;  this is the last file
Quit    PROC

        cmp     bCsrSta, 1              ; Check cursor status
        jg      csrvislast              ; 2 - Make cursor visible on last line
        je      csrvis                  ; 1 - Make cursor visible
        jmp     csrasis                 ; 0 - Leave cursor as is

csrvislast:
        INVOKE  VioSetCurPos,           ; Restore cursor on last line
                yMax,
                0,
                0
        INVOKE  VioScrollDn,
                yMax,
                0,
                yMax,
                79,
                1,
                ADDR chInit,
                0
csrvis:                                 ; Fall through
        mov     ax, atCsr               ; Restore cursor attribute
        mov     vciCsr.attr_, ax
        INVOKE  VioSetCurType,
                ADDR vciCsr,
                0
csrasis:                                ; Fall through
        .EXIT   0

Quit    ENDP


Show    PROC

; Display first page

        mov     yCur, 1
        INVOKE  Pager,                  ; Start at 0
                0

; Handle keys

        .REPEAT
        INVOKE  KbdCharIn,              ; Get a key and load to register
                ADDR kkiChar,
                IO_WAIT,
                0

        mov     al, kkiChar.chChar_

        .BREAK .IF al == 27             ; If ESCAPE get out for next file

        ; If null or E0 (for extended keyboard), it's an extended key
        .IF     (al == 0) || (al == 0E0h)
        mov     al, kkiChar.chScan_      ; Load scan code
        .ENDIF

        push    ds                      ; ES = DS
        pop     es
        mov     di, OFFSET achKeys      ; Load address and length of key list
        mov     cx, LENGTHOF achKeys + 1
        repne   scasb                   ; Find position and point to key
        sub     di, OFFSET achKeys + 1
        shl     di, 1                   ; Adjust pointer for word addresses
        call    afnKeys[di]             ; Call procedure
        .UNTIL  0

        ret
Show    ENDP

HomeKey:
        mov     offBuf, 0               ; HOME - set position to 0
        mov     yCur, 1
        INVOKE  Pager, offBuf
        retn

UpKey:
        INVOKE  Pager, -1               ; UP - scroll back 1 line
        retn

PgUpKey:
        mov     ax, yMax                ; PGUP - Page back
        neg     ax
        INVOKE  Pager, ax
        retn

EndKey:
        mov     ax, cbBuf               ; END - Get last byte of file
        dec     ax                      ; Zero adjust
        mov     offBuf, ax              ; Make it the file position
        mov     yCur, -1                ; Set illegal line number as flag
        mov     ax, yMax                ; Page back
        neg     ax
        INVOKE  Pager, ax
        retn

DownKey:
        INVOKE  Pager, 1                ; DOWN - scroll forward 1 line
        retn

PgDnKey:
        INVOKE  Pager, yMax             ; PGDN - page forward
        retn

UnknownKey:
        retn                            ; Ignore unknown key


;* GetVid - Gets the video mode and sets related global variables.
;*
;* Params: None
;*
;* Return: Number of lines in current mode (25, 43, or 50)

GetVid  PROC

        LOCAL   x:USHORT, y:USHORT, cb:USHORT


        INVOKE  VioGetMode,             ; Get video mode
                ADDR vmiMode,
                0

        sub     ax, ax                  ; Clear AH
        mov     al, vmiMode.fbType_     ; Put type in register

        ; If monochrome or color burst off:
        .IF     (al & VGMT_GRAPHICS) || (al & VGMT_DISABLEBURST)
        mov     atSta, STAT_BW          ; Set B&W defaults for status line
        mov     atScr, SCRN_BW          ;   and screen background
        .ENDIF

        INVOKE  VioGetCurPos,           ; Get cursor position (for cell read)
                ADDR y,                 ; Row
                ADDR x,                 ; Column
                0                       ; Console handle

        mov     cb, 1                   ; One cell
        INVOKE  VioReadCellStr,         ; Read cell to get current attribute
                ADDR chInit,            ; Address to receive cell
                ADDR cb,                ; Address of length
                y,                      ; Row
                x,                      ; Column
                0                       ; Console handle
        mov     chInit, ' '             ; Make sure character is space

        INVOKE  VioGetCurType,          ; Get cursor mode
                ADDR vciCsr,
                0
        mov     ax, vciCsr.attr_        ; Save cursor attribute
        xchg    atCsr, ax
        mov     vciCsr.attr_, ax        ; Set hidden cursor attribute
        mov     ax, vmiMode.row_        ; Get number of rows and adjust
        dec     ax
        mov     yMax, ax

        INVOKE  VioSetCurType,          ; Hide cursor
                ADDR vciCsr,
                0

        ret

GetVid  ENDP


        END
