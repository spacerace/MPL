;----- SetDrive.Asm - sets the default drive

;Syntax -
;

;   CALL SetDrive(Drive$)
;   where Drive$ = "A" or "a" or "B", etc.

.286
.Model Medium, Basic
.Code
    Extrn DosSelectDisk:Proc ;declare the OS/2 call as external

SetDrive Proc, Drive:Ptr

    Mov  SI,Drive         ;put Drive$ descriptor address into SI
    Cmp  Word Ptr [SI],0  ;is Drive$ null?
    Jz   Exit             ;yes, ignore the request

    Mov  SI,[SI+02]       ;put address of Drive$ data in SI
    Mov  DL,[SI]          ;put ASC(Drive$) into DL

    Cmp  DL,'a'           ;is it below "a"?
    Jb   Continue         ;yes, skip
    Sub  DL,32            ;no, convert to upper case

Continue:
    Sub  DL,64            ;"A" now equals 1, "B" = 2, etc.
    Xor  DH,DH            ;clear DH so we can use all of DX
    Push DX               ;push drive number
    Call DosSelectDisk    ;call OS/2

Exit:
    Ret                   ;return to BASIC

SetDrive Endp
End
