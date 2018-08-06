; ReadName.Asm - reads a list of file names into a BASIC string array

;Syntax -
;
; Spec$ = "*.*" + CHR$(0)
; CALL ReadFile(Spec$, BYVAL VARPTR(FileName$(0)))
;
; Where Spec$ holds the directory specification, and elements in the
; FileName$() array receive the directory entries.

.286
.Model Medium, Basic
.Data

    FileJunk DB 22 Dup (?)
    NameLen  DB ?
    FileName DB 13 Dup (?)

.Code
    Extrn DosFindFirst: Proc  ;declare the OS/2 calls as external
    Extrn DosFindNext:  Proc
    Extrn DosFindClose: Proc

ReadNames Proc, Spec:Ptr, Array:Ptr

    Local Handle:   Word
    Local MaxCount: Word

    Mov  SI,Spec          ;SI holds address of Spec$ descriptor
    Mov  SI,[SI+02]       ;now SI points to the first character
    Mov  BX,Array         ;get address of FileName$(1) for later

    Mov  Handle, -1       ;directory handle of -1 to initiate search
    Mov  MaxCount,1       ;maximum number of entries to find

    Push DS               ;push segment of Spec$
    Push SI               ;push offset of Spec$ data
    Push SS               ;push segment of Handle in local storage
    Lea  AX,Handle        ;get address of Handle
    Push AX               ;push it
    Push 00100111b        ;the attribute for any type of file
    Push DS               ;segment for the buffer
    Push Offset FileJunk  ;push buffer address
    Push 36               ;push length of buffer
    Push SS               ;push segment of MaxCount
    Lea  AX,MaxCount      ;get address of MaxCount
    Push AX               ;push it
    Push 0                ;push a DWord of 0's
    Push 0
    Call DosFindFirst     ;make call to find first matching file name
    Or   AX,AX            ;was there a file by that name?
    Jnz  Exit             ;no, exit

    Push DS               ;ensure that ES points to BASIC's string
    Pop  ES               ; space for Movsb instructions below

NextFile:
    Cmp  Word Ptr [BX],12 ;is the string at least 12 characters long?
    Jb   Done             ;no, so get out now
    Mov  DI,[BX+02]       ;DI holds address of first character in 
                          ;FileName$()
    Lea  SI,FileName      ;get address of the name portion of buffer
    Mov  CL,NameLen       ;load CL with the number of characters to
                          ;transfer
    Xor  CH,CH            ;clear CH so we can use all of CX
    Rep  Movsb            ;copy the string
    Add  BX,4             ;point BX to next array element for later

    Push SS:Handle        ;push handle
    Push DS               ;push segment of buffer
    Push Offset FileJunk  ;and buffer address
    Push 36               ;push length of result buffer
    Push SS               ;push segment of MaxCount
    Lea  AX,MaxCount      ;get address of MaxCount
    Push AX               ;push it
    Call DosFindNext      ;make call to find next matching file name
    Or   AX,AX            ;did we get another one?
    Jz   NextFile         ;if so, keep looking

Done:
    Push SS:Handle        ;push the handle number
    Call DosFindClose     ;free it up

Exit:
    Ret                   ;return to BASIC

ReadNames Endp
End
