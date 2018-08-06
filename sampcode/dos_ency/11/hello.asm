;
; Name:          hello
;
; Description:   This RAM-resident (terminate-and-stay-resident) utility
;                displays the message "Hello, World" in response to a
;                software interrupt.
;
; Comments:      Assemble and link to create HELLO.EXE.
;
;                Execute HELLO.EXE to make resident.
;
;                Execute  INT 64h  to display the message.
;


TSRInt           EQU     64h
STDOUT           EQU     1

RESIDENT_TEXT    SEGMENT byte public 'CODE'
                 ASSUME  cs:RESIDENT_TEXT,ds:RESIDENT_DATA

TSRAction        PROC    far

                 sti                    ; enable interrupts

                 push    ds             ; preserve registers
                 push    ax
                 push    bx
                 push    cx
                 push    dx

                 mov     dx,seg RESIDENT_DATA
                 mov     ds,dx
                 mov     dx,offset Message      ; DS:DX -> message
                 mov     cx,16                  ; CX = length
                 mov     bx,STDOUT              ; BX = file handle
                 mov     ah,40h                 ; AH = INT 21h function 40h
                                                ; (Write File)
                 int     21h                    ; display the message

                 pop     dx                     ; restore registers and exit
                 pop     cx
                 pop     bx
                 pop     ax
                 pop     ds
                 iret

TSRAction        ENDP

RESIDENT_TEXT    ENDS


RESIDENT_DATA    SEGMENT word public 'DATA'

Message          DB      0Dh,0Ah,'Hello, World',0Dh,0Ah

RESIDENT_DATA    ENDS


TRANSIENT_TEXT   SEGMENT para public 'TCODE'
                 ASSUME  cs:TRANSIENT_TEXT,ss:TRANSIENT_STACK

HelloTSR PROC    far                    ; At entry:     CS:IP -> SnapTSR
                                        ;               SS:SP -> stack
                                        ;               DS,ES -> PSP
; Install this TSR's interrupt handler

                 mov     ax,seg RESIDENT_TEXT
                 mov     ds,ax
                 mov     dx,offset RESIDENT_TEXT:TSRAction
                 mov     al,TSRInt
                 mov     ah,25h
                 int     21h

; Terminate and stay resident

                 mov     dx,cs          ; DX = paragraph address of start of
                                        ; transient portion (end of resident
                                        ; portion)
                 mov     ax,es          ; ES = PSP segment
                 sub     dx,ax          ; DX = size of resident portion

                 mov     ax,3100h       ; AH = INT 21h function number (TSR)
                                        ; AL = 00H (return code)
                 int     21h

HelloTSR         ENDP

TRANSIENT_TEXT   ENDS


TRANSIENT_STACK  SEGMENT word stack 'TSTACK'

                 DB      80h dup(?)

TRANSIENT_STACK  ENDS


                 END     HelloTSR
