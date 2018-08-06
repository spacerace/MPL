;----- GetDir.Asm - retrieves current directory

;Syntax -
;
; Drive$ = "A"
; Dir$ = GetDir$(Drive$)
; PRINT "Current directory is: " Dir$
;
; Where Drive$ = "" for the default drive, or "A" or "a" or "B", etc.
; and Dir$ receives the current directory path.  If an error occurs,
; Dir$ will be assigned to a null string.

.286
.Model Medium, Basic
.Data
    DescrLen   DW ?
    DescrAdr   DW Offset DirName
    DirName    DB "\", 64 Dup (?)

.Code
    Extrn DosQCurDir:Proc    ;declare the OS/2 call as external

GetDir Proc, Drive:Ptr

    Local LenName:Word       ;create a local variable

    Cld                      ;clear direction flag so 
                             ;string moves are forward
    Mov  DescrLen, 0         ;assume a null string in case 
                             ;there's an error
    Mov  LenName,65          ;tells OS/2 the available buffer length


    Mov  DI,Drive            ;put address of Drive$ into DI
    Mov  DX,[DI]             ;assume they want the default 
                             ;drive for now
    Or   DX,DX               ;check LEN(Drive$)
    Je   Do_It               ;it's null, leave DX holding 0 
                             ;and skip ahead

    Mov  DI,[DI+02]          ;put address of first character 
                             ;in Drive$ into DI
    Mov  DL,[DI]             ;put ASC(Drive$) into DL
    Cmp  DL,"a"              ;is it less than "a"?
    Jb   Continue            ;yes, skip
    Sub  DL,32               ;no, convert to upper case

Continue:
    Sub  DL,64               ;"A" now equals 1, "B" = 2, etc.

Do_It:
    Mov  DI,Offset DirName   ;get address for the string descriptor
    Inc  DI                  ;bump past the leading backslash
    Mov  SI,DI               ;save it in SI too for later

    Push DX                  ;pass the drive number
    Push DS                  ;show which segment receives the name

    Push DI                  ;and which address
    Push SS                  ;pass segment for LenName 
                             ;(SS because it's local)
    Lea  CX,LenName          ;get address for LenName
    Push CX                  ;and pass that too

    Call DosQCurDir          ;ask OS/2 to do the dirty work
    Or   AX,AX               ;was there an error?
    Jnz  Exit                ;yes, get out with DescrLen 
                             ;showing a null string

AddLen:                      ;scan for zero byte that marks the end
    Inc  DescrLen            ;show the string as being one longer
    Lodsb                    ;get a character from directory string
    Or   AL,AL               ;are we at the end of directory name?
    Jnz  AddLen              ;no, keep searching

Exit:
    Mov  AX,Offset DescrLen  ;tell BASIC where to find the descriptor
    Ret                      ;return to BASIC

GetDir Endp
End
