; FCount.Asm - counts the number of files that match a specification

;Syntax -
;
; Spec$ = "*.*" + CHR$(0)
; Count = FileCount%(Spec$)
; Where Spec$ holds the file specification, and Count is assigned the
; number of files that match.

.286
.Model Medium, Basic
.Code
    Extrn DosFindFirst:Proc	; Declare the OS/2 calls as external
    Extrn DosFindNext: Proc
    Extrn DosFindClose:Proc

FileCount Proc, Spec:Ptr

    Local Handle:      Word
    Local Buffer [18]: Word   ;36 bytes
    Local SearchCount: Word

    Xor  BX,BX            ;zero the count accumulator
    Mov  SI,Spec          ;SI holds address of Spec$ descriptor
    Mov  DX,[SI+02]       ;now DX points to the first character

    Mov  Handle, -1       ;directory handle of -1 to initiate search
    Mov  SearchCount,1    ;number of entries to find

    Push DS               ;push segment of Spec$
    Push DX               ;push offset of Spec$
    Push SS               ;push segment of local Handle
    Lea  AX,Handle        ;get Handle's address
    Push AX               ;push it too
    Push 00100111b        ;push attribute - all file types 
                          ;(except directory)
    Push SS               ;segment of buffer
    Lea  AX,Buffer [18]   ;get address of buffer
    Push AX               ;push it
    Push 36               ;push length of buffer
    Push SS               ;push segment of SearchCount
    Lea  AX,SearchCount   ;get address of SearchCount
    Push AX               ;push it
    Push 0                ;push a DWord of 0's (Reserved& in 
                          ;BASIC version)
    Push 0
    Call DosFindFirst     ;make call to find first occurrence of file
    Or   AX,AX            ;was there a file by that name?
    Jnz  Exit             ;if AX is not zero, then no file was found

Next_File:
    Inc  BX               ;show that we got another matching file

    Push Handle           ;push handle
    Push SS               ;push segment of buffer
    Lea  AX,Buffer [18]   ;get address of buffer
    Push AX               ;push it
    Push 36               ;push length of buffer
    Push SS               ;push segment of SearchCount
    Lea  AX,SearchCount   ;get address of SearchCount
    Push AX               ;push it
    Call DosFindNext      ;make call to find first occurrence of file
    Or   AX,AX            ;was there a file by that name?
    Jz   Next_File        ;if not carry, keep looking

Done:
    Push Handle           ;push handle
    Call DosFindClose     ;close it

Exit:
    Mov  AX,BX            ;assign the function output
    Ret                   ;return to BASIC

FileCount Endp
End
