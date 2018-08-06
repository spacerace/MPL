                page    60,128
                title   m_ext.asm -- Microsoft Editor Extension

; (c) Copyright Ken Waldron, 1989.
;               P.O. Box 69807 Stn K, Vancouver, B.C. V5K 4Y7
;
;   Permission is granted to use this code and assembled versions of it
;   for non-commercial purposes, and to distribute these on electronic
;   bulletin boards provided no special fee is charged for downloading
;   them. For other uses, please obtain written permission. Microsoft
;   and Wordstar are registered trademarks of Microsoft Corporation and
;   Micropro International, respectively.

; Purpose:
;
;   This file defines an extension for the Microsoft Editor. The
;   editor, M.EXE, is shipped with MASM 5.x and MSC 5.x. Documentation
;   supplied by Microsoft describes how to write an editor extension in
;   MSC. This extension is written using MASM because I don't own MSC.
;   I wrote it initially to install a word delete function (see
;   Pdelword). I tried using an editor macro (pword sdelete), but its
;   appetite for punctuation, brackets, etc. made it unsuitable for
;   editing source code. More functions have since been added. All
;   low-level editor functions have been declared external, and there
;   are text macros to simplify access to editor ARG structures, so it
;   should be fairly straightforward to add new functions, however you
;   will need the editor manual and the skel.* and ext.* files supplied
;   with the editor to figure it all out.

; New editor functions defined:
;
;   Changecase
;   Pdelword
;   Quik
;   Transpose
;   Fill
;   Number
;   Enqueue
;   Dequeue
;
; These are documented where defined below.

; To use
;
;   NOTE:  If you've used earlier versions, please note that
;   =====  the extension name has been changed to m_ext.ZXT.
;
;   Place following command in tools.ini file (just after [M] label):
;      load:m_ext.zxt
;   Assign the new commands to desired key-chords.  For example:
;      Changecase:alt+C
;      Pdelword:alt+T
;      Quik:ctrl+Q

; To compile:
;
;   masm [/Dnomsg] /Mx m_ext;
;   link /NOI exthdr.obj m_ext.obj, m_ext;
;   ren m_ext.exe m_ext.ZXT
;
;   MASM version 5.1 is required.
;
;   exthdr.obj is supplied with the Microsoft editor (if you're using
;   the protected mode version of the editor, link with exthdrp.obj,
;   however I don't know if the extension will work in protected mode
;   as I haven't tried it).
;
;   Ignore "warning L4021: no stack segment" from the linker.
;
;   When the extension is loaded it announces itself by displaying a
;   message. If "nomsg" is defined, then the message is suppressed.
;
;   An option has been added to control which functions are assembled.
;   To exclude a function, define a symbol of the form "nofunction",
;   where "function" is the name of the function to be excluded (to
;   exclude the queue functions, define "noqueue"). These definitions
;   may appear on the MASM command line (e.g. /Dnoquik) or may be
;   inserted in this file just before the data segment.

; Release history:
;
; 30-Mar-1989  Release 1.0
;
; 02-Apr-1989  Release 1.1
;              Corrected command type of Pdelword.
;              Added Quik function.
;              Added assembler switch to control "loaded" message.
;
; 21-Apr-1989  Beta 2.0
;              Tightened and cleaned up code considerably.
;              Modified Changecase. It now accepts NOARG or BOXARG, but
;              no longer accepts STREAMARG (the old code may be found
;              at the end of this file).
;              Added Transpose and Fill functions.
;
; 01-May-1989  Beta 2.01
;              Speeded up Fill.
;              Added Number function.
;
; 04-May-1989  Release 2.1
;              Tidied up documentation.
;
; 07-May-1989  Beta 2.11
;              Corrected misspelled "nomsg" in WhenLoaded.
;
; 09-May-1989  Beta 2.2
;              Tightened code (dispensed with inefficient "push"
;              macros) and improved documentation.
;              Added Enqueue and Dequeue functions.
;
; 14-May-1989  Release 2.3
;              Added assembly-time option to control which functions
;              are included in the extension.
;              Created a separate documentation file.
;
; 16-May-1989  Beta 2.31
;              Corrected misspelled "noquik" on line 286.
;              Corrected numbase range check in Number.
;              A function that accepts a markarg does not need to test
;              that the markarg is of a type it can handle; args that
;              resolve to types not specified in the command type of
;              the function are rejected by the editor, so I've removed
;              some redundant tests and corrected the docs.
;              Renamed extension to m_ext.ZXT to preclude accidental
;              loading by DOS (it hangs the computer).
;
; 18-Aug-1989  Release 2.32
;              Increased size of linebuf to make room for NUL in lines
;              of maximum length (fixes fatal bug in Pdelword and Fill).

; Considerations for interfacing with MSC:
;
;   Chars are passed as ints, with char itself in low byte.
;
;   Long ints (or pointers) are stored with low-order word (or offset)
;   first, but passed by pushing high-order word (or segment) first. 1
;   byte values are returned in AL, 2 byte values in AX, and 4 byte
;   values AX (low-order) and DX (high-order).
;
;   Only si, di, ds, bp are guarranteed to be preserved across calls.
;
;   NULL == 0 (or 0L in compact, large and huge models).
;
;   For functions using C calling convention, push parameters right-to-
;   left.  For functions using pascal convention, push left-to-right.

; Considerations for interfacing with Microsoft editor:
;
;   Buffer filled by GETLINE is NUL-terminated. PUTLINE expects same.
;
;   Don't store strings to be passed to the editor in segment .CONST.
;
;   The fMeta parameter passed to extension functions is equal to 1 if
;   the Meta key was pressed and 0 otherwise (the editor docs imply
;   that fMeta will be TRUE (-1) if the Meta key was pressed).

; Considerations for modifying the following code:
;
;   When an extension function is called, the editor passes it a far
;   pointer to an ARG structure which defines the selected text. The
;   functions below use a convention of loading the pointer into es:di
;   shortly after they are called, and accessing members of the ARG
;   structure with macros that refer to es:di. Nasty bugs can result if
;   you fail to preserve es across calls to built-in editor functions,
;   since most of them trash it.

                .model  compact, c

                public  WhenLoaded, swiTable, cmdTable

                extrn   pascal REPLACE: near
                extrn   pascal MOVECUR: near
                extrn   pascal DELLINE: near
                extrn   pascal DELBOX: near
                extrn   pascal DELSTREAM: near
                extrn   pascal GETLINE: near
                extrn   pascal ADDFILE: near
                extrn   pascal DELFILE: near
                extrn   pascal FILENAMETOHANDLE: near
                extrn   pascal REMOVEFILE: near
                extrn   pascal COPYLINE: near
                extrn   pascal COPYBOX: near
                extrn   pascal COPYSTREAM: near
                extrn   pascal PFILETOTOP: near
                extrn   pascal DISPLAY: near
                extrn   pascal FILEREAD: near
                extrn   pascal FILEWRITE: near
                extrn   pascal SETKEY: near
                extrn   pascal DOMESSAGE: near
                extrn   pascal PUTLINE: near
                extrn   pascal BADARG: near
                extrn   pascal FILELENGTH: near
                extrn   pascal GETCURSOR: near
                extrn   pascal FEXECUTE: near
                extrn   pascal READCMD: near
                extrn   pascal READCHAR: near
                extrn   pascal KBUNHOOK: near
                extrn   pascal KBHOOK: near

; Some macros to clarify idiomatic instruction usages.

zero            macro   reg
                xor     reg,reg                 ; set register to 0
                endm

setzf           macro
                cmp     ax,ax                   ; set zero flag
                endm

cmpz            macro   reg
                or      reg,reg                 ; compare register to 0
                endm

; The following equates define the bits of a word which specifies how
; command arguments are to be processed.

NOARG           equ     0001h
TEXTARG         equ     0002h
NULLARG         equ     0004h
NULLEOL         equ     0008h
NULLEOW         equ     0010h
LINEARG         equ     0020h
STREAMARG       equ     0040h
BOXARG          equ     0080h
NUMARG          equ     0100h
MARKARG         equ     0200h
BOXSTR          equ     0400h
KEEPMETA        equ     2000h
WINDOWFUNC      equ     4000h
CURSORFUNC      equ     8000h

; Switch types.

SWI_BOOLEAN     equ     0
SWI_NUMERIC     equ     1
SWI_SCREEN      equ     4
SWI_SPECIAL     equ     5
RADIX10         equ     0Ah SHL 8
RADIX16         equ     10h SHL 8

; Maximum line length.

BUFLEN          equ     250

; Return values.

TRUE            equ     -1
FALSE           equ     0

; Text macros to access ARG structure members, assuming that es:di
; points to the ARG structure.

argType$        equ     <word ptr es:[di+ 0]>

noarg$y         equ     <word ptr es:[di+ 2]>
noarg$x         equ     <word ptr es:[di+ 6]>

textarg$cArg    equ     <word ptr es:[di+ 2]>
textarg$y       equ     <word ptr es:[di+ 4]>
textarg$x       equ     <word ptr es:[di+ 8]>
textarg$pText   equ     <word ptr es:[di+10]>

nullarg$cArg    equ     <word ptr es:[di+ 2]>
nullarg$y       equ     <word ptr es:[di+ 4]>
nullarg$x       equ     <word ptr es:[di+ 8]>

linearg$cArg    equ     <word ptr es:[di+ 2]>
linearg$yStart  equ     <word ptr es:[di+ 4]>
linearg$yEnd    equ     <word ptr es:[di+ 8]>

streamarg$cArg   equ    <word ptr es:[di+ 2]>
streamarg$yStart equ    <word ptr es:[di+ 4]>
streamarg$xStart equ    <word ptr es:[di+ 8]>
streamarg$yEnd   equ    <word ptr es:[di+10]>
streamarg$xEnd   equ    <word ptr es:[di+14]>

boxarg$cArg     equ     <word ptr es:[di+ 2]>
boxarg$yTop     equ     <word ptr es:[di+ 4]>
boxarg$yBottom  equ     <word ptr es:[di+ 8]>
boxarg$xLeft    equ     <word ptr es:[di+12]>
boxarg$xRight   equ     <word ptr es:[di+14]>

                .data

; Variables.

linebuf         db      "Editor extension loaded.  (c) 1989 Ken Waldron.",0
                db      (BUFLEN + 1 - ($ - linebuf)) dup (0)
cfile           dw      0                       ; current file handle
col             dw      0                       ; current column
row             dw      0,0                     ; current row

              ifndef nofill
fillmargin      dw      72                      ; right margin for Fill
              endif

              ifndef nonumber
numcount        dw      0                       ; counter for Number
numstep         dw      1                       ; step value for Number
numwidth        dw      6                       ; field width for Number
numbase         dw      10                      ; print radix for Number
numpad          dw      32                      ; pad char for Number
              endif

              ifndef noqueue
qfile           dw      0                       ; queue file handle
              endif

; Constants.

nulstr          db      0

              ifndef nochangecase
Changecase_name db      "Changecase",0
              endif

              ifndef nopdelword
Pdelword_name   db      "Pdelword",0
              endif

              ifndef noquik
Quik_name       db      "Quik",0
Quik_E_cmd      db      "Meta Up",0
Quik_R_cmd      db      "Arg Mpage",0
Quik_S_cmd      db      "Meta Begline",0
Quik_D_cmd      db      "Endline",0
Quik_X_cmd      db      "Meta Down",0
Quik_C_cmd      db      "Arg Ppage",0
              endif

              ifndef notranspose
Transpose_name  db      "Transpose",0
              endif

              ifndef nofill
Fill_name       db      "Fill",0
fillmargin_name db      "fillmargin",0
fillfile_name   db      "<fill>",0
              endif

              ifndef nonumber
Number_name     db      "Number",0
numcount_name   db      "numcount",0
numstep_name    db      "numstep",0
numwidth_name   db      "numwidth",0
numbase_name    db      "numbase",0
numpad_name     db      "numpad",0
              endif

              ifndef noqueue
Enqueue_name    db      "Enqueue",0
Dequeue_name    db      "Dequeue",0
queuefile_name  db      "<queue>",0
              endif

; Switch table.

swiTable        label   byte

              ifndef nofill
                dd      fillmargin_name         ; far pointer to switch name
                dd      fillmargin              ; far pointer to switch
                dw      SWI_NUMERIC + RADIX10   ; int switch type
              endif

              ifndef nonumber
                dd      numcount_name
                dd      numcount
                dw      SWI_NUMERIC + RADIX10

                dd      numstep_name
                dd      numstep
                dw      SWI_NUMERIC + RADIX10

                dd      numwidth_name
                dd      numwidth
                dw      SWI_NUMERIC + RADIX10

                dd      numbase_name
                dd      numbase
                dw      SWI_NUMERIC + RADIX10

                dd      numpad_name
                dd      numpad
                dw      SWI_NUMERIC + RADIX10
              endif

                dd      0                       ; NULL far pointer
                dd      0                       ; NULL far pointer
                dw      0                       ; int 0

; Command table.

cmdTable        label   byte

              ifndef nochangecase
                dd      Changecase_name         ; far pointer to function name
                dd      Changecase              ; far pointer to function
                dw      0                       ; int editor scratch area
                dw      NOARG or BOXARG or MARKARG or KEEPMETA
                                                ; int command type
              endif

              ifndef nopdelword
                dd      Pdelword_name
                dd      Pdelword
                dw      0
                dw      NOARG or KEEPMETA
              endif

              ifndef noquik
                dd      Quik_name
                dd      Quik
                dw      0
                dw      CURSORFUNC or KEEPMETA
              endif

              ifndef notranspose
                dd      Transpose_name
                dd      Transpose
                dw      0
                dw      NOARG or KEEPMETA
              endif

              ifndef nofill
                dd      Fill_name
                dd      Fill
                dw      0
                dw      LINEARG or MARKARG or NUMARG or KEEPMETA
              endif

              ifndef nonumber
                dd      Number_name
                dd      Number
                dw      0
                dw      NOARG or NULLARG
              endif

              ifndef noqueue
                dd      Enqueue_name
                dd      Enqueue
                dw      0
                dw      NOARG or LINEARG or MARKARG or NUMARG

                dd      Dequeue_name
                dd      Dequeue
                dw      0
                dw      NOARG
              endif

                dd      0                       ; NULL far pointer
                dd      0                       ; NULL far pointer
                dw      0                       ; int 0
                dw      0                       ; int 0

                .code

                assume  ss:nothing

; Utility procedures.

setup           proc

                mov     ax,@data
                mov     ds,ax

                ; Get current file handle into cfile.
                mov     ax,offset nulstr
                push    ds
                push    ax
                zero    ax
                push    ax
                push    ax
                call    FILENAMETOHANDLE
                mov     cfile,ax

                ret

setup           endp


isdigit         proc

                ; Set zero flag if al '0'..'9', otherwise clear.

                cmp     al,'0'
                jb      no
                cmp     al,'9'
                jbe     yes

  no:           ret

  yes:          setzf
                ret

isdigit         endp


isalpha         proc

                ; Set zero flag if al is alpha, otherwise clear.

                cmp     al,'A'
                jb      no
                cmp     al,'Z'
                jbe     yes
                cmp     al,'a'
                jb      no
                cmp     al,'z'
                jbe     yes

  no:           ret

  yes:          setzf
                ret

isalpha         endp


xgetline        proc    uses es

                ; Get current row of current file into linebuf.

                push    row[2]                  ; GETLINE( line,
                push    row[0]                  ;
                mov     ax,offset linebuf       ;          buf,
                push    ds                      ;
                push    ax                      ;
                push    cfile                   ;          pfile
                call    GETLINE                 ;                )
                ret

xgetline        endp


xputline        proc    uses es

                ; Put linebuf to current file at current row.

                push    row[2]                  ; PUTLINE( line,
                push    row[0]                  ;
                mov     ax,offset linebuf       ;          buf,
                push    ds                      ;
                push    ax                      ;
                push    cfile                   ;          pFile
                call    PUTLINE                 ;                )
                ret

xputline        endp


xgetcursor      proc    uses es

                ; Get current cursor position into row, col.

                push    ds
                mov     ax,offset col
                push    ax
                push    ds
                mov     ax,offset row
                push    ax
                call    GETCURSOR

                ret

xgetcursor      endp


; Changecase
;      Toggles the case of an alphabetic character under the cursor.
;
; Arg boxarg Changecase
; Arg markarg Changecase       [the area referenced must be a box]
;      Changes upper-case characters in specified box to lower-case.
;
; Arg Arg boxarg Changecase
; Arg Arg markarg Changecase   [the area referenced must be a box]
;      Changes lower-case characters in specified box to upper-case.
;
; Returns FALSE if the argument is invalid, otherwise TRUE.

              ifndef nochangecase
Changecase      proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Determine argument type and act accordingly.
                test    argType$,BOXARG
                jnz     doboxarg

                ; NOARG, so toggle case of char under cursor.
  donoarg:      mov     ax,noarg$y[0]           ; init current row
                mov     row[0],ax
                mov     ax,noarg$y[2]
                mov     row[2],ax
                call    xgetline                ; get current line
                mov     bx,noarg$x
                cmp     ax,bx                   ; cursor beyond eol?
                jbe     nochange
                mov     al,byte ptr linebuf[bx]
                call    isalpha                 ; isalpha?
                jnz     nochange
                xor     al,020h                 ; toggle case
                push    ax                      ; REPLACE( c,
                push    bx                      ;          x,
                push    noarg$y[2]              ;          y,
                push    noarg$y[0]              ;
                push    cfile                   ;          pFile,
                mov     ax,FALSE                ;          fInsert
                push    ax                      ;
                call    REPLACE                 ;                  )
    nochange:   jmp     short done

                ; Loop over lines in box, changing case per cArg.
  doboxarg:     mov     ax,boxarg$yTop[0]       ; init current row
                mov     row[0],ax
                mov     ax,boxarg$yTop[2]
                mov     row[2],ax

    dobox:      call    xgetline                ; get current line

                ; Process cx chars at linebuf[bx], where bx=xLeft and
                ; cx=min(xRight+1,length)-xLeft.  If cx<=1 skip line.
                ; ax presently holds line length returned by GETLINE.
                mov     bx,boxarg$xLeft
                mov     cx,boxarg$xRight
                inc     cx
                cmp     ax,cx                   ; xRight beyond eol?
                ja      @F                      ;   no  -- use xRight
                mov     cx,ax                   ;   yes -- use length
        @@:     sub     cx,bx                   ; any chars to process?
                jbe     nextln                  ;   no

                ; Process the line buffer.  Upcase if cArg > 1.
                mov     si,offset linebuf
                add     si,bx
                mov     ax,'AZ'                 ; toggle A..Z (lowcase)
                cmp     boxarg$cArg,1
                je      ccase
                mov     ax,'az'                 ; toggle a..z (upcase)
    ccase:      cmp     byte ptr [si],ah
                jb      @F
                cmp     byte ptr [si],al
                ja      @F
                xor     byte ptr [si],020h      ; toggle case
        @@:     inc     si
                loop    ccase

                ; Replace current line with processed buffer.
                call    xputline

                ; Bump line index, and check whether we're done.
    nextln:     mov     ax,row[0]
                mov     dx,row[2]
                add     ax,1
                adc     dx,0
                mov     row[0],ax
                mov     row[2],dx
                cmp     dx,boxarg$yBottom[2]    ; beyond last line?
                jg      done
                cmp     ax,boxarg$yBottom[0]
                jg      done
                jmp     short dobox

                ; Return TRUE
  done:         mov     ax,TRUE
                ret

Changecase      endp
              endif


; Pdelword
;      Deletes the character under the cursor and any characters
;      immediately following which are in the same class. There are two
;      main character classes: alphanumeric characters and space. Others
;      (including tab and end-of-line) are in classes of their own.
;      Deleted text is not copied to the clipboard. The deletions are
;      undoable.
;
; Always returns TRUE.

              ifndef nopdelword
Pdelword        proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Init current row and get current line into buffer.
                mov     ax,noarg$y[0]
                mov     row[0],ax
                mov     ax,noarg$y[2]
                mov     row[2],ax
                call    xgetline

                ; Get buffer pointer into ds:si.
                mov     si,offset linebuf

                ; Cursor beyond last char of current line? If so, set up
                ; to delete line break. Line length returned by GETLINE
                ; is in ax.
                cmp     noarg$x,ax              ; col < line length?
                jl      @F
                add     row[0],1                ; increment row
                adc     row[2],0
                jmp     short delete

                ; Set buffer pointer to starting col.
        @@:     add     si,noarg$x

                ; Determine class of current char, scan forward to next
                ; char not in same class (or NUL at end of line), and
                ; set up to delete to that char.
                mov     al,[si]                 ; get char at cursor
                cmp     al,' '                  ;   and classify it
                jz      space
                call    isdigit
                jz      alnum
                call    isalpha
                jz      alnum

                ; Char at cursor is in class of its own.
                inc     si
                jmp     short delete

                ; Char at cursor is a space.
  space:        inc     si
                cmp     byte ptr [si],' '
                jz      space
                jmp     short delete

                ; Char at cursor is alphanumeric.
  alnum:        inc     si
                mov     al,[si]
                call    isdigit
                jz      alnum
                call    isalpha
                jz      alnum

  delete:       sub     si,offset linebuf
                push    cfile                   ; DELSTREAM( pFile,
                push    noarg$x                 ;            xStart,
                push    noarg$y[2]              ;            yStart,
                push    noarg$y[0]              ;
                push    si                      ;            xEnd,
                push    row[2]                  ;            yEnd
                push    row[0]                  ;
                call    DELSTREAM               ;                   )

                ; Return TRUE
                mov     ax,TRUE
                ret

Pdelword        endp
              endif


; Quik
;      I attach this function to ctrl+Q to implement the Wordstar quick
;      cursor movement commands. It is a replacement for the WS.ZXT
;      extension supplied with the editor. Changes the interpretation
;      of ctrl+Q S|D|E|X|R|C slightly, and does not support ctrl+Q
;      A|F|K|Y. Avoids the need to load two extension files, saving
;      time and memory, and lets me make ctrl+Q do exactly what I want
;      (even assign its functionality to a different key than ctrl+Q).
;
; Returns the value of the command string executed.

              ifndef noquik
Quik            proc    far pascal \
                        argData: word, pArg: far ptr, fMeta: word

                ; Get next keystroke. Look at scan code in ah, put
                ; offset of appropriate command string in cx, and
                ; execute it. The scan code for an alpha key is same
                ; whether or not ctrl, shift, alt have been pressed.

                call    READCHAR

                mov     cx,offset Quik_E_cmd
                cmp     ah,012h                 ; E
                je      @F

                mov     cx,offset Quik_R_cmd
                cmp     ah,013h                 ; R
                je      @F

                mov     cx,offset Quik_S_cmd
                cmp     ah,01fh                 ; S
                je      @F

                mov     cx,offset Quik_D_cmd
                cmp     ah,020h                 ; D
                je      @F

                mov     cx,offset Quik_X_cmd
                cmp     ah,02dh                 ; X
                je      @F

                mov     cx,offset Quik_C_cmd
                cmp     ah,02eh                 ; C
                je      @F

                mov     cx,offset nulstr        ; default (empty string)

                ; Execute (possibly NUL) command and return what it returns.
        @@:     mov     bx,@data                ; segment of command string
                push    bx
                push    cx                      ; offset          "
                call    FEXECUTE
                ret

Quik            endp
              endif


; Transpose
;      Exchanges the character under the cursor with the following
;      character. If the cursor is at or beyond the last character on
;      the line, then the last two characters on the line are
;      exchanged.
;
; Always returns TRUE.

              ifndef notranspose
Transpose       proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Init row.
                mov     ax,noarg$y[0]
                mov     row[0],ax
                mov     ax,noarg$y[2]
                mov     row[2],ax

                ; Get current line, compare length with cursor column,
                ; and act accordingly.
                call    xgetline
                sub     ax,1
                jle     done                    ; < 2 chars on line?
                mov     bx,noarg$x
                cmp     bx,ax
                jl      change                  ; cursor inside line?
                mov     bx,ax                   ; cursor at/beyond last char
                dec     bx

  change:       mov     ax,word ptr linebuf[bx]
                xchg    al,ah
                mov     word ptr linebuf[bx],ax
                call    xputline

  done:         mov     ax,TRUE
                ret

Transpose       endp
              endif


; Arg linearg Fill
; Arg numarg Fill
; Arg markarg Fill     [the area referenced must be a linearg]
;
;      Fills the selected lines by rearranging text to the right of the
;      cursor to fit as many words as possible on each line between the
;      cursor column and the fill margin. The default fill margin is at
;      column 72 (this can be changed by assigning a different value to
;      the numeric switch "fillmargin"). Words are delimited by spaces.
;      Extra spaces and lines are lost after filling. If a single word
;      is too long to fit between the cursor column and the fill margin
;      then the word will spill over the margin.
;
; Returns FALSE if the argument is invalid, or if the fillmargin is
; less than the initial cursor column or greater than BUFLEN. Otherwise
; returns TRUE.
;
; Tip: Fill can be used to break a group of lines into a list of words.
;      Set fillmargin to 1, mark the lines, and Fill.
;
; Bugs:
;      If the editor switch "trailspace" is on, then trailing spaces
;      are treated as a word.
;
;      Lines which end up "empty" after filling are deleted, even if
;      they contain text to the left of the cursor.
;
;      After a call to Fill, only that one Fill can be undone; the rest
;      of the undo stack is lost (I don't know why).


              ifndef nofill
Fill            proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                local   leftmarg: word, ffile: word, \
                        fcol: word, frow_lo: word, frow_hi: word

                call    setup

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Set left margin to initial cursor column.
                call    xgetcursor
                mov     ax,col
                mov     leftmarg,ax

                ; If fillmargin out of range, return FALSE.
                mov     ax,FALSE
                mov     bx,fillmargin
                cmp     bx,BUFLEN
                jle     @F
                jmp     exit
        @@:     cmp     bx,leftmarg
                jg      @F
                jmp     exit

                ; Store a zero in si for various uses below.
        @@:     zero    si

                ; Create Fill pseudo file.
                push    es
                push    ds
                mov     ax,offset fillfile_name
                push    ax
                call    ADDFILE
                pop     es
                mov     ffile,ax

                ; Copy any text to left of leftmarg to fill file.
                mov     bx,leftmarg
                sub     bx,1
                jbe     @F
                push    es
                push    cfile                   ; COPYBOX( pFileSrc,
                push    ffile                   ;          pFileDst,
                push    si                      ;          xLeft, = 0
                push    linearg$yStart[2]       ;          yTop,
                push    linearg$yStart[0]       ;
                push    bx                      ;          xRight,
                push    linearg$yEnd[2]         ;          yBottom,
                push    linearg$yEnd[0]         ;
                push    si                      ;          xDst,  = 0
                push    si                      ;          yDst   = 0L
                push    si                      ;
                call    COPYBOX                 ;                    )
                pop     es

                ; Init current row and col for getword. Bias current
                ; row to first selected line - 1, and set col to -1 to
                ; signal that a new line must be read.
        @@:     mov     ax,linearg$yStart[0]
                mov     dx,linearg$yStart[2]
                sub     ax,1
                sbb     dx,0
                mov     row[0],ax
                mov     row[2],dx
                mov     col,-1

                ; Init fill row and col.
                mov     frow_lo,si              ; 0L
                mov     frow_hi,si
                mov     bx,leftmarg
                mov     fcol,bx

                ; Build filled block in fill file. While getword
                ; returns word length in cx > 0, write word at
                ; linebuf[si] to fill file. When a row is filled
                ; bump row index.

  dofill:       mov     cx,leftmarg
                call    getword
                cmpz    cx
                jz      cleanup

                ; Would appending this word exceed the fill margin?
                mov     bx,fcol
                add     bx,cx
                cmp     bx,fillmargin
                jle     @F

                ; Yes, so reset column, and bump row.
                mov     bx,leftmarg
                mov     fcol,bx
                add     frow_lo,1
                adc     frow_hi,0

                ; Append word.
        @@:     push    es
                push    cfile                   ; COPYSTREAM( pFileSrc,
                push    ffile                   ;             pFileDst,
                mov     bx,si                   ;             xStart,
                push    bx                      ;
                mov     ax,row[0]               ;             yStart,
                mov     dx,row[2]               ;
                push    dx                      ;
                push    ax                      ;
                add     bx,cx                   ;             xEnd,
                push    bx                      ;
                push    dx                      ;             yEnd,
                push    ax                      ;
                push    fcol                    ;             xDst,
                inc     cx                      ; skip a space after word
                add     fcol,cx                 ;
                push    frow_hi                 ;             yDst
                push    frow_lo                 ;
                call    COPYSTREAM              ;                        )
                pop     es
                jmp     short dofill

                ; Delete the original lines.
  cleanup:      push    es
                push    cfile                   ; DELLINE( pFile,
                push    linearg$yStart[2]       ;          yStart,
                push    linearg$yStart[0]       ;
                push    linearg$yEnd[2]         ;          yEnd
                push    linearg$yEnd[0]         ;
                call    DELLINE                 ;                 )
                pop     es

                ; Insert filled lines in current file.
                zero    ax
                push    es
                push    ffile                   ; COPYLINE( pFileSrc,
                push    cfile                   ;           pFileDst,
                push    ax                      ;           yStart, = 0L
                push    ax                      ;
                push    frow_hi                 ;           yEnd,
                push    frow_lo                 ;
                push    linearg$yStart[2]       ;           yDst
                push    linearg$yStart[0]       ;
                call    COPYLINE                ;                    )
                pop     es

                ; Delete fill file and return TRUE.
                push    ffile
                call    REMOVEFILE
                mov     ax,TRUE
  exit:         ret

Fill            endp


getword         proc

                ; Called by Fill. Scans selected text from left margin
                ; forward for next word. If no more words returns 0 in
                ; cx, otherwise returns word length in cx, and si
                ; indexes start of word in linebuf. Expects to receive
                ; left margin in cx.

                ; Need new line?
                mov     si,col
                cmp     si,-1
                jne     skipspace

                ; Yes. Reset si.
                mov     si,cx                   ; cx=left margin

                ; If already at end of selected text then fail.
  nextline:     mov     ax,row[0]
                mov     dx,row[2]
                add     ax,1
                adc     dx,0
                cmp     dx,linearg$yEnd[2]
                jg      fail
                cmp     ax,linearg$yEnd[0]
                jg      fail
                mov     row[0],ax
                mov     row[2],dx

                 ; Get line into linebuf.
                call    xgetline

                ; If line empty, try next line.
                cmp     ax,si
                jle     nextline

                ; We've got a line. Bias si.
                dec     si

                ; Get next word.
  skipspace:    inc     si
                cmp     byte ptr linebuf[si],' '
                je      skipspace
                mov     bx,si                   ; save beginning of word
  scanword:     cmp     byte ptr linebuf[si],0  ; end of line?
                je      iseol
                inc     si
                cmp     byte ptr linebuf[si],' '; end of word?
                jne     scanword
                jmp     short noteol

  iseol:        mov     col,-1
                jmp     short done

  noteol:       mov     col,si

  done:         mov     cx,si
                sub     cx,bx
                mov     si,bx
                ret

  fail:         zero    cx
                ret

getword         endp
              endif


; [Meta] Number
;      This function is used to generate integer sequences. Each time
;      it is called, Number inserts a string representing the current
;      value of "numcount" as a base "numbase" integer at the cursor
;      position, then increments numcount by the amount of "numstep"
;      (which may be negative, in which case numcount is decremented).
;      If the Meta prefix is used then the number is right adjusted in
;      a field "numwidth" columns wide, padded with the character of
;      ASCII value "numpad" (if the number is too long for the field
;      then numwidth is ignored).
;
; [Meta] Arg Number
;      As above, except that numcount is not incremented.
;
; [Meta] Arg Arg Number
;      As in first paragraph above, except that numcount is first
;      initialized to zero.
;
; Always returns TRUE.
;
; Numeric switches        Default value        Legal values
; recognized by Number
; --------------------    ----------------     -----------------------
;      numcount                 0              -32768..32767
;      numstep                  1              -32768..32767
;      numwidth                 6              1..BUFLEN
;      numbase                 10              2..36
;      numpad                  32  (space)     1..255
;
; Bugs:
;      Expect weirdness if switches are set out of range. If numbase is
;      out of range, Number will use the default base 10 to avoid fatal
;      math errors. If numpad is zero, then if padding is used, the NUL
;      padding character(s) will truncate the current line before the
;      number is inserted. If numpad is otherwise out of range the
;      padding character is unpredictable. If numwidth is out of range
;      it will be ignored. If numcount is incremented out of range it
;      will simply wrap around, changing its sign. If you undo Number,
;      numcount will NOT be decremented by the undo.

              ifndef nonumber
Number          proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get cursor position.
                call    xgetcursor

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Get numcount into ax.
                mov     ax,numcount

                ; If no arg, continue.
                test    argType$,NULLARG
                jz      bumpnum

                ; If arg count = 1, don't bump numcount.
                cmp     nullarg$cArg,1
                je      buildnum

                ; Arg count > 1, so zero numcount.
                zero    ax
                mov     numcount,ax

                ; Increment numcount.
  bumpnum:      mov     bx,numstep
                add     numcount,bx

                ; Build number in linebuf.
  buildnum:     zero    si
                zero    cx                      ; assume no minus sign

                ; numcount positive?
                cmpz    ax
                jge     @F

                ; No, so note this, and flip sign.
                inc     cx                      ; we do need a minus sign
                neg     ax

                ; If numbase is out of range, use default radix 10.
        @@:     mov     bx,numbase
                cmp     bx,36
                jg      @F
                cmp     bx,2
                jl      @F
                jmp     short gendigits
        @@:     mov     bx,10

                ; Generate digits in reverse order.
  gendigits:    cwd
                idiv    bx
                add     dl,'0'
                cmp     dl,'9'
                jle     @F
                add     dl,'a'-'0'-10
        @@:     mov     linebuf[si],dl
                inc     si
                cmpz    ax
                jnz     gendigits

                ; Put minus sign if needed.
                cmpz    cx
                jz      padnum
                mov     linebuf[si],'-'
                inc     si

                ; Pad if requested and needed.
  padnum:       cmp     word ptr fMeta,1
                jne     putnum
                mov     cx,numwidth
                sub     cx,si
                jle     putnum
                mov     ax,si                   ; don't pad if width too great
                add     ax,cx
                cmp     ax,BUFLEN
                jg      putnum
                mov     ax,numpad
        @@:     mov     linebuf[si],al
                inc     si
                loop    @B

                ; Put to file.
  putnum:       dec     si
                push    word ptr linebuf[si]    ; REPLACE( c,
                push    col                     ;          x,
                push    row[2]                  ;          y,
                push    row[0]                  ;
                push    cfile                   ;          pFile,
                mov     ax,TRUE                 ;          fInsert
                push    ax                      ;
                call    REPLACE                 ;                  )
                inc     col
                cmpz    si
                jg      putnum

                ; Return.
                mov     ax,TRUE
                ret

Number          endp
              endif


; [Meta] Enqueue
;      Copies the current line to the end of the queue.
;
; Arg [Arg] [Meta] linearg Enqueue
; Arg [Arg] [Meta] numarg Enqueue
; Arg [Arg] [Meta] markarg Enqueue [area referenced must be a linearg]
;      Copies the selected lines to the end of the queue. If the extra
;      Arg prefix is supplied, then the selected lines are deleted
;      after they are copied (the deleted lines are not copied to the
;      built-in editor clipboard).
;
; If the Meta prefix is supplied then the contents of the queue are
; erased before the new text is appended.
;
; Returns FALSE if the argument is invalid and TRUE otherwise.
;
; See also the Dequeue function.
;
; The queue is a pseudo file maintained by the Enqueue and Dequeue
; functions. I use it to gather scattered lines together: Enqueue the
; lines in turn, then Dequeue them all at the desired location. The
; queue may be used to reverse the order of a group of lines: Enqueue
; the lines, then Meta Dequeue them one at a time. The queue is also
; handy as an extra clipboard (but note that the queue is strictly
; line-oriented).

              ifndef noqueue
Enqueue         proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get queue file handle.
                call    getqueue

                ; If Meta then clear queue.
                cmp     word ptr fMeta,1
                jne     @F
                push    qfile
                call    DELFILE

                ; Get index of next available row in queue.
        @@:     push    qfile
                call    FILELENGTH
                mov     row[0],ax
                mov     row[2],dx

                ; Clear si to indicate no delete after copy.
                zero    si

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Test arg type and act accordingly.
                test    argType$,LINEARG
                jnz     dolinearg

                ; Must be NOARG.
  donoarg:      mov     ax,noarg$y[0]
                mov     bx,noarg$y[2]
                mov     cx,ax
                mov     dx,bx
                jmp     short copy

                ; It's a LINEARG.
  dolinearg:    cmp     linearg$cArg,2
                jl      @F
                inc     si                      ; do delete after copy
        @@:     mov     ax,linearg$yStart[0]
                mov     bx,linearg$yStart[2]
                mov     cx,linearg$yEnd[0]
                mov     dx,linearg$yEnd[2]

                ; Copy lines to end of queue.
  copy:         push    ax
                push    bx
                push    cx
                push    dx
                push    cfile                   ; COPYLINE( pFileSrc,
                push    qfile                   ;           pFileDst,
                push    bx                      ;           yStart,
                push    ax                      ;
                push    dx                      ;           yEnd,
                push    cx                      ;
                push    row[2]                  ;           yDst
                push    row[0]                  ;
                call    COPYLINE                ;                    )
                pop     dx
                pop     cx
                pop     bx
                pop     ax

                ; Delete lines just copied, if requested.
                cmpz    si
                jz      done
                push    cfile                   ; DELLINE( pFile,
                push    bx                      ;          yStart,
                push    ax                      ;
                push    dx                      ;          yEnd
                push    cx                      ;
                call    DELLINE                 ;                 )

  done:         mov     ax,TRUE
                ret

Enqueue         endp


; Dequeue
;      Inserts the contents of the queue at the current line.
;
; Meta Dequeue
;      Inserts the first line of the queue at the current line and
;      removes the line from the queue.
;
; Returns FALSE if queue is empty and TRUE otherwise.
;
; See the definition of the Enqueue function for more information
; about the queue functions and the queue file.

Dequeue         proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                call    setup

                ; Get queue file handle.
                call    getqueue

                ; Get queue length into ax:dx
                push    qfile
                call    FILELENGTH

                ; Abort if queue empty.
                cmpz    dx
                jg      @F
                cmpz    ax
                jg      @F
                mov     ax,FALSE
                jmp     short exit

                ; Get ARG struct pointer into es:di
        @@:     les     di,pArg

                ; If Meta, set up to copy the first line.
                cmp     word ptr fMeta,1
                jne     @F
                zero    ax
                zero    dx
                jmp     short copy

                ; No Meta, so set up to copy the whole queue.
        @@:     sub     ax,1
                sbb     dx,0

                ; Copy as specified.
  copy:         zero    si
                push    qfile                   ; COPYLINE( pFileSrc,
                push    cfile                   ;           pFileDst,
                push    si                      ;           yStart,
                push    si                      ;
                push    dx                      ;           yEnd,
                push    ax                      ;
                push    noarg$y[2]              ;           yDst
                push    noarg$y[0]              ;
                call    COPYLINE                ;                    )

                ; If Meta, delete head of queue.
                cmp     word ptr fMeta,1
                jne     done
                push    qfile                   ; DELLINE( pFile,
                push    si                      ;          yStart,
                push    si                      ;
                push    si                      ;          yEnd
                push    si                      ;
                call    DELLINE                 ;                 )

  done:         mov     ax,TRUE
  exit:         ret

Dequeue         endp


getqueue        proc

                ; Loads queue file handle into qfile. Creates file if
                ; needed. Does not preserve es.

                mov     si,offset queuefile_name

                push    ds
                push    si
                zero    ax
                push    ax
                push    ax
                call    FILENAMETOHANDLE
                cmpz    ax                      ; NULL?
                jnz     done

                push    ds
                push    si
                call    ADDFILE

  done:         mov     qfile,ax
                ret

getqueue        endp
              endif


WhenLoaded      proc

              ifndef nomsg
                mov     ax,@data
                push    ax
                mov     ax,offset linebuf       ; signon msg is in linebuf
                push    ax
                call    DOMESSAGE
              endif

                ret

WhenLoaded      endp


                end


-------------------

The following is an alternate version of Changecase that accepts a
streamarg rather than a boxarg. Since this code was last tested, many
changes have been made to the ARG macros, global variables, utility
functions, etc, so it **may no longer work**. It is here simply as an
example of one way to write a function that accepts a streamarg.

; Arg streamarg Changecase
;      Changes upper-case chars in specified text to lower-case.
;
; Arg Arg streamarg Changecase
;      Changes lower-case chars in specified text to upper-case.
;
; Always returns TRUE.

Changecase      proc    far pascal uses si di ds, \
                        argData: word, pArg: far ptr, fMeta: word

                local   lnflag:word

                mov     ax,@data
                mov     ds,ax

                ; Get ARG struct pointer into es:di
                les     di,pArg

                ; Get current file handle.
                pushfp  nulstr
                pushfp  NULL
                call    FILENAMETOHANDLE
                mov     cfile,ax

                ; Loop over lines in stream, changing case per cArg.
                ; Observe start col on first line, and end col on last
                ; (these may be the same line).
                mov     ax,streamarg$yStart[0]  ; get starting row
                mov     row[0],ax
                mov     ax,streamarg$yStart[2]
                mov     row[2],ax
                mov     lnflag,1                ; 1=first line,2=last,0=other

  dostream:     push    es
                push    row[2]                  ; GETLINE( line,
                push    row[0]                  ;
                pushfp  linebuf                 ;          buf,
                push    cfile                   ;          pfile
                call    GETLINE                 ;                )
                pop     es

                ; We're going to process cx chars at linebuf[bx].
                ; Initiallize cx and bx according to which line we're
                ; on.  Begin by assuming this is a middle line (bx=0).
                ; ax presently holds line length returned by GETLINE.
                mov     cx,ax
                zero    bx

                mov     ax,row[2]
                cmp     ax,streamarg$yEnd[2]    ; last line?
                jb      @F
                mov     ax,row[0]
                cmp     ax,streamarg$yEnd[0]
                jb      @F
                or      lnflag,2                ; set "last line" bit
                mov     cx,streamarg$xEnd

        @@:     test    lnflag,1                ; first line?
                jz      @F
                xor     lnflag,1                ; clear "first line" bit
                mov     bx,streamarg$xStart

        @@:     sub     cx,bx
                jz      nextln                  ; skip empty lines

                ; Process the line buffer.  Upcase if cArg > 1.
                mov     si,offset linebuf
                add     si,bx
                cmp     streamarg$cArg,1
                ja      @F
                mov     ax,'ZA'                 ; set up to locase
                jmp     short ccase
        @@:     mov     ax,'za'                 ; set up to upcase
    ccase:      cmp     byte ptr [si],al
                jb      @F
                cmp     byte ptr [si],ah
                ja      @F
                xor     byte ptr [si],020h      ; toggle case
        @@:     inc     si
                loop    ccase

                ; Replace current line with processed buffer.
                push    es
                push    row[2]                  ; PUTLINE( line,
                push    row[0]                  ;
                pushfp  linebuf                 ;          buf,
                push    cfile                   ;          pfile
                call    PUTLINE                 ;                )
                pop     es

                ; Bump line index, and check whether we're done.
  nextln:       add     row[0],1
                adc     row[2],0
                test    lnflag,2                ; was last line processed?
                jz      dostream

                ; Return TRUE
                mov     ax, TRUE
                ret

Changecase      endp
