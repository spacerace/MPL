          TITLE   Show

; Program SHOW.ASM
; Purpose Text file displayer
; Input   File name from command line or prompt
; Output  Display file to screen

          DOSSEG
          .MODEL  small, pascal

INCL_DOSFILEMGR   EQU 1         ; Enable call groups
INCL_DOSMEMMGR    EQU 1
INCL_KBD          EQU 1
INCL_VIO          EQU 1

          INCLUDE os2.inc
          INCLUDELIB doscalls.lib

          .STACK  800h

          .DATA

; Status line

          PUBLIC  stLine, nLines, curLine
curLine   DW      1             ; Current line number
nLines    DW      ?             ; Lines per screen
stLine    DB      "Line: 12345 "
stFile    DB      "File: 12345678.123  "
          DB      "Quit: Q  Next: ESC  Move:   PGUP PGDN HOME END"

; Variables for screen and cursor handling

          PUBLIC  vMode, Cell, stAtrib, scAtrib
vMode     VIOMODEINFO <>        ; Structures for video and cursor data
lvMode    EQU     $ - vMode     ; Length of structure
vType     DW      0             ; Video type - 0 flag for no change

cMode     VIOCURSORINFO <>
cAtrib    DW      -1            ; Cursor attribute (initized to hidden)
cStatus   DB      0             ; 0 = cursor visible, position unchanged
                                ; 1 = cursor invisible, position unchanged
                                ; 2 = cursor invisible, position changed

stAtrib   DB      030h          ; Status line color default - black on cyan
stBW      EQU     070h          ; B&W default - black on white
Cell      LABEL   WORD          ; Cell (character and attribute)
scChar    DB      " "           ; Initialize to space
scAtrib   DB      017h          ; Screen color default - white on blue
scBW      EQU     007h          ; B&W default - white on black

; Variables for buffer and file handling

          PUBLIC  Buffer, oBuffer, sBuffer, lBuffer
Buffer    LABEL   DWORD
oBuffer   DW      0             ; Position in buffer (offset)
sBuffer   DW      ?             ; Base of buffer (segment)
lBuffer   DW      ?             ; Length of buffer

; File information

lfName    EQU     66
fName     DB      lfName DUP (" ")
fHandle   DW      ?             ; Holds file handle on open
fAction   DW      ?             ; Result of open
fAtrib    EQU     0             ; Normal file
fFlag     EQU     1             ; Open file if exist, fail if not exist
; Read only, deny none, private, error codes, use cache, normal file
fModeRec  RECORD  DA:1=0,WT:1=0,FE:1=0,R1:5=0,INF:1=1,SM:3=2,R2:1=0,AM:3=0
fMode     fModeRec <>
fRead     DW      ?             ; Bytes read from file

; Directory information for file name search

dHandle   DW      -1            ; Directory handle
dResult   FILEFINDBUF <>        ; Structure for results
dlResult  EQU     $ - dResult   ;   length of result
dCount    DW      1             ; Find one file at a time

Prompt    DB      13,10,"Enter filename: "
lPrompt   EQU     $ - Prompt
Prompt2   DB      13,10,"No such file. Try again? "
lPrompt2  EQU     $ - Prompt2
Prompt3   DB      13,10,"File too large: "
lPrompt3  EQU     $ - Prompt3
Prompt4   DB      13,10,"Memory problem.",13,10
lPrompt4  EQU     $ - Prompt4

; Keyboard data

kChar     KBDKEYINFO <>         ; Structures for character and string input
kStr      STRINGINBUF <>
kWait     EQU     0             ; Wait flag

; Call table

kTable    DB      71,72,73,79,80,81,'q','Q'; Key codes
lkTable   EQU     $-kTable
procTable DW      homek                    ; Table of keys and procedures
          DW      upk
          DW      pgupk
          DW      endk
          DW      downk
          DW      pgdnk
          DW      Quit
          DW      Quit
          DW      nonek

          .CODE
          EXTRN   Pager:PROC         ; Routine in other module

start     PROC
          mov     es, ax             ; Load environment segment
          mov     di, bx

; Throw away .EXE name

          sub     ax, ax             ; Find null at end of program name
          repne   scasb
          cmp     BYTE PTR es:[di], 0; If double zero, there's no name
          je      Prompter           ;   so get from prompt

	  cmp	  BYTE PTR es:[di], ' '
          jne     skip1
          inc     di                 ; Skip leading space
skip1:
; Copy command line to file name buffer

          mov     si, di             ; Filename source
          mov     di, OFFSET fName   ; Name buffer destination
          mov     bx, ds             ; Save segment registers
          mov     dx, es
          mov     ds, dx             ; DS = ES
          mov     es, bx             ; ES = DS
	  mov	  cx, lfName	     ; Count = max file name allowed
loop1:	  lodsb 		     ; Copy first character
	  stosb
          cmp     al,' '             ; Terminate on space too
          je      skip2
	  or	  al,al
	  loopnz  loop1 	     ; If not null, copy another
skip2:
          mov     ds, bx             ; Restore DS
	  mov	  BYTE PTR [di-1], 0
          jmp     FindFile

NoFile:   @VioWrtTTy Prompt2, lPrompt2, 0
          @KbdCharIn kChar, kWait, 0
          and     kChar.kbci_chChar, 11011111b ; Convert to uppercase
          cmp     kChar.kbci_chChar, "Y"
          mov     dHandle, -1
          mov     dCount, 1
          je      Prompter           ; If yes, try again
          jmp     Quit               ;   else quit

Prompter: @VioWrtTTy Prompt, lPrompt, 0 ; Else prompt for file name

          mov     kStr.kbsi_cb, lfName

          @KbdStringIn fName, kStr, kWait, 0
          mov     di, kStr.kbsi_cchIn ; Null terminate
          mov     fName[di], 0

; Find first (or only) file in filespec

FindFile: @DosFindFirst fName, dHandle, 0, dResult, dlResult, dCount, 0
          or      ax, ax
          jz      skip3
          jmp     NoFile

; Adjust for current mode and video adapter and hide cursor
skip3:    call    GetVid

FileLoop:
          mov     cStatus, 2         ; Cursor invisible, position unchanged

; Copy file name to file spec

          push    ds                 ; Get file name position in file spec
          @Pushc  <OFFSET fName>
          call    GetNamPos
          mov     si, OFFSET dResult.findbuf_achName ; Load source name
          mov     es, dx             ; Load adjusted destination address
          mov     di, ax             ;   from return value
          sub     cx, cx             ; Load file length
          mov     cl, dResult.findbuf_cchName
          rep     movsb              ; Copy to spec
          mov     BYTE PTR es:[di], 0; Null terminate

; Copy file name to status line

          sub     cx, cx             ; Load file length
          mov     cl, dResult.findbuf_cchName
          mov     bx, 12             ; Calculate blank spaces to fill
          sub     bx, cx
          push    ds                 ; ES=DS
          pop     es
          mov     si, OFFSET dResult.findbuf_achName ; File name as source
          mov     di, OFFSET stFile[6] ; Status line as destination
          rep     movsb
          mov     al, " "            ; Fill rest of name space with blanks
          mov     cx, bx
          rep     stosb

; Open file

          @DosOpen fName, fHandle, fAction, 0, fAtrib, fFlag, [fMode], 0
          or      ax, ax
          jz      skip4
          jmp     NoFile

skip4:    cmp     WORD PTR dResult.findbuf_cbFile[2], 0
          jz      skip6              ; Make sure file is less than a segment
          mov     cStatus, 1         ; Cursor invisible, position unchanged
          @VioWrtTTy Prompt3, lPrompt3, 0
          @VioWrtTTy <stFile + 6>, 12, 0
          cmp     [dCount], 0        ; Get key if there's another file
          je      skip5
          @KbdCharIn kChar, kWait, 0
skip5:    jmp     skip11

; Allocate file buffer

skip6:    mov     ax, WORD PTR dResult.findbuf_cbFile[0] ; Save size
          mov     lBuffer, ax
          mov     oBuffer, 0
          @DosAllocSeg ax, sBuffer, 0
          or      ax, ax
          jz      skip7
          mov     cStatus, 1         ; Cursor invisible, position unchanged
          @VioWrtTTy Prompt4, lPrompt4, 0
          jmp     Quit

; Read the file into the buffer

skip7:    @DosRead [fHandle], [Buffer], [lBuffer], fRead
          or      ax, ax
          jz      skip8
          jmp     NoFile

; Search back for EOF marker and adjust if necessary

skip8:    mov     di, [fRead]        ; Load file length
          dec     di                 ;   and adjust
          mov     es, [sBuffer]      ; Save ES and load buffer segment
          std                        ; Look backward for 255 characters
          mov     cx, 0FFh
          cmp     cx, di
          jb      skip9
          mov     cx, di
skip9:    mov     al, 1Ah            ; Search for EOF marker
          repe    scasb
          cld
          jcxz    skip10             ; If none, we're OK
          inc     di                 ;   else adjust and save file size
          mov     [lBuffer], di

; Show a screen of text and allow commands

skip10:   call    Show


skip11:   @DosClose [fHandle]        ; Close file

          @DosFreeSeg [sBuffer]      ; Free memofy

          @DosFindNext [dHandle], dResult, dlResult, dCount ; Get next file
          
          cmp     [dCount], 0        ; Quit if no next file
          jz      exit
          jmp     FileLoop

exit:     jmp     Quit
start     ENDP
          
Show      PROC

; Display first page

          @Pushc  0                  ; Start at 0
          call    Pager

; Handle keys

nextkey:  @KbdCharIn kChar, kWait, 0 ; Get a key and load to register
          mov     al, kChar.kbci_chChar
          or      al, al             ; Is ascii code null?
          jz      skip1              ; Yes? Load scan
          cmp     al, 0E0h           ; Extended key on extended keyboard?
          jne     skip2              ; No? Got code
skip1:    mov     al, kChar.kbci_chScan
skip2:
          cmp     al, 27             ; Is it ESCAPE?
          je      Exit               ; Yes? Get out for next file

          push    ds                 ; ES = DS
          pop     es
          mov     di, OFFSET kTable  ; Load address and length of key list
          mov     cx, lkTable + 1
          repne   scasb              ; Find position and point to key
          sub     di, (OFFSET kTable) + 1
          shl     di, 1              ; Adjust pointer for word addresses
          call    procTable[di]      ; Call procedure
          jmp     nextkey

exit:     ret
Show      ENDP

homek:    mov     oBuffer, 0         ; HOME - set position to 0
          push    oBuffer
          mov     curLine, 1
          call    Pager
          retn

upk:      @Pushc  -1                 ; UP - scroll back 1 line
          call    Pager
          retn

pgupk:    mov     ax, nLines         ; PGUP - Page back
          neg     ax
          push    ax
          call    Pager
          retn

endk:     mov     ax, lBuffer        ; END - Get last byte of file
          mov     oBuffer, ax        ; Make it the file position
          mov     curLine, -1        ; Set illegal line number as flag
          mov     ax, nLines         ; Page back
          neg     ax
          push    ax
          call    Pager
          retn

downk:    @Pushc  1                  ; DOWN - scroll forward 1 line
          call    Pager
          retn

pgdnk:    push    nLines             ; PGDN - page forward
          call    Pager
          retn

nonek:    retn                       ; Ignore unknown key

GetVid    PROC

          mov     vMode.viomi_cb, lvMode
          @VioGetMode vMode, 0       ; Get video mode

          sub     ax, ax             ; Clear AH
          mov     al, vMode.viomi_fbType ; Put type in register
          mov     vType, ax          ;   and save
          test    al, 1              ; Test for color
          jz      skip1              ; No? Mono
          test    al, 100b           ; Test for color burst on
          jz      skip2              ; Yes? Color
skip1:    mov     stAtrib, stBW      ; Set B&W defaults for status line
          mov     scAtrib, scBW      ;   and screen background

skip2:    @VioGetCurType cMode, 0    ; Get cursor mode
          mov     ax, cMode.vioci_attr ; Save attribute
          xchg    cAtrib, ax
          mov     cMode.vioci_attr, ax ; Set hidden cursor attribute
          mov     ax, vMode.viomi_row; Get number of rows and adjust
          dec     ax
          mov     nLines, ax

          @VioSetCurType cMode, 0    ; Hide cursor

          ret
GetVid    ENDP

GetNamPos PROC    USES di si, argline:FAR PTR BYTE

          les     di, argline        ; Load address of file name
          mov     si, di             ; Save copy

          sub     cx, cx             ; Ignore count
          sub     dx, dx             ; Use DX as found flag
          dec     di                 ; Adjust
          mov     ax, "\"            ; Search for backslash
loop1:    scasb                      ; Get next character
          jz      skip1              ; If backslash, set flag and save
          cmp     BYTE PTR es:[di], 0; If end of name, done
          je      skip2
          loop    loop1              ; If neither, continue
skip1:    mov     si, di             ; Save position
          inc     dx                 ; Set flag to true
          loop    loop1

skip2:    or      dx, dx             ; Found backslash?
          je      skip3              ; If none, search for colon
          mov     ax, si             ;   else return position in DX:AX
          jmp     SHORT exit

skip3:    neg     cx                 ; Adjust count
          mov     di, si             ; Restore start of name
          mov     ax, ":"            ; Search for colon
          repne   scasb
          jnz     skip4              ; If no colon, restore original
          mov     ax, di             ;   else return position in DX:AX
          jmp     SHORT exit

skip4:    mov     ax, si             ; Return original address

exit:     mov     dx, es
          ret
GetNamPos ENDP

Quit      PROC

          mov     scAtrib, 7         ; Restore cell attribute for clear screen

          cmp     cStatus, 1         ; Check cursor status
          jg      skip1              ; 2 - Make cursor visible on last line
          je      skip1              ; 1 - Make cursor visible
          jmp     SHORT skip3        ; 0 - Leave cursor as is

skip1:    @VioSetCurPos [nLines], 0, 0 ; Restore cursor on last line
          @VioScrollDn [nLines], 0, [nLines], 79, 1, Cell, 0


skip2:    mov     ax, cAtrib         ; Restore cursor attribute
          mov     cMode.vioci_attr, ax
          @VioSetCurType cMode, 0

skip3:    @DosExit 1, 0              ; Quit

Quit      ENDP

          END    start
