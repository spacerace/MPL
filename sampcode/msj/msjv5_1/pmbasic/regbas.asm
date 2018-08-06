
; Program Name: RegBas.ASM
;
; Functions :
;           BreakLong
;           MakeLong
;           BasClientWndProc
;           RegBas
;
; Description : This supplies interface routines for BASIC
programs.
;               RegBas is used to register window classes
from BASIC.
;               Also provided are the utility routines:
BreakLong and MakeLong.

DOSSEG
.286                               ; OS/2 only runs on 286
machines
.model medium, PASCAL              ; Medium memory model
when interfacing with BASIC
                                   ; PASCAL and BASIC use
same calling conventions

.data

hwndb1    DW  ?                    ; set up local data for
BasClientWndProc
hwndb2    DW  ?
msgb      DW  ?
mp1b1     DW  ?
mp1b2     DW  ?
mp2b1     DW  ?
mp2b2     DW  ?

.code

EXTRN     ClientWndProc:PROC       ; BASIC function to call

PUBLIC BasClientWndProc, BreakLong, MakeLong, RegBas

; Breaklong(Long, hi_word, lo_word)
;
BreakLong  PROC FAR USES bx, long1:WORD, long2:WORD,
hiword:WORD, loword:WORD
           mov bx, long1           ; get the hi-byte of the
long
           mov ax, bx
           mov bx, hiword          ; return it to hiword
           mov [bx], ax
           mov bx, long2           ; get the lo-byte
           mov ax, bx
           mov bx, loword          ; return it to loword
           mov [bx], ax
           ret

BreakLong  ENDP
; MakeLong&(hi_word, low_word)
;
MakeLong   PROC FAR USES bx, hiword:WORD, loword:WORD
           mov bx, hiword
           mov ax, bx
           mov dx, ax              ; return hi-word in dx
           mov bx, loword          ; lo-word in ax for
function
           mov ax, bx              ; returning a long
           ret
MakeLong   ENDP

BasClientWndProc    PROC hwnd1:word, hwnd2:word, msg:word,
mp11:word, mp12:word, mp21:word, mp22:word

           push ds
           mov  ax, @data          ; get our data segment
           mov  ds, ax

           mov  ax, hwnd2          ; transfer the values
passed
           mov  hwndb1, ax         ; from PM to local
variables
           mov  ax, hwnd1          ; for the call to BASIC
           mov  hwndb2, ax
           mov  ax, msg
           mov  msgb, ax
           mov  ax, mp12
           mov  mp1b1, ax
           mov  ax, mp11
           mov  mp1b2, ax
           mov  ax, mp22
           mov  mp2b1, ax
           mov  ax, mp21
           mov  mp2b2, ax

           mov  ax, OFFSET hwndb1  ; set up for call to
BASIC
           push ax                 ; BASIC expects values to
           mov  ax, OFFSET msgb    ; be passed by reference.
           push ax
           mov  ax, OFFSET mp1b1
           push ax
           mov  ax, OFFSET mp2b1
           push ax

           call ClientWndProc      ; call BASIC routine -
note
                                   ; return values are
already
           pop  ds                 ; in dx, ax so we don't
have
           ret                     ; to do anything.
BasClientWndProc ENDP

RegBas     PROC
           mov dx, SEG BasClientWndProc      ; return
address of
           mov ax, OFFSET BasClientWndProc   ; BASIC
routine.
           ret
RegBas     ENDP
           end
